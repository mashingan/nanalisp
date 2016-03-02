struct NODE
    value   dd  ?   ; Handles memory location of value structure
    hMemory dd  ?   ; Handles memory location of node
    length  dd  ?   ; Handles the length of elements for node header
    type    dd  ?   ; Data type of value
    prev    dd  ?   ; Handles memory location of previous link
    next    dd  ?   ; Handles memory location of next link
ends

struct STRINGSTRUCT
    size    dd  ?   ; Size of string character
    hStr    dd  ?   ; Handles memory location of string
    text    db  ?   ; The actual text accordance to size
ends

struct SYMBOLSTRUCT
    hMemory dd  ?   ; Handles memory location of symbol
    hVars   dd  ?   ; Handles memory location for variables closures
    hString du  ?   ; Handles memory location of STRINGSTRUCT
ends

struct VARIABLESTRUCT
    hVar    dd  ?   ; Handles memory location of variable text
    hValue  dd  ?   ; Handles memory location of the value
    hMemory dd  ?   ; Handles memory location of the variable
    next    dd  ?   ; Handles memory location of next variable
    prev    dd  ?   ; Handles memory location of previous variable
ends

struct NUMBERSTRUCT
    hNumber dd  ?   ; Handles memory location of number
    type    dd  ?   ; Data type of number, INTEGER, FLOAT, REAL
    value   dd  ?   ; Handles memory location of actual number value
ends

struct LAMBDASTRUCT
    hExpr   dd  ?   ; Handles memory location of the expression
    hVars   dd  ?   ; Handles memory location of the local variables
    hMemory dd  ?   ; Handles memory location of the lambda struct
ends

struct CHARSTRUCT
    hMemory dd  ?
    hString dd  ?
ends

macro FAILCHK {
    local .next
        push    eax
        cmp     eax, NULL
        jne     .next
        invoke  GetLastError
        cmp     eax, 0
        je      .next
        jmp     exit
    .next:
        pop     eax
}

macro memalloc allocsize {
        invoke  HeapAlloc, [procHeap], HEAPFLAG, allocsize
        FAILCHK
}

macro memfree memlocation {
        invoke  HeapFree, [procHeap], HEAP_NO_SERIALIZE, memlocation
        FAILCHK
}

HEAPFLAG    equ HEAP_ZERO_MEMORY + HEAP_NO_SERIALIZE

calcprnths  dd  0
prtnopen    du  '('
prtnopenln = $ - prtnopen
prtnclose    du  ')'
prtncloseln = $ - prtnclose
spacechar   du  ' '
spacechrln = $ - spacechar
dotchar     du  ' .'
dotlen = $ - dotchar

macro openprntn theout {
    invoke  WriteConsoleW, theout, prtnopen, prtnopenln shr 1, numwrtn, 0
}

macro closeprntn theout {
    invoke  WriteConsoleW, theout, prtnclose, prtncloseln shr 1, numwrtn, 0
}

macro spacing theout {
    push    eax ecx edx
    invoke  WriteConsoleW, theout, spacechar, spacechrln shr 1, numwrtn, 0
    pop     edx ecx eax
}

macro dotted theout {
    push    eax ecx edx
    invoke  WriteConsoleW, theout, dotchar, dotlen shr 1, numwrtn, 0
    pop     edx ecx eax
}

macro toMakelist [thetype, thename, additional] {
        cmp     eax, thetype
        jne     @f
    if additional eq
        stdcall allocate#thename, edi
    else
        stdcall allocate#thename, edi, thetype
    end if
        stdcall allocateNode, eax, thetype
        stdcall addNode, [header], eax
        jmp     .nextloop
      @@:
}

