;; This is a snipped from start label after jne @b

;===================================================
;; This part of allocating memory
;; enable list variable in data section

        stdcall allocateNode, buffin, 0
        stdcall addNode, list, eax
        stdcall listNodes, [outhnd], list
        stdcall removeNode, eax

;===================================================
;; Below is for printing all token

        mov     esi, buffin
        invoke  lstrlen, buffin
        cmp     word [esi], '('
        jne     @f
        sub     eax, 3
        inc2    esi
    @@:
        mov     ecx, eax
    @@:
        cmp     ecx, 0
        jle     @f
        cmp     word [esi], 0
        je      @f
        clearBuffer bufout, 128
        stdcall getToken, esi, bufout
        push    eax ecx esi edi
        mov     esi, bufout
        cmp     word [esi], 0
        je      .notneeded
        stdcall identify, bufout
        shl     eax, 2
        mov     edi, [nametype+eax]
        cinvoke wprintf, typefmt, bufout, edi
        newline [outhnd]
        clearBuffer dummy, 128
      .notneeded:
        pop     edi esi ecx eax
        mov     ebx, eax
        lea     eax, [eax*2]
        add     esi, eax
        sub     ecx, ebx
        jnz     @b
    @@:

;===================================================
;; This part is for converting ascii to double

        invoke  lstrlen, bufout
        stdcall convertToDouble, bufout, eax
        cinvoke printf, fltfmt, dword [hDbl], dword [hDbl+4]


;===================================================
;; This part is in getSymbol procedure before .startloop label

        cmp     byte [esi], "'"
        jne     @f
        movsb
        inc     eax
        mov     byte [edi], 0
    @@:
        cmp     byte [esi], '`'
        jne     @f
        movsb
        inc     eax
        mov     byte [edi], 0
    @@:

;===================================================
;; This part is from convert.inc

macro checkIsValidInt thestr, looptime {
        local .notvalid, .quit
        push    esi ecx
        mov     esi, thestr
        mov     ecx, looptime
        sub     ecx, 2
        cmp     byte [esi], '-'
        jne     @f
        dec     ecx
        inc     esi
    @@:
        cmp     byte [esi], 0
        je      @f
        cmp     byte [esi], 48
        jb      .notvalid
        cmp     byte [esi], 57
        ja      .notvalid
        inc     esi
        loop    @b
    @@:
        xor     eax, eax
        jmp     .quit
    .notvalid:
        mov     eax, 1
    .quit:
        pop     ecx esi
}

;===================================================
;; This part is for printing the STRINGSTRUCT, taken from inputting_v2.asm
        ;clearBuffer dummy, 128
        stdcall getToken, buffin, dummy
        writeLn [outhnd], dummy
        stdcall allocateString, dummy
        mov     ebx, eax
        lea     esi, [eax+STRINGSTRUCT.text]
        invoke  WriteConsole, dword [outhnd], esi,\
            dword [eax+STRINGSTRUCT.size], numwrtn, 0
        invoke  GlobalFree, [ebx+STRINGSTRUCT.hStr]
        FAILCHK


;===================================================
;; This part is for printing the STRINGSTRUCT, taken from inputting_v2.asm
;; This is the v2 using VirtualAlloc instead of GlobalAlloc

        stdcall getToken, buffin, dummy
        writeLn [outhnd], dummy
        stdcall allocateString, dummy
        mov     ebx, eax
        lea     esi, [eax+STRINGSTRUCT.text]
        invoke  WriteConsole, dword [outhnd], esi,\
            dword [eax+STRINGSTRUCT.size], numwrtn, 0
        mov     eax, 8
        add     eax, dword [ebx+STRINGSTRUCT.size]
        invoke  VirtualFree, dword [ebx+STRINGSTRUCT.hStr],\
            eax, MEM_RELEASE
        FAILCHK

;===================================================
;; This part is for printing the STRINGSTRUCT, taken from inputting_v2.asm
;; This is the v3 using HeapAlloc instead of VirtualAlloc

        stdcall getToken, buffin, dummy
        writeLn [outhnd], dummy
        stdcall allocateString, dummy
        mov     ebx, eax
        lea     esi, [eax+STRINGSTRUCT.text]
        invoke  WriteConsole, dword [outhnd], esi,\
            dword [eax+STRINGSTRUCT.size], numwrtn, 0
        invoke  GetProcessHeap
        invoke  HeapFree, eax, HEAP_NO_SERIALIZE,\
            dword [ebx+STRINGSTRUCT.hStr]


