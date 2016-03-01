hDbl    dq  0.0

macro isInteger thestr {
        local   .startloop, .finish, .notvalid, .exit, .actual_loop
        local   .not_plus, .not_minus, .not_null, .to_continue
        push    ebx ecx esi
        mov     bl, FALSE
        mov     esi, thestr
        invoke  lstrlenW, esi
        mov     ecx, eax
    .startloop:
        cmp     word [esi], '+'
        ;jne     @f
        jne     .not_plus
        inc2    esi
        dec     ecx
        jmp     .actual_loop
    ;@@:
    .not_plus:
        cmp     word [esi], '-'
        ;jne     @f
        jne     .not_minus
        inc2    esi
        dec     ecx
        jmp     .actual_loop
    ;@@:
    .not_minus:
    .actual_loop:
        cmp     word [esi], 0
        ;je      .finish
        ;jne     @f
        jne     .not_null
        cmp     bl, TRUE
        je      .finish
        jmp     .notvalid
    ;@@:
    .not_null:
        cmp     bh, TRUE
        ;jne     @f
        jne     .to_continue
        mov     bh, TRUE
    ;@@:
    .to_continue:
        cmp     word [esi], '0'
        jb      .notvalid
        cmp     word [esi], '9'
        ja      .notvalid
        inc2    esi
        dec     ecx
        jnz     .actual_loop
    .finish:
        xor     eax, eax
        jmp     .exit
    .notvalid:
        mov     eax, 1
    .exit:
        pop     esi ecx ebx
}

macro isFloat thestr {
    local .startloop, .finish, .notvalid, .exit
    local .next1, .not_e, .not_E
        push    ebx ecx esi
        mov     esi, thestr
        xor     eax, eax
        mov     bl, FALSE
        mov     bh, FALSE
        invoke  lstrlenW, esi
        mov     ecx, eax
        cmp     word [esi], '-'
        jne     @f
        dec     ecx
        inc2    esi
        jmp     .startloop
    @@:
        cmp     word [esi], '+'
        jne     @f
        dec     ecx
        inc2    esi
        jmp     .startloop
    @@:
    .startloop:
        cmp     word [esi], 0
        jne     @f
        cmp     bh, TRUE
        je      .finish
        jmp     .notvalid
    @@:
        cmp     word [esi], '.'
        jne     .next1
        cmp     bl, TRUE
        jne     @f
        jmp     .notvalid
    @@:
        mov     bl, TRUE
        inc2    esi
        dec     ecx
        jnz     .startloop
    .next1:
        cmp     word [esi], 'e'
        jne     .not_e
        inc2    esi
        dec     ecx
        isInteger esi
        cmp     eax, 1
        je      .notvalid
        jmp     .finish
    .not_e:
        cmp     word [esi], 'E'
        jne     .not_E
        inc2    esi
        dec     ecx
        isInteger esi
        cmp     eax, 1
        je      .notvalid
        jmp     .finish
    .not_E:
        cmp     bh, TRUE
        jne     @f
        mov     bh, TRUE
    @@:
        cmp     word [esi], '0'
        jb      .notvalid
        cmp     word [esi], '9'
        ja      .notvalid
        inc2    esi
        dec     ecx
        jnz     .startloop
    .finish:
        xor     eax, eax
        jmp     .exit
    .notvalid:
        mov     eax, 1
    .exit:
        pop     esi ecx ebx
}


proc convertToInt uses esi ebx ecx edx, intstr, digits
        xor     eax, eax
        mov     esi, [intstr]
        mov     ecx, [digits]
        mov     dl, FALSE
        cmp     word [esi], '-'
        jne     .theloop
        mov     dl, TRUE
        inc     esi
        dec     ecx
    .theloop:
        cmp     word [esi], 0
        je      .exit
        movzx   ebx, word [esi]
        inc2    esi
        sub     ebx, '0'
        imul    eax, 10
        cmp     dl, TRUE
        jne     @f
        sub     eax, ebx
        loop    .theloop
        jmp     .exit
    @@:
        add     eax, ebx
        loop    .theloop
    .exit:
        ret
endp