proc makelist uses esi edi ebx ecx, thestring
        local   header:DWORD, len:DWORD, tempbuffer:DWORD, lenElm:DWORD
        memalloc MAXBUFFER shl 1
        mov     [lenElm], 0
        mov     [tempbuffer], eax
        inc     [calcprnths]
        mov     esi, [thestring]
        invoke  lstrlenW, esi
        sub     eax, 4
        inc2    esi
        mov     ecx, eax
        memalloc sizeof.NODE
        mov     [header], eax
    .startloop:
        cmp     ecx, 0
        jle     .endloop
        cmp     word [esi], 0
        je      .endloop
        cmp     word [esi], ')'
        jne     @f
        dec     [calcprnths]
        cmp     [calcprnths], 0
        je      .endloop
        inc2    esi
        dec     ecx
        jmp     .startloop
    @@:
        mov     edi, [tempbuffer]
        stdcall getToken, esi, edi
        mov     [len], eax
        stdcall identify, edi
        inc     [lenElm]
        toMakelist\
            SYMBOL, Symbol, ,\
            CHAR, Char, ,\
            STRING, String, ,\
            INTEGER, Number, INTEGER,\
            FLOAT, Number, FLOAT
        cmp     eax, LIST
        jne     .nextloop
        stdcall makelist, edi
        stdcall allocateNode, eax, LIST
        stdcall addNode, [header], eax
    .nextloop:
        mov     eax, [len]
        lea     ebx, [eax*2]
        add     esi, ebx
        sub     ecx, eax
        jnz     .startloop
    .endloop:
        memfree [tempbuffer]
        mov     eax, [header]
        mov     ebx, [lenElm]
        mov     [eax+NODE.length], ebx
        ret
endp

proc addNode uses esi edi eax ecx, listHandle, newnode:DWORD
        mov     esi, [newnode]
        mov     edi, [listHandle]
        cmp     [edi+NODE.value], 0
        jne     @f
        mov     ecx, sizeof.NODE shr 2
        rep     movsd
        jmp     .exit
    @@:
        cmp     dword [edi+NODE.prev], NULL
        je      @f
        mov     eax, dword [edi+NODE.prev]
        mov     dword [eax+NODE.next], esi
        mov     dword [edi+NODE.prev], esi
        mov     dword [esi+NODE.prev], eax
        mov     dword [esi+NODE.next], edi
        jmp     .exit
    @@:
        mov     dword [edi+NODE.next], esi
        mov     dword [esi+NODE.prev], edi
        mov     dword [edi+NODE.prev], esi
        mov     dword [esi+NODE.next], edi
    .exit:
        ret
endp

proc pushNode uses esi edi ebx ecx, listhandle, newnode:DWORD
        stdcall addNode, [listhandle], [newnode]
        mov     esi, [listhandle]
        cmp     [esi+NODE.prev], NULL
        je      .exit
        mov     esi, [esi+NODE.prev]
    .exit:
        mov     eax, esi
        ret
endp

proc popNode uses esi edi ebx ecx, listhandle
        cmp     [listhandle], NULL
        je      .exit
        mov     esi, [listhandle]
        cmp     [esi+NODE.next], NULL
        jne     @f
        ;stdcall removeNode, esi
        jmp     .exit
    @@:
        mov     ecx, dword [esi+NODE.next]
        mov     ebx, dword [esi+NODE.prev]
        ;stdcall removeNode, esi
        mov     [ecx+NODE.prev], ebx
        mov     [ebx+NODE.next], ecx
        mov     [esi+NODE.next], 0
        mov     [esi+NODE.prev], 0
    .exit:
        mov     eax, esi
        ret
endp

proc allocateString uses esi edi ebx ecx edx, instr
        local   allocsize:DWORD, len:DWORD
        mov     esi, [instr]
        invoke  lstrlenW, esi
        mov     [len], eax
        mov     [allocsize], 8
        lea     eax, [eax*2]
        add     [allocsize], eax
        memalloc dword [allocsize]
        mov     edx, eax
        mov     edi, eax
        mov     eax, [len]
        mov     [edi+STRINGSTRUCT.size], eax
        mov     [edi+STRINGSTRUCT.hStr], edx
        add     edi, 8
        mov     ecx, [len]
        rep     movsw
        mov     eax, edx
        ret
endp

proc allocateSymbol uses esi ebx ecx edx, instr
        mov     esi, [instr]
        memalloc sizeof.SYMBOLSTRUCT
        mov     ebx, eax
        mov     dword [ebx+SYMBOLSTRUCT.hMemory], eax
        stdcall allocateString, esi
        mov     dword [ebx+SYMBOLSTRUCT.hString], eax
        memalloc sizeof.VARIABLESTRUCT
        stdcall allocateVariable, NULL
        mov     dword [ebx+SYMBOLSTRUCT.hVars], eax
        mov     eax, ebx
        ret
endp