;===================================================
;; This part is for printing the SYMBOLSTRUCT, taken from inputting_v2.asm
;; According to allocateString v3

        stdcall getToken, buffin, dummy
        writeLn [outhnd], dummy
        stdcall allocateSymbol, dummy
        mov     ebx, eax
        mov     esi, [eax+SYMBOLSTRUCT.hString]
        lea     eax, [esi+STRINGSTRUCT.text]
        invoke  WriteConsole, [outhnd], eax,\
            dword [esi+STRINGSTRUCT.size], numwrtn, 0
        invoke  GetProcessHeap
        invoke  HeapFree, eax, HEAP_NO_SERIALIZE, [esi+STRINGSTRUCT.hStr]
        invoke  GlobalFree, [ebx+SYMBOLSTRUCT.hSymbol]


;===================================================
;; This part is for printing the SYMBOLSTRUCT, taken from inputting_v2.asm
;; for SYMBOLSTRUCT v2

        stdcall getToken, buffin, dummy
        writeLn [outhnd], dummy
        stdcall allocateSymbol, dummy
        mov     ebx, eax
        mov     esi, [eax+SYMBOLSTRUCT.hString]
        lea     eax, [esi+STRINGSTRUCT.text]
        invoke  WriteConsole, [outhnd], eax,\
            dword [esi+STRINGSTRUCT.size], numwrtn, 0
        invoke  GetProcessHeap
        invoke  HeapFree, eax, HEAP_NO_SERIALIZE, [esi+STRINGSTRUCT.hStr]
        invoke  GetProcessHeap
        invoke  HeapFree, eax, HEAP_NO_SERIALIZE, [ebx+SYMBOLSTRUCT.hNodes]
        invoke  GetProcessHeap
        invoke  HeapFree, eax, HEAP_NO_SERIALIZE, [ebx+SYMBOLSTRUCT.hSymbol]

;===================================================
;; This part is for allocating the heap and processing based on the token
;; produced

        invoke  GetProcessHeap
        mov     [procHeap], eax
        stdcall getToken, buffin, dummy
        stdcall identify, dummy
        cmp     eax, SYMBOL
        jne     @f
        stdcall allocateSymbol, dummy
        stdcall printSymbol, [outhnd], eax
        stdcall removeSymbol, eax
        jmp     .finish
    @@:
        cmp     eax, STRING
        jne     @f
        stdcall allocateString, dummy
        ;memval  [outhnd], eax
        stdcall printString, [outhnd], eax
        ;memval  [outhnd], eax
        stdcall removeString, eax
        jmp     .finish
    @@:
        cmp     eax, LIST
        jne     @f
        stdcall allocateNode, dummy, eax
        stdcall printNode, [outhnd], eax
        stdcall removeNode, eax
        jmp     .finish
    @@:
    .finish:


;===================================================
;; This part is for testing quote proc which yields 2 results in eax as
;; pointer and ebx as the identification

        stdcall getToken, buffin, dummy
        stdcall quote, dummy
        cmp     ebx, SYMBOL
        jne     @f
        stdcall printSymbol, [outhnd], eax
        stdcall removeSymbol, eax
        jmp     .finish
    @@:
        cmp     ebx, STRING
        jne     @f
        stdcall printString, [outhnd], eax
        stdcall removeString, eax
        jmp     .finish
    @@:
        cmp     ebx, LIST
        jne     @f
        stdcall printNode, [outhnd], eax
        stdcall removeNode, eax
        jmp     .finish
    @@:
    .finish:


;===================================================
;; This part is for testing the NUMBERSTRUCT but printNumber still not
;; implemented yet

        isInteger buffin
        cmp     eax, 0
        jne     @f
        stdcall allocateNumber, buffin, INTEGER
        jmp     .finish
    @@:
        isFloat buffin
        cmp     eax, 0
        jne     .finish
        stdcall allocateNumber, buffin, FLOAT
    .finish:
        stdcall removeNumber, eax


;===================================================
;; This part is for testing the makelist proc and printNumber still not
;; implemented yet

        stdcall getToken, buffin, dummy
        stdcall identify, dummy
        cmp     eax, SYMBOL
        jne     @f
        stdcall allocateSymbol, dummy
        stdcall printSymbol, [outhnd], eax
        stdcall removeSymbol, eax
        jmp     .finish
    @@:
        cmp     eax, STRING
        jne     @f
        stdcall allocateString, dummy
        stdcall printString, [outhnd], eax
        stdcall removeString, eax
        jmp     .finish
    @@:
        cmp     eax, LIST
        jne     @f
        stdcall makelist, dummy
        stdcall printList, [outhnd], eax
        stdcall removeList, eax
        jmp     .finish
    @@:
        cmp     eax, INTEGER
        jne     @f
        stdcall allocateNumber, dummy, eax
        stdcall removeNode, eax
        jmp     .finish
    @@:
        cmp     eax, FLOAT
        jne     @f
        stdcall allocateNumber, dummy, eax
        stdcall removeNode, eax
        jmp     .finish
    @@:
    .finish:


;===================================================
;; This part is for testing the various cpy structures and its subsequent
;; procedures, enable testnm1 and testnm2 at data segment

        stdcall allocateNumber, testnm1, INTEGER
        mov     esi, [eax+NUMBERSTRUCT.value]
        checkItr [esi]
        mov     ebx, eax
        stdcall cpyNumber, eax
        mov     esi, [eax+NUMBERSTRUCT.value]
        checkItr [esi]
        stdcall removeNumber, eax
        stdcall removeNumber, ebx
        stdcall allocateNumber, testnm2, FLOAT
        mov     ebx, eax
        mov     esi, [eax+NUMBERSTRUCT.value]
        push    eax
        cinvoke printf, fltfmt, dword [esi], dword [esi+4]
        pop     eax
        stdcall cpyNumber, eax
        mov     esi, [eax+NUMBERSTRUCT.value]
        push    eax
        cinvoke printf, fltfmt, dword [esi], dword [esi+4]
        pop     eax
        stdcall removeNumber, eax
        stdcall removeNumber, ebx

;===================================================
;; This part is for testing the various cpy structures and its subsequent
;; procedures, enable testexpression in data segment

        stdcall makelist, testexpression
        mov     ebx, eax
        stdcall printList, [outhnd], eax
        newline [outhnd]
        mov     ebx, [eax+NODE.next]
        mov     esi, [ebx+NODE.value]
        stdcall printSymbol, [outhnd], esi
        spacing [outhnd]
        stdcall cpySymbol, esi
        stdcall printSymbol, [outhnd], eax
        newline [outhnd]
        stdcall removeSymbol, eax
        mov     ebx, [ebx+NODE.next]
        stdcall printSymbol, [outhnd], [ebx+NODE.value]
        spacing [outhnd]
        stdcall cpyNode, ebx
        mov     esi, [eax+NODE.value]
        stdcall printSymbol, [outhnd], esi
        stdcall removeNode, eax
        mov     ebx, [ebx+NODE.next]
        newline [outhnd]
        stdcall printList, [outhnd], ebx
        stdcall cpyList, ebx
        newline [outhnd]
        stdcall printList, [outhnd], eax
        newline [outhnd]
        stdcall removeList, eax
        stdcall removeList, ebx
    .finish:

;===================================================
;; This part is for testing the setq procedure and its subsequent procedures
;; such as allocateVariable

        stdcall makelist, testexpression
        mov     ebx, eax
        stdcall printList, [outhnd], eax
        newline [outhnd]
        mov     ecx, [eax+NODE.next]
        stdcall setq, eax, ecx
        stdcall removeList, ebx
        stdcall checkVARTABLE
        stdcall allocateSymbol, gettest
        stdcall allocateNode, eax, SYMBOL
        mov     ecx, eax
        stdcall symbolLength, eax
        cmp     eax, 32
        jng     @f
        mov     eax, 32
    @@:
        dec     eax
        shl     eax, 2
        lea     esi, [VARTABLE+eax]
        mov     esi, [esi]
        stdcall lookup, esi, ecx
        mov     ebx, eax
        cmp     eax, 0
        je      .finish
        mov     esi, [eax+VARIABLESTRUCT.hVar]
        stdcall printSymbol, [outhnd], [esi+NODE.value]
        spacing [outhnd]
        mov     esi, [eax+VARIABLESTRUCT.hValue]
        stdcall printSymbol, [outhnd], [esi+NODE.value]
        newline [outhnd]
        ;; If below 2 instructions are not disabled, it will result in
        ;; error due to invalid argument feed to HeapFree. The error
        ;; is happened because of double-release between removeString
        ;; in invocation of removeSymbol and removeVariable respectively.
        ;stdcall removeSymbol, [esi+NODE.value]
        ;stdcall removeNode, [eax+VARIABLESTRUCT.hVar]
        stdcall removeVariable, ebx
        stdcall checkVARTABLE
    .finish:

;===================================================
;; This part is for testing the readExpression procedure with additional
;; newly implemented printNumber

        stdcall readExpression, buffin
        checkItr [eax+NODE.type]
        stdcall printNode, [outhnd], eax
        newline [outhnd]

