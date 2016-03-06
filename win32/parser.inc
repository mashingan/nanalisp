proc checkInput uses esi eax, instr
        mov     esi, [instr]
    .startloop:
        cmp     word [esi], 0
        je      .endloop
        cmp     word [esi], ';'
        je      .nextloop
        cmp     word [esi], '"'
        jne     @f
        cmp     word [esi-2], '\'       ; Check whether escape
        je      .nextloop
        cmp     byte [aphostrophs], FALSE
        jne     .aphostrophs
        mov     byte [aphostrophs], TRUE
        jmp     .nextloop
      .aphostrophs:
        mov     byte [aphostrophs], FALSE
        jmp     .nextloop
    @@:
        cmp     byte [aphostrophs], TRUE
        je      .nextloop
        cmp     word [esi], '('
        jne     @f
        inc     [parenthesis]
    @@:
        cmp     word [esi], ')'
        jne     .nextloop
        dec     [parenthesis]
    .nextloop:
        inc2    esi
        jmp     .startloop
    .endloop:
        ret
endp

proc getAllChars uses esi edi ebx, instr, outstr, chr:WORD
        mov     esi, [instr]
        mov     edi, [outstr]
        cld
        movsw
        mov     ebx, 1
        mov     ax, [chr]
    .startloop:
        cmp     word [esi], 0
        je      .exit
        cmp     word [esi], ax
        jne     @f
        cmp     word [esi-2], '\'   ; check whether escape
        je      @f
        jmp     .lastloop
    @@:
        inc     ebx
        movsw
        jmp     .startloop
    .lastloop:
        movsw
        inc     ebx
        mov     word [edi], 0
    .exit:
        mov     eax, ebx
        ret
endp

proc getToken uses esi edi ebx ecx, instr, outstr
        mov     esi, [instr]
        mov     edi, [outstr]
        xor     ebx, ebx
    .startloop:
        cmp     word [esi], 0
        je      .endloop
        cmp     word [esi], ' '
        jne     @f
        inc2    esi
        inc     ebx
        jmp     .startloop
    @@:
        cmp     word [esi], '"'
        jne     @f
        stdcall getString, esi, edi
        add     ebx, eax
        lea     eax, [eax*2]
        add     esi, eax
        add     edi, eax
        jmp     .endloop
    @@:
        cmp     word [esi], '('
        jne     @f
        stdcall getWithinParenthesis, esi, edi
        add     ebx, eax
        lea     eax, [eax*2]
        add     esi, eax
        add     edi, eax
        jmp     .endloop
    @@:
        stdcall getSymbol, esi, edi
        add     ebx, eax
        lea     eax, [eax*2]
        add     esi, eax
        add     edi, eax
    .endloop:
        mov     word [edi], 0
        mov     eax, ebx
        ret
endp

macro isChar thestr {
    local .exit_ischar
        inc2    esi
        cmp     word [esi], '\'
        jne     .exit_ischar
        inc2    esi
        inc2    esi
        cmp     word [esi], 0
        jne     .exit_ischar
        mov     eax, CHAR
        jmp     .exit
    .exit_ischar:
}

proc identify uses esi ebx ecx, thestr
        mov     eax, UNDEF
        mov     esi, [thestr]
        mov     bl, FALSE
        mov     bh, FALSE
        cld
    .startloop:
        cmp     word [esi], 0
        je      .exit
        cmp     word [esi], '('
        jne     @f
        mov     eax, LIST
        jmp     .exit
    @@:
        cmp     word [esi], '#'
        jne     @f
        isChar  esi
        mov     eax, SYMBOL
        jmp     .exit
    @@:
        cmp     word [esi], '.'
        ;je      .dotproc
        je      .procnum
        cmp     word [esi], '+'
        je      .procnum
        cmp     word [esi], '-'
        je      .procnum
        cmp     word [esi], '"'
        jne     @f
        mov     eax, STRING
        jmp     .exit
    @@:
        cmp     word [esi], '9'
        jng     @f
        mov     eax, SYMBOL
        jmp     .exit
    @@:
        cmp     word [esi], '0'
        jnl     @f
        mov     eax, SYMBOL
        jmp     .exit
    ;.dotproc:
        ;inc2    esi
        ;dec     ecx
    @@:
    .procnum:
        isInteger esi
        cmp     eax, 0
        jne     @f
        invoke  lstrlenW, [thestr]
        cmp     eax, INT32_LEN
        jle     .is_integer
        mov     eax, LONG
        jmp     .exit
      .is_integer:
        mov     eax, INTEGER
        jmp     .exit
    @@:
        isFloat esi
        cmp     eax, 0
        jne     @f
        mov     eax, FLOAT
        jmp     .exit
    @@:
        mov     eax, SYMBOL
    .exit:
        ret