proc allocateNumber uses esi ebx ecx edx, strvalue, valtype
        local   len:DWORD
        mov     esi, [strvalue]
        invoke  lstrlenW, esi
        mov     [len], eax
        memalloc sizeof.NUMBERSTRUCT
        mov     ebx, eax
        mov     dword [eax+NUMBERSTRUCT.hNumber], eax
        mov     edx, [valtype]
        mov     dword [eax+NUMBERSTRUCT.type], edx
        cmp     edx, INTEGER
        jne     @f
        memalloc 4
        mov     dword [ebx+NUMBERSTRUCT.value], eax
        stdcall convertToInt, esi, [len]
        mov     ecx, dword [ebx+NUMBERSTRUCT.value]
        mov     [ecx], eax
        jmp     .exit
    @@:
        cmp     dword [valtype], FLOAT
        jne     @f
        memalloc 8
        mov     dword [ebx+NUMBERSTRUCT.value], eax
        stdcall convertToDouble, esi, [len]
        mov     edx, dword [ebx+NUMBERSTRUCT.value]
        fstp    qword [edx]
        jmp     .exit
    @@:
    .exit:
        mov     eax, ebx
        ret
endp

macro allocNumber thetype {
        xor     ecx, ecx
        memalloc sizeof.NUMBERSTRUCT
        mov     ebx, eax
        mov     [ebx+NUMBERSTRUCT.hNumber], eax
    if thetype eq INTEGER
        mov     ecx, 4
    else if thetype eq FLOAT
        mov     ecx, 8
    end if
        mov     [ebx+NUMBERSTRUCT.type], thetype
        memalloc ecx
        mov     [ebx+NUMBERSTRUCT.value], eax
}

proc allocateFloat uses esi edi ebx ecx, thevalue
        allocNumber FLOAT
        mov     esi, [thevalue]
        mov     edi, eax
        movsd
        movsd
        mov     eax, ebx
        ret
endp

proc allocateInteger uses esi edi ebx, thevalue
        allocNumber INTEGER
        mov     esi, [thevalue]
        mov     edi, eax
        mov     [edi], esi
        mov     eax, ebx
        ret
endp

proc allocateChar uses esi ebx ecx edx, charstr
        xor     eax, eax
        mov     esi, [charstr]
        memalloc sizeof.CHARSTRUCT
        mov     ebx, eax
        mov     [ebx+CHARSTRUCT.hMemory], eax
        stdcall allocateString, esi
        mov     [ebx+CHARSTRUCT.hString], eax
        mov     eax, ebx
        ret
endp

proc allocateNode uses esi ebx ecx edx, value, valtype
        xor     eax, eax
        xor     ecx, ecx
        memalloc sizeof.NODE
        mov     ebx, eax
        mov     esi, [value]
        mov     dword [eax+NODE.value], esi
        mov     dword [eax+NODE.hMemory], ebx
        mov     ecx, [valtype]
        mov     dword [eax+NODE.type], ecx
        ret
endp

proc allocateVariable uses esi ebx ecx edx, symbolnode
        mov     esi, [symbolnode]
        memalloc sizeof.VARIABLESTRUCT
        mov     ebx, eax
        xor     eax, eax
        cmp     esi, NULL
        je      @f
        stdcall cpyNode, esi
    @@:
        mov     dword [ebx+VARIABLESTRUCT.hVar], eax
        mov     dword [ebx+VARIABLESTRUCT.hMemory], ebx
        mov     dword [ebx+VARIABLESTRUCT.hValue], 0
        mov     dword [ebx+VARIABLESTRUCT.next], 0
        mov     dword [ebx+VARIABLESTRUCT.prev], 0
        mov     eax, ebx
        ret
endp

proc printList uses esi edi eax ebx ecx, theout, theheader
        openprntn [theout]
        mov     esi, [theheader]
        mov     ebx, esi
    .startloop:
        stdcall printNode, [theout], esi
        cmp     [esi+NODE.next], NULL
        je      .exit
        cmp     [esi+NODE.next], ebx
        je      .exit
        spacing [theout]
        mov     esi, [esi+NODE.next]
        jmp     .startloop
    .exit:
        closeprntn [theout]
        ret
endp

proc printPair uses esi edi eax ebx ecx edx, theout, theheader
        openprntn [theout]
        mov     esi, [theheader]
        mov     edi, [esi+NODE.prev]
    .startloop:
        stdcall printNode, [theout], esi
        cmp     [esi+NODE.next], NULL
        je      .exit
        cmp     [esi+NODE.next], edi
        je      .exit
        spacing [theout]
        mov     esi, [esi+NODE.next]
        jmp     .startloop
    .exit:
        mov     eax, [esi+NODE.next]
        cmp     [eax+NODE.type], LIST
        je      @f
        cmp     [eax+NODE.type], PAIR
        je      @f
        dotted  [theout]
    @@:
        spacing [theout]
        stdcall printNode, [theout], eax
        closeprntn [theout]
        ret