proc convertToDouble uses esi edi ebx ecx edx, dblstr, digits
        locals
            hTemp   dq  10.0
            h2      dd  0.0
        endl
        xor     eax, eax
        xor     ebx, ebx
        mov     esi, [dblstr]
        mov     ecx, [digits]
        finit
        cld
        fldz                    ; st0 = 0.0
        mov     dh, FALSE       ; act as decimal switcher
        mov     dl, FALSE       ; act as sign switcher
        cmp     word [esi], '-'
        jne     .theloop
        inc2    esi
        dec     ecx
        mov     dl, TRUE
    .theloop:
        cmp     word [esi], 0
        je      .exit
        cmp     word [esi], 'e'
        jne     @f
        jmp     .exponent
    @@:
        cmp     word [esi], 'E'
        jne     @f
        jmp     .exponent
    @@:
        cmp     word [esi], '.'
        jne     @f
        mov     dh, TRUE
        fstp    [hDbl]
        finit
        fldz                    ; st0 = 0
        inc2    esi
        loop    .theloop
    @@:
        movzx   eax, word [esi]
        sub     eax, '0'
        mov     [h2], eax
        inc2    esi
        cmp     dh, TRUE        ; if decimals
        je      @f
        fld     [hTemp]         ; st0 = 10, st1 = hdbl
        fmulp                   ; st0 = (htemp * 10)
        fild    [h2]            ; st0 = h2, st1 = (hdbl * 10)
        faddp                   ; st0 = (hdbl * 10) + h2
        loop    .theloop
        jmp     .exit
    @@:
        inc     ebx
        fild    [h2]            ; st0 = 10, st1 = fordec
        push    ecx
        mov     ecx, ebx
    @@:
        fld     [hTemp]         ; st0 = hTemp, st1 = 10, st2 = fordec
        fdivp                   ; st0 = (10 / hTemp), st1 = fordec
        dec     ecx
        jnz     @b
        faddp                   ; st0 = (10 / hTemp) + fordec
        pop     ecx
        loop    .theloop
        fld     [hDbl]
        faddp
    .exit:
        ;.if dl, e, TRUE
            ;fchs
        ;.endif
        cmp     dl, TRUE
        jne     @f
        fchs
    @@:
        ;fstp    [hDbl]
        fst     [hDbl]
        ret
    .exponent:
        xor     ebx, ebx
        xor     eax, eax
        inc2    esi
        dec     ecx
        cmp     dh, TRUE
        jne     @f
        fld     [hDbl]
        faddp
      @@:
        mov     dh, FALSE
      .procexp:
        cmp     word [esi], 0
        je      .exit
        cmp     word [esi], '-'
        jne     @f
        mov     dh, TRUE
        inc2    esi
        dec     ecx
        jmp     .procexp
      @@:
        cmp     word [esi], '+'
        jne     @f
        inc2    esi
        dec     ecx
        jmp     .procexp
      @@:
        movzx   ebx, word [esi]
        inc2    esi
        sub     ebx, '0'
        imul    eax, 10
        add     eax, ebx
        loop    .procexp
        push    ecx
        mov     ecx, eax
      .mulexp:
        cmp     dh, TRUE
        jne     @f
        fld     [hTemp]
        fdivp
        loop    .mulexp
        jmp     .fin
      @@:
        fld     [hTemp]
        fmulp
        loop    .mulexp
      .fin:
        pop     ecx
        jmp     .exit
endp

proc convertFromInteger uses edi eax ebx edx, thenum, outstr
        xor     edx, edx
        mov     cl, FALSE
        mov     eax, [thenum]
        mov     edi, [outstr]
        cmp     eax, 0
        jns     @f
        mov     cl, TRUE
        neg     eax
    @@:
        mov     ebx, 10
        cdq
    @@:
        cmp     eax, -1
        je      .exit
        cmp     eax, 10
        jl      @f
        cdq
        idiv    ebx
        add     dx, 0x30
        mov     word [edi], dx
        inc2    edi
        jmp     @b
    @@:
        add     ax, 0x30
        mov     word [edi], ax
        inc2    edi
    .exit:
        cmp     cl, TRUE
        jne     @f
        mov     word [edi], '-'
        inc2    edi
    @@:
        mov     word [edi], 0
        stdcall reverseString, [outstr]
        ret
endp

proc reverseString uses esi edi eax ecx edx, thestr
        invoke  lstrlenW, [thestr]
        mov     ecx, eax
        cmp     eax, 1
        jbe     .exit
        mov     esi, [thestr]
        mov     edi, esi
        shl     eax, 1
        shr     ecx, 1
        add     edi, eax
        sub     edi, 2
    @@:
        mov     ax, word [esi]
        mov     dx, word [edi]
        mov     word [esi], dx
        mov     word [edi], ax
        inc2    esi
        dec2    edi
        dec     ecx
        jnz     @b
    .exit:
        ret
endp

;; TODO: to complete the implementation
proc convertFromDouble uses esi edi eax ebx ecx edx, value, holder
        local exponent:WORD
        mov     ecx, [value]
        mov     edx, [value+4]
        mov     edi, [holder]
        shr     edx, 16     ; shift left to check the sign
        test    dx, 8000h   ; 1000 0000 0000 0000, looking for sign
        jz      @f
        mov     word [edi], '-'
        inc2    edi
    @@:
        and     dx, 7FFFh   ; 0111 1111 1111 1111, clear sign bit
        shr     dx, 4       ; shift left 4 to give exponent value
        sub     dx, 1023    ; sub with exponent bias
        mov     dword [exponent], dx
    .exit
        ret
endp
