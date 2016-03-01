nwln du  13,10
lnnwln = $ - nwln
macro newline thestdout {
        pushad
        invoke  WriteConsoleW, thestdout, nwln, lnnwln shr 1, numwrtn, 0
        popad
}

macro writeIt thehandle, thestr {
        invoke  lstrlenW, thestr
        invoke  WriteConsoleW, thehandle, thestr, eax, numwrtn, 0
}

macro writeLn thehandle, thestr {
        pushad
        writeIt thehandle, thestr
        newline thehandle
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
        writeLn [outhnd], [hDummy]
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
        newline [outhnd]
    common
        display 'common 2',13,10
        popad
}

macro checkNode [thenodes] {
    common
        pushad
    forward
        stdcall printNode, [outhnd], thenodes
        newline [outhnd]
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
        newline [outhnd]
    common
        popad
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

macro initFUNSTACKTRACE {
        push    eax ecx edx
        invoke  HeapAlloc, [procHeap], HEAPFLAG, sizeof.NODE
        FAILCHK
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
        invoke  HeapAlloc, eax, HEAPFLAG, MAXBUFFER shl 1
        FAILCHK
        mov     [hBuffin], eax
        invoke  HeapAlloc, [procHeap], HEAPFLAG, MAXBUFFER shl 1
        FAILCHK
        mov     [hDummy], eax
        initFUNSTACKTRACE
        stdcall initVarTable
        initPrimFunTable
        cleanFUNSTACKTRACE
        initFUNSTACKTRACE
}

macro cleanupProgram {
        cleanFUNSTACKTRACE
        invoke  HeapFree, [procHeap], HEAP_NO_SERIALIZE, [hBuffin]
        FAILCHK
        invoke  HeapFree, [procHeap], HEAP_NO_SERIALIZE, [hDummy]
        FAILCHK
        ;invoke  HeapFree, [procHeap], HEAP_NO_SERIALIZE, [FUNSTACKTRACE]
        ;FAILCHK
}

macro the_error [labels, msgaddr, errtype] {
    labels:
        writeLn [outhnd], msgaddr
        checkFUNSTACKTRACE
        mov     eax, errtype
        jmp     finish
}