;===================================================
;; This part is for testing the various newly linked-list procedures aside
;; the addNode, such as: pushNode, popNode. But pushNode, and popNode
;; still don't update the node value itself and need to be updated manually

        stdcall readExpression, testexpression
        mov     ebx, eax
        stdcall printNode, [outhnd], eax
        newline [outhnd]
        stdcall readExpression, gettest
        mov     ecx, eax
        stdcall pushNode, [ebx+NODE.value], eax
        mov     [ebx+NODE.value], eax
        stdcall printNode, [outhnd], ebx
        newline [outhnd]
        stdcall readExpression, gettest
        stdcall addNode, [ebx+NODE.value], eax
        stdcall printNode, [outhnd], ebx
        newline [outhnd]
        stdcall popNode, [ebx+NODE.value]
        mov     [ebx+NODE.value], eax
        stdcall printNode, [outhnd], ebx
        newline [outhnd]
        stdcall removeNode, ebx

;===================================================
;; This part is for testing the cons procedure together with newly
;; implemented eval in cons

        stdcall readExpression, testexpression
        mov     ebx, eax
        stdcall printNode, [outhnd], eax
        newline [outhnd]
        mov     eax, [eax+NODE.value]
        stdcall cons, eax, [eax+NODE.next]
        mov     ecx, eax
        stdcall printNode, [outhnd], eax
        newline [outhnd]
        stdcall removeNode, ebx
        stdcall removeNode, eax

;===================================================
;; This part is for testing the quote procedure

        stdcall readExpression, testexpression
        mov     ebx, eax
        stdcall printNode, [outhnd], eax
        newline [outhnd]
        mov     eax, [eax+NODE.value]
        stdcall quote, eax, [eax+NODE.next]
        stdcall printNode, [outhnd], eax
        newline [outhnd]
        stdcall removeNode, ebx
        stdcall removeNode, eax

;===================================================
;; This part is for testing the lookup procedure to ensure it still can
;; read listed node

        stdcall readExpression, testexpression
        mov     ebx, eax
        stdcall printNode, [outhnd], eax
        newline [outhnd]
        mov     eax, [eax+NODE.value]
        stdcall setq, eax, [eax+NODE.next]
        stdcall readExpression, gettest
        mov     ecx, eax
        stdcall printNode, [outhnd], eax
        newline [outhnd]
        lea     esi, [VARTABLE+12]
        mov     esi, [esi]
        stdcall printNode, [outhnd], [eax+NODE.value]
        newline [outhnd]
        stdcall lookup, esi, [eax+NODE.value]
        mov     edx, eax
        stdcall removeNode, ebx
        stdcall removeNode, ecx
        cmp     eax, NULL
        je      .finish
        mov     ebx, [eax+VARIABLESTRUCT.hValue]
        stdcall printNode, [outhnd], ebx
        newline [outhnd]
        stdcall removeVariable, eax
    .finish:

;===================================================
;; This part is for testing to check whether the list is null or not

        stdcall readExpression, buffin
        mov     ebx, eax
        mov     eax, [eax+NODE.value]
        cmp     [eax+NODE.value], NULL
        je      .finish
        mov     eax, ebx
        stdcall printNode, [outhnd], eax
        newline [outhnd]


;===================================================
;; This part is for running the input and evaluating it
;; one time

    .startloop:    ; total 42 lines
        mov     edi, [hBuffin]
        clearBuffer edi, MAXBUFFER
        writeIt [outhnd], prompt
        mov     edx, 0
    @@:
        mov     esi, [hBuffin]
        lea     esi, [esi+edx]
        invoke  ReadConsoleW, [inhnd], esi, MAXBUFFER, numwrtn, 0
        stdcall checkInput, esi
        invoke  lstrlenW, edi
        lea     eax, [eax*2]
        mov     edx, eax
        cmp     [numwrtn], 2
        je      @b
        cmp     byte [aphostrophs], TRUE
        je      @b
        cmp     [parenthesis], 0
        jl      .invalid_expression
        jne     @b
        stdcall normalizeInput, edi
        invoke  lstrlenW, edi
        lea     eax, [eax*2]
        mov     word [edi+eax-4], 0
        ;cinvoke wsprintf, bufout, fmt, buffin
        ;writeLn [outhnd], bufout

        stdcall readExpression, edi
        mov     ebx, eax
        stdcall eval, 0, eax, 0
        mov     ecx, eax
        stdcall printNode, [outhnd], eax
        newline [outhnd]
        stdcall removeNode, ebx
        stdcall removeNode, ecx
        checkFUNSTACKTRACE
        cleanFUNSTACKTRACE
        jmp     .finish