endp

proc getSymbol uses esi edi ebx, instr, outstr
        mov     esi, [instr]
        mov     edi, [outstr]
        xor     eax, eax
        mov     bl, FALSE
        cld
    .startloop:
        cmp     word [esi], 0
        je      .endloop
        cmp     word [esi], '('
        je      .endloop
        cmp     word [esi], ')'
        je      .endloop
        cmp     word [esi], ' '
        jne     @f
        cmp     bl, TRUE
        je      .endloop
        inc2    esi
        jmp     .startloop
    @@:
        cmp     bl, TRUE
        je      @f
        mov     bl, TRUE
    @@:
        movsw
        mov     word [edi], 0
        inc     eax
        jmp     .startloop
    .endloop:
        ret
endp

proc getString uses esi edi ebx ecx, instr, outstr
        mov     esi, [instr]
        mov     edi, [outstr]
        cld
        mov     ebx, 0
        stdcall getAllChars, esi, edi, '"'
        add     ebx, eax
        lea     eax, [eax*2]
        add     esi, eax
        add     edi, eax
        mov     eax, ebx
        inc2    edi
        mov     word [edi], 0
        ret
endp

proc getWithinParenthesis uses esi edi ebx ecx, instr, outstr
        mov     esi, [instr]
        mov     edi, [outstr]
        cld
        movsw
        mov     ebx, 1
        mov     ecx, 1
    .startloop:
        cmp     word [esi], 0
        je      .endloop
        jecxz   .endloop
        cmp     word [esi], '('
        jne     @f
        inc     ecx
        inc     ebx
        movsw
        jmp     .startloop
    @@:
        cmp     word [esi], ')'
        jne     @f
        dec     ecx
        inc     ebx
        movsw
        jmp     .startloop
    @@:
        cmp     word [esi], '"'
        jne     @f
        stdcall getString, esi, edi
        add     ebx, eax
        lea     eax, [eax*2]
        add     esi, eax
        add     edi, eax
        jmp     .startloop
    @@:
        movsw
        inc     ebx
        jmp     .startloop
    .endloop:
        inc2    edi
        mov     word [edi], 0
        mov     eax, ebx
        ret
endp

proc getUntilChar uses esi edi ecx, thestr, thechar:WORD
        mov     esi, [thestr]
        mov     cx, [thechar]
        xor     eax, eax
    @@:
        cmp     word [esi], 0
        je      @f
        cmp     word [esi], cx
        je      @f
        inc2    esi
        inc     eax
        jmp     @b
    @@:
        ret
endp

proc normalizeInput uses esi edi eax, thestr
        mov     esi, [thestr]
        xor     eax, eax
    .startloop:
        cmp     word [esi], 0
        je      .exit
        cmp     word [esi], '"'
        jne     @f
        stdcall getString, esi, [hDummy]
        shl     eax, 1
        add     esi, eax
        jmp     .startloop
    @@:
        cmp     word [esi], 10
        jne     @f
        mov     word [esi], ' '
        cmp     word [esi-2], 13
        jne     .nextchar
        mov     word [esi-2], ' '
        jmp     .nextchar
    @@:
        cmp     word [esi], ';'
        jne     @f
        stdcall getUntilChar, esi, 10
        mov     ecx, eax
        shl     eax, 1
        mov     edi, esi
        add     esi, eax
        mov     word [esi], ' '
        push    edi esi
      .internalloop:
        mov     ax, word [esi]
        mov     word [edi], ax
        inc2w   esi, edi
        cmp     ax, 0
        jne     .internalloop
        pop     esi edi
        mov     esi, edi
    @@:
    .nextchar:
        inc2    esi
        jmp     .startloop
    .exit:
        ret
endp