endp

macro printingNode [thetype, thename] {
        cmp     eax, thetype
        jne     @f
        stdcall print#thename, ebx, esi
        jmp     .exit
    @@:
}

proc printNode uses esi edi eax ebx ecx, theout, thenode
        mov     esi, [thenode]
        cmp     esi, 0
        je      .exit
        mov     eax, [esi+NODE.type]
        mov     ebx, [theout]
        mov     esi, [esi+NODE.value]
        printingNode\
            SYMBOL,   Symbol,\
            STRING,   String,\
            CHAR,       Char,\
            LIST,       List,\
            PAIR,       Pair,\
            INTEGER, Integer,\
            FLOAT,     Float,\
            LAMBDA,   Lambda,\
            FUNCTION, Function
    .exit:
        ret
endp

macro toPrintString thestring {
        mov     esi, thestring
        mov     eax, [esi+STRINGSTRUCT.size]
        cmp     eax, 0
        je      @f
        add     esi, STRINGSTRUCT.text
        invoke  WriteConsoleW, edx, esi, eax, numwrtn, 0
    @@:
}

proc printSymbol uses esi eax ecx edx, stdout, thesymbol
        mov     edx, [stdout]
        mov     esi, [thesymbol]
        toPrintString dword [esi+SYMBOLSTRUCT.hString]
        ret
endp

proc printString uses esi eax ecx edx, stdout, thestring
        mov     edx, [stdout]
        toPrintString [thestring]
        ret
endp

proc printChar uses esi edi eax ecx edx, stdout, thechar
        mov     edx, [stdout]
        mov     esi, [thechar]
        mov     esi, [esi+CHARSTRUCT.hString]
        toPrintString [esi+CHARSTRUCT.hString]
        ret
endp

proc printInteger uses esi edi eax ecx edx, theout, thenum
        mov     esi, [thenum]
        mov     eax, [esi+NUMBERSTRUCT.value]
        mov     eax, [eax]
        stdcall convertFromInteger, eax, [hDummy]
        writeIt [outhnd], [hDummy]
        ret
endp

proc printFloat uses eax ecx edx, theout, thenum
        mov     eax, [thenum]
        mov     eax, [eax+NUMBERSTRUCT.value]
        cinvoke wprintf, fmtflt, dword [eax], dword [eax+4]
        ret
endp

macro funcprint lambdanode {
        mov     esi, [lambdanode]
        mov     esi, [lambdanode+LAMBDASTRUCT.hExpr]
        lea     edi, [buffmt]
        cinvoke wprintf, edi, esi
}

proc printLambda uses esi edi eax ecx edx, theout, thelambda
        locals
            buffmt du '<LAMBDA:0x%p>',0
        endl
        funcprint thelambda
        ret
endp

proc printFunction uses esi edi eax ecx edx, theout, thelambda
        locals
            buffmt du '<FUNCTION:0x%p>',0
        endl
        funcprint thelambda
        ret
endp

macro removingNode [thetype, thename] {
        cmp     edx, thetype
        jne     @f
        stdcall remove#thename, eax
        jmp     .finish
    @@:
}

proc removeNode uses esi eax ecx edx, thenode
        cmp     [thenode], NULL
        je      .exit
        mov     esi, [thenode]
        mov     edx, dword [esi+NODE.type]
        mov     eax, dword [esi+NODE.value]
        cmp     eax, NULL
        je      .exit
        removingNode\
            SYMBOL,   Symbol,\
            STRING,   String,\
            CHAR,       Char,\
            LIST,       List,\
            PAIR,       List,\
            INTEGER,  Number,\
            FLOAT,    Number,\
            LAMBDA,   Lambda,\
            FUNCTION, Function
    .finish:
        mov     ecx, [esi+NODE.hMemory]
        memfree ecx
    .exit:
        ret
endp

proc removeString uses esi eax ecx edx, thestring
        cmp     [thestring], NULL
        je      @f
        memfree [thestring]
    @@:
        ret
endp

proc removeChar uses esi eax ecx edx, thechar
        cmp     [thechar], 0
        je      @f
        mov     esi, [thechar]
        mov     eax, [esi+CHARSTRUCT.hString]
        memfree eax
        memfree esi
    @@:
        ret
endp

