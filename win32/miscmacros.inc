nwln du  13,10
lnnwln = $ - nwln
macro newline {
        pushad
        invoke  WriteConsoleW, [outhnd], nwln, lnnwln shr 1, numwrtn, 0
        popad
}

macro writeIt thestr {
        invoke  lstrlenW, thestr
        invoke  WriteConsoleW, [outhnd], thestr, eax, numwrtn, 0
}

macro writeLn thestr {
        pushad
        writeIt thestr
        newline
        popad
}

okline  du  'This is executed',13,10
oklen = $ - okline
macro okexecuted {
        pushad
        invoke  WriteConsoleW, [outhnd], okline, oklen shr 1, numwrtn, 0
        popad
}

macro checkItr value {
        pushad
        stdcall convertFromInteger, value, [hDummy]
        writeLn [hDummy]
        popad
}

macro clearBuffer thestr, len {
    pushad
    mov     edi, thestr
    if len eq
        invoke  lstrlenW, thestr
        mov     ecx, eax
    else
        mov     ecx, len
    end if
    xor     eax, eax
    rep     stosw
    popad
}

memfmt du 'The memory value is %p',0
macro memval [theholder] {
    common
        pushad
        display 'common 1 '
    forward
        display 'forward '
        cinvoke wprintf, memfmt, theholder
        newline
    common
        display 'common 2',13,10
        popad
}

macro checkNode [thenodes] {
    common
        pushad
    forward
        stdcall printNode, [outhnd], thenodes
        newline
    common
        popad
}

macro checkVars [thevars] {
    common
        pushad
    forward
        stdcall checkVARLIST, thevars
    common
        popad
}

macro checkList [thelist] {
    common
        pushad
    forward
        stdcall printList, [outhnd], thelist
        newline
    common
        popad
}

expecttypelines du 'Got %s expected %s',0
macro expectedType thenode, typeintended {
        pushad
        mov     ebx, thenode
        checkNode ebx
        mov     eax, [ebx+NODE.type]
        mov     ecx, typeintended
        lea     eax, [eax*4]
        lea     ecx, [ecx*4]
        mov     esi, [datatypeenum+eax]
        mov     edi, [datatypeenum+ecx]
        cinvoke wprintf, expecttypelines, esi, edi
        newline
        popad
}

macro typeErrorJump thenode, thetype {
    .type_error:
        expectedType thenode, thetype
        jmp     type_error
}

expectargnumlines du 'Supplied %d expected %d',0
macro expectedArgnum thenode, supplied, expected {
        pushad
        checkList thenode
        cinvoke wprintf, expectargnumlines, supplied, expected
        newline
        popad
}

macro argnumErrorJump thenode, expected {
        stdcall internalLength, thenode
        dec     eax
        expectedArgnum thenode, eax, expected
        jmp     argnum_error
}

macro inc2 theval {
        inc     theval
        inc     theval
}

macro dec2 theval {
        dec     theval
        dec     theval
}

macro inc2w [theval] {
        inc2    theval
}

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

HEAPFLAG    equ HEAP_ZERO_MEMORY + HEAP_NO_SERIALIZE

macro memalloc allocsize {
        invoke  HeapAlloc, [procHeap], HEAPFLAG, allocsize
        FAILCHK
}

macro memfree memlocation {
        invoke  HeapFree, [procHeap], HEAP_NO_SERIALIZE, memlocation
        FAILCHK
}

macro initFUNSTACKTRACE {
        push    eax ecx edx
        memalloc sizeof.NODE
        mov     [FUNSTACKTRACE], eax
        pop     edx ecx eax
}

macro checkFUNSTACKTRACE {
        stdcall printList, [outhnd], [FUNSTACKTRACE]
}

macro cleanFUNSTACKTRACE {
        stdcall removeList, [FUNSTACKTRACE]
}

macro pushToFUNSTACKTRACE thenode {
        stdcall pushNode, [FUNSTACKTRACE], thenode
        mov     [FUNSTACKTRACE], eax
}

macro initProgram {
        invoke  AllocConsole
        invoke  SetConsoleCP, 65001
        invoke  GetStdHandle, STD_OUTPUT_HANDLE
        mov     [outhnd], eax
        invoke  GetStdHandle, STD_INPUT_HANDLE
        mov     [inhnd], eax
        invoke  GetProcessHeap
        mov     [procHeap], eax
        memalloc MAXBUFFER shl 1
        mov     [hBuffin], eax
        memalloc MAXBUFFER shl 1
        mov     [hDummy], eax
        initFUNSTACKTRACE
        stdcall initVarTable
        initPrimFunTable
        cleanFUNSTACKTRACE
        initFUNSTACKTRACE
}

macro cleanupProgram {
        cleanFUNSTACKTRACE
        memfree [hBuffin]
        memfree [hDummy]
        ;invoke  HeapFree, [procHeap], HEAP_NO_SERIALIZE, [FUNSTACKTRACE]
        ;FAILCHK
}

macro the_error [labels, errtype] {
    labels:
        writeLn errtype#text
        ;mov     eax, errtype
        ;jmp     finish
        checkFUNSTACKTRACE
        newline
        jmp     start.to_finish
}
