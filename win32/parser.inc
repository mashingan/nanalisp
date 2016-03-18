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

macro togettoken [funcname, thechar] {
    common x = 0
    forward x = x + 1
    common b = 0
    forward
        b = b + 1
    @@:
    if ~ thechar eq
        cmp     word [esi], thechar
        jne     @f
    end if
        stdcall get#funcname, esi, edi
        add     ebx, eax
        lea     eax, [eax*2]
        add     esi, eax
        add     edi, eax
    if b <> x
        jmp     .endloop
    end if
}

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
    togettoken\
        String, '"',\
        WithinParenthesis, '(',\
        Symbol
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

macro appendstring dst, src {
    local .start_copying, .stop_copying
        lea     edx, src
      .start_copying:
        mov     ax, [edx]
        cmp     ax, 0
        je      .stop_copying
        mov     [dst], ax
        inc2w   dst, edx
        jmp     .start_copying
      .stop_copying:
}

;macro ischeckconvert [chr] {
        ;cmp     word [esi], chr
        ;je      .exit
;}

proc isConvertAgain uses esi, thestr
        mov     eax, TRUE
        mov     esi, [thestr]
        ;ischeckconvert "'", '`', ',', '@'
        stdcall identify, esi
        cmp     eax, LIST
        jne     .is_false
        mov     eax, TRUE
        jmp     .exit
    .is_false:
        mov     eax, FALSE
    .exit:
        ret
endp

macro toconvertagain towhere, [chr] {
        cmp     word [edi], chr
        je      towhere
}

macro appendquotes [chr, quotetype] {
    local .not_list, .close_prnt, .convert_again
    @@:
        cmp     word [edi], chr
        jne     @f
        inc2    edi
        appendstring esi, [quotetype]
        toconvertagain .convert_again, "'", '`', ',', '@'
        push    edi
        stdcall getToken, edi, [bufdum]
        mov     ebx, eax
        stdcall isConvertAgain, [bufdum]
        cmp     eax, TRUE
        jne     .not_list
        stdcall convertToQuote, [bufdum]
        mov     edi, [bufdum]
        invoke  lstrlenW, edi
        mov     ecx, eax
        xchg    esi, edi
        rep     movsw
        xchg    esi, edi
        pop     edi
        lea     ebx, [ebx*2]
        add     edi, ebx
        jmp     .close_prnt
    .convert_again:
        mov     [bufdum], edi
        stdcall convertToQuote, [bufdum]
        invoke  lstrlenW, [bufdum]
        mov     ecx, eax
        xchg    esi, edi
        rep     movsw
        xchg    esi, edi
        jmp     .close_prnt
    .not_list:
        pop     edi
        xchg    esi, edi
        mov     ecx, ebx
        rep     movsw
        xchg    esi, edi
    .close_prnt:
        appendstring esi, [clsprnt]
        jmp     .next_loop
    @@:
}

proc convertToQuote uses esi edi eax ebx ecx edx, thestring
        locals
            buffer  dd  0
            bufdum  dd  0
            thq     du  '(quote ',0
            thqq    du  '(quasiquote ',0
            thuq    du  '(unquote ',0
            thuqs   du  '(unquote-splicing ',0
            clsprnt du  ')',0
        endl
        memalloc MAXBUFFER shl 1
        mov     [bufdum], eax
        memalloc MAXBUFFER shl 1
        mov     [buffer], eax
        mov     esi, [thestring]
        invoke  lstrlenW, [thestring]
        mov     ecx, eax
        mov     edi, [buffer]
        rep     movsw
        mov     esi, [thestring]
        mov     edi, [buffer]
    .start_loop:
        cmp     word [edi], 0
        je      .end_loop
        appendquotes\
            "'", thq,\
            '`', thqq
        cmp     word [edi], ','
        jne     @f
        cmp     word [edi+2], '@'
        jne     .not_splicing
        inc2    edi
        appendquotes '@', thuqs
        jmp     .next_loop
      .not_splicing:
        appendquotes ',', thuq
    @@:
    .next_loop:
        mov     ax, [edi]
        mov     word [esi], ax
        inc2w   esi, edi
        jmp     .start_loop
    .end_loop:
        mov     word [edi+2], 0
        memfree [buffer]
        ret
endp