proc removeSymbol uses esi eax ebx ecx edx, thesymbol
        cmp     [thesymbol], NULL
        je      .exit
        mov     esi, [thesymbol]
        mov     ecx, dword [esi+SYMBOLSTRUCT.hString]
        mov     edx, [esi+SYMBOLSTRUCT.hVars]
        cmp     esi, NULL
        je      @f
        stdcall removeString, ecx
    @@:
        cmp     edx, NULL
        je      @f
        stdcall removeVariable, edx
    @@:
        memfree esi
   .exit:
        ret
endp

proc removeNumber uses esi eax ebx ecx edx, thenumber
        cmp     [thenumber], NULL
        je      @f
        mov     eax, [thenumber]
        mov     ebx, eax
        mov     ecx, [eax+NUMBERSTRUCT.value]
        memfree ecx
        ;mov     eax, [ebx+NUMBERSTRUCT.hNumber]
        memfree ebx
    @@:
        ret
endp

proc removeList uses esi eax, listheader
        cmp     [listheader], NULL
        je      .exit
        mov     esi, [listheader]
        mov     eax, esi
    @@:
        cmp     esi, NULL
        je      .exit
        cmp     [esi+NODE.value], NULL
        ;cmp     [esi+NODE.next], NULL
        je      .exit
        stdcall removeNode, esi
        mov     esi, [esi+NODE.next]
        cmp     esi, eax
        je      .exit
        jmp     @b
    .exit:
        ret
endp

proc removeVariable uses esi eax ebx ecx edx, thevariable
        mov     esi, [thevariable]
        cmp     esi, NULL
        je      .exit
        mov     eax, [esi+VARIABLESTRUCT.hVar]
        mov     ecx, [esi+VARIABLESTRUCT.prev]
        mov     edx, [esi+VARIABLESTRUCT.next]
        cmp     ecx, NULL
        je      @f
        cmp     edx, NULL
        je      @f
        mov     [ecx+VARIABLESTRUCT.next], edx
        mov     [edx+VARIABLESTRUCT.prev], ecx
        cmp     ecx, edx
        jne     @f
        mov     [ecx+VARIABLESTRUCT.prev], 0
        mov     [ecx+VARIABLESTRUCT.next], 0
    @@:
        ;invoke  HeapFree, [procHeap], HEAP_NO_SERIALIZE, eax
        ;FAILCHK
        ;invoke  HeapFree, [procHeap], HEAP_NO_SERIALIZE, esi
        ;FAILCHK
        cmp     eax, NULL
        je      @f
        stdcall removeNode, eax
    @@:
        mov     ebx, [esi+VARIABLESTRUCT.hValue]
        cmp     ebx, NULL
        je      @f
        stdcall removeNode, ebx
    @@:
        memfree esi
    .exit:
        ret
endp

proc removeLambda uses esi eax ecx edx, thelambda
        mov     eax, [thelambda]
        cmp     eax, 0
        je      @f
        mov     ecx, [eax+LAMBDASTRUCT.hVars]
        mov     edx, [eax+LAMBDASTRUCT.hExpr]
        stdcall removeVariable, ecx
        stdcall removeNode, edx
        memfree eax
    @@:
        ret
endp

proc removeFunction uses esi eax ecx edx, thefunction
        mov     eax, [thefunction]
        cmp     eax, 0
        je      @f
        memfree eax
    @@:
        ret
endp

proc freeNodes uses esi eax ebx, listHandle
        mov     esi, [listHandle]
        mov     ebx, esi
    @@:
        test    esi, esi
        jnz     @f
        mov     eax, [esi+NODE.next]
        stdcall removeNode, esi
        mov     esi, eax
        jmp     @b
    @@:
        ret
endp

proc cpyString uses esi edi ebx ecx edx, strstruct
        mov     esi, [strstruct]
        mov     eax, [esi+STRINGSTRUCT.size]
        mov     ebx, eax
        add     eax, 8
        lea     eax, [eax*2]
        memalloc eax
        mov     edi, eax
        movsd               ; copying size from esi
        mov     [edi], eax  ; putting the hStr
        add     esi, 4
        add     edi, 4
        mov     ecx, ebx
        rep     movsw
        ret
endp

proc cpySymbol uses esi edi ebx ecx edx, symstruct
        mov     esi, [symstruct]
        memalloc sizeof.SYMBOLSTRUCT
        mov     edi, eax
        mov     dword [edi+SYMBOLSTRUCT.hMemory], eax
        mov     ebx, dword [esi+SYMBOLSTRUCT.hString]
        stdcall cpyString, ebx
        mov     dword [edi+SYMBOLSTRUCT.hString], eax
        memalloc sizeof.VARIABLESTRUCT
        mov     dword [edi+SYMBOLSTRUCT.hVars], eax
        mov     eax, edi
        ret
endp

proc cpyChar uses esi edi ebx ecx edx, charstruct
        mov     esi, [charstruct]
        memalloc sizeof.CHARSTRUCT
        mov     edi, eax
        mov     dword [edi+CHARSTRUCT.hMemory], eax
        mov     ebx, dword [esi+CHARSTRUCT.hString]
        stdcall cpyString, ebx
        mov     dword [edi+CHARSTRUCT.hString], eax
        mov     eax, edi
        ret
endp

proc cpyNumber uses esi edi ebx ecx edx, numstruct
        mov     esi, [numstruct]
        mov     ecx, sizeof.NUMBERSTRUCT
        memalloc ecx
        mov     ebx, eax
        mov     edi, eax
        mov     dword [edi+NUMBERSTRUCT.hNumber], eax
        mov     edx, [esi+NUMBERSTRUCT.type]
        mov     dword [edi+NUMBERSTRUCT.type], edx
        cmp     edx, INTEGER
        jne     @f
        memalloc 4
        mov     dword [edi+NUMBERSTRUCT.value], eax
        mov     edi, eax
        mov     esi, [esi+NUMBERSTRUCT.value]
        movsd
        jmp     .to_exit
    @@:
        memalloc 8
        mov     [edi+NUMBERSTRUCT.value], eax
        mov     edi, eax
        mov     esi, [esi+NUMBERSTRUCT.value]
        movsd
        movsd
    .to_exit:
        mov     eax, ebx
        jmp     .exit
    .exit:
        ret
endp

proc cpyVariables uses esi edi ebx ecx edx, vartable
        local table:DWORD
        mov     esi, [vartable]
        memalloc sizeof.VARIABLESTRUCT
        mov     [table], eax
        cmp     esi, 0
        je      .exit
    @@:
        stdcall assignVarToTable, [table],\
            [esi+VARIABLESTRUCT.hVar], [esi+VARIABLESTRUCT.hValue]
        cmp     [esi+VARIABLESTRUCT.next], 0
        je      .exit
        mov     esi, [esi+VARIABLESTRUCT.next]
        cmp     esi, [vartable]
        jne     @b
    .exit:
        mov     eax, [table]
        ret
endp

proc cpyLambda uses esi edi ebx ecx edx, lambdastruct
        mov     esi, [lambdastruct]
        memalloc sizeof.LAMBDASTRUCT
        mov     ebx, eax
        mov     [ebx+LAMBDASTRUCT.hMemory], eax
        cmp     esi, 0
        je      .exit
        stdcall cpyNode, [esi+LAMBDASTRUCT.hExpr]
        mov     [ebx+LAMBDASTRUCT.hExpr], eax
        stdcall cpyVariables, [esi+LAMBDASTRUCT.hVars]
        mov     [ebx+LAMBDASTRUCT.hVars], eax
    .exit:
        mov     eax, ebx
        ret
endp


macro cypingNode [thetype, thename] {
        cmp     ebx, thetype
        jne     @f
        stdcall cpy#thename, esi
        stdcall allocateNode, eax, thetype
        jmp     .exit
    @@:
}

proc cpyNode uses esi edi ebx ecx, nodestruct
        mov     esi, [nodestruct]
        mov     ebx, [esi+NODE.type]
        mov     esi, [esi+NODE.value]
        cypingNode\
            SYMBOL, Symbol,\
            CHAR, Char,\
            STRING, String,\
            LIST, List,\
            INTEGER, Number,\
            FLOAT, Number,\
            LAMBDA, Lambda
        mov     eax, [nodestruct]
    .exit:
        ret
endp

proc cpyList uses esi ebx ecx edx, listheader
        cmp     [listheader], NULL
        je      .exit
        local   header:DWORD
        memalloc sizeof.NODE
        mov     [header], eax
        mov     esi, [listheader]
        mov     ebx, esi
    @@:
        cmp     esi, NULL
        je      .exit
        cmp     [esi+NODE.value], NULL
        ;cmp     [esi+NODE.next], NULL
        je      .exit
        stdcall cpyNode, esi
        stdcall addNode, [header], eax
        mov     esi, [esi+NODE.next]
        cmp     esi, ebx
        je      .exit
        jmp     @b
    .exit:
        mov     eax, [header]
        ret
endp
