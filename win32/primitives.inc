setqname    du  'setq',0
setfname    du  'setf',0
set2name    du  'set!',0
quotename   du  'quote',0
consname    du  'cons',0
evalname    du  'eval',0
funcallname du  'funcall',0
quitname    du  'quit',0
carname     du  'car',0
cdrname     du  'cdr',0
lambdaname  du  'lambda',0
prognname   du  'progn',0   ; evalAll
beginname   du  'begin',0   ; evalAll
lengthname  du  'length',0
nullname    du  'null',0
atompname1  du  'atom?',0
nullpname1  du  'null?',0
atompname2  du  'atom',0
nullpname2  du  'nullp',0
equalmemname1   du 'eq?',0
equalmemname2   du 'eq',0
equalvaluename1 du 'equal?',0
equalvaluename2 du 'equal',0
condname    du  'cond',0
displayname du  'display',0
addnumname  du  '+',0
subnumname  du  '-',0
mulnumname  du  '*',0
divnumname  du  '/',0
sinnumname  du  'sin',0
cosnumname  du  'cos',0
tannumname  du  'tan',0
cotannumname du  'cotan',0
absnumname  du  'abs',0
sqrtname    du  'sqrt',0
lessnumname du  '<',0
leqnumname  du  '<=',0
morenumname du  '>',0
meqnumname  du  '>=',0
lessstrname du  'string<?',0
leqstrname  du  'string<=?',0
morestrname du  'string>?',0
meqstrname  du  'string>=?',0
lesschrname du  'char<?',0
leqchrname  du  'char<=?',0
morechrname du  'char>?',0
meqchrname  du  'char>=?',0
qquotename  du  'quasiquote',0
primconst   du  '(setq null (quote ()) true (quote #t) false (quote #f) '
            du  '#f (quote #f) #t (quote #t))',0

macro addFun funname, funaddress {
        stdcall readExpression, funname
        mov     ebx, eax
        stdcall allocateNode, funaddress, FUNCTION
        stdcall assignVarToTable, PRIMFUNTABLE, ebx, eax
        stdcall removeNode, ebx
}

macro initPrimFunTable {
        addFun  setqname, setq
        addFun  setfname, setq
        addFun  set2name, setq
        addFun  quotename, quote
        addFun  consname, cons
        addFun  evalname, eval
        addFun  funcallname, funcall
        addFun  quitname, quit
        addFun  carname, car
        addFun  cdrname, cdr
        addFun  lambdaname, lambda
        addFun  prognname, progn
        addFun  beginname, progn
        addFun  lengthname, length
        addFun  atompname1, atomp
        addFun  nullpname1, nullp
        addFun  atompname2, atomp
        addFun  nullpname2, nullp
        addFun  equalmemname1, equalMem
        addFun  equalmemname2, equalMem
        addFun  equalvaluename1, equalValue
        addFun  equalvaluename2, equalValue
        addFun  condname, cond
        addFun  displayname, thedisplay
        addFun  addnumname, addNum
        addFun  subnumname, subNum
        addFun  mulnumname, mulNum
        addFun  divnumname, divNum
        addFun  sinnumname, sinNum
        addFun  cosnumname, cosNum
        addFun  tannumname, tanNum
        addFun  cotannumname, cotanNum
        addFun  absnumname, absNum
        addFun  sqrtname, sqrt
        addFun  lessnumname, lessNodeNumber
        addFun  leqnumname, lessEqualNodeNumber
        addFun  morenumname, moreNodeNumber
        addFun  meqnumname, moreEqualNodeNumber
        addFun  lessstrname, lessNodeString
        addFun  leqstrname, lessEqualNodeString
        addFun  morestrname, moreNodeString
        addFun  meqstrname, moreEqualNodeString
        addFun  lesschrname, lessNodeChar
        addFun  leqchrname, lessEqualNodeChar
        addFun  morechrname, moreNodeChar
        addFun  meqchrname, moreEqualNodeChar
        addFun  qquotename, quasiquote

        stdcall readExpression, primconst
        mov     ebx, eax
        mov     eax, [eax+NODE.value]
        stdcall setq, eax, [eax+NODE.next], 0
        stdcall removeNode, ebx
}

macro toReadExpression [thetype, thename, additional, alternative] {
        cmp     eax, thetype
        jne     @f
    if (additional eq INTEGER) | (additional eq FLOAT)
        stdcall allocate#thename, edi, thetype
    else if alternative eq makelist
        stdcall alternative, edi
    else
        stdcall allocate#thename, edi
    end if
        stdcall allocateNode, eax, thetype
        jmp     .exit
    @@:
}

proc readExpression uses esi edi ebx ecx edx, instr
        mov     esi, [instr]
        mov     edi, [hDummy]
        stdcall getToken, esi, edi
        stdcall identify, edi
        mov     ebx, eax
        toReadExpression\
            SYMBOL, Symbol  , , ,\
            CHAR, Char      , , ,\
            STRING, String  , , ,\
            INTEGER, Number , INTEGER, ,\
            FLOAT, Number   , FLOAT, ,\
            LIST,           , , makelist
    .exit:
        ret
endp

;; symbolLength procedure returns the length of textual symbol
proc symbolLength uses esi ebx, symbolnode
        mov     esi, [symbolnode]
        mov     eax, [esi+NODE.value]
        mov     ebx, dword [eax+SYMBOLSTRUCT.hString]
        mov     eax, [ebx+STRINGSTRUCT.size]
        ret
endp

;; compVar procedure returns the comparison between variable and symbol
;; which result determined as:
;; == 0 -- the same textual between variable and symbol
;;  < 0 -- symbol less than variable
;;  > 0 -- symbol greater than variable
proc compVar uses esi edi ebx ecx, varstruct, symbolnode
        mov     esi, [varstruct]
        mov     edi, [symbolnode]
        mov     esi, [esi+VARIABLESTRUCT.hVar]
        mov     esi, [esi+NODE.value]
        mov     esi, dword [esi+SYMBOLSTRUCT.hString]
        mov     eax, [esi+STRINGSTRUCT.size]
        mov     edi, [edi+NODE.value]
        mov     edi, dword [edi+SYMBOLSTRUCT.hString]
        mov     ebx, [edi+STRINGSTRUCT.size]
        cmp     eax, ebx
        jl      .less_than
        jg      .greater_than
        add     esi, STRINGSTRUCT.text
        add     edi, STRINGSTRUCT.text
        mov     ecx, ebx
        ;cld
        ;repe    cmpsb
        xor     eax, eax
    @@:
        mov     ax, word [esi]
        cmp     ax, word [edi]
        jne     @f
        inc2w   esi, edi
        dec     ecx
        cmp     ecx, 0
        jne     @b
    @@:
        cmp     ecx, 0
        je      @f
        sub     ax, word [edi]
        cmp     ax, 0
        jl      .less_than
        jg      .greater_than
    @@:
        xor     eax, eax
        jmp     .exit
    .less_than:
        mov     eax, -1
        jmp     .exit
    .greater_than:
        mov     eax, 1
    .exit:
        ret
endp

;; lookup procedure will return the variable node within VARTABLE
;; in eax and 0 if no binding found
proc lookup uses esi edi ebx ecx, vars, symbolnode
        local   tableHeader:DWORD
        xor     eax, eax
        cmp     [vars], NULL
        je      .no_binding
        mov     esi, [vars]
        cmp     [esi+VARIABLESTRUCT.hVar], NULL
        jne     @f
        jmp     .no_binding
    @@:
        mov     edi, [symbolnode]
        mov     ebx, esi
        mov     [tableHeader], ebx
        cmp     [esi+VARIABLESTRUCT.next], NULL
        jne     .varslist
        cmp     [esi+VARIABLESTRUCT.prev], NULL
        jne     .varslist
        stdcall compVar, esi, edi
        cmp     eax, 0
        jne     .no_binding
        jmp     .find_it
    @@:
    .varslist:
    .startloop:
        stdcall compVar, esi, edi
        cmp     eax, 0
        je      .find_it
        mov     esi, [esi+VARIABLESTRUCT.next]
        cmp     esi, [tableHeader]
        je      .no_binding
        jmp     .startloop
    .exit:
        ret
    .no_binding:
        xor     eax, eax
        jmp     .exit
    .find_it:
        mov     eax, esi
        jmp     .exit
endp

proc newVar uses esi edi ebx ecx, symbolnode, theval
        mov     esi, [symbolnode]
        mov     edi, [theval]
        stdcall allocateVariable, esi
        mov     ebx, eax
        mov     eax, edi
        cmp     edi, NULL
        je      @f
        stdcall cpyNode, edi
    @@:
        mov     [ebx+VARIABLESTRUCT.hValue], eax
        mov     eax, ebx
        ret
endp

proc assignVarToTable uses esi edi ebx ecx edx, vars, symbolnode, theval
        mov     edi, [vars]
        mov     esi, [symbolnode]
        stdcall lookup, edi, esi
        cmp     eax, 0
        jne     @f
        stdcall newVar, esi, [theval]
        cmp     [edi+VARIABLESTRUCT.hVar], NULL
        je      .first_elm
        cmp     [edi+VARIABLESTRUCT.prev], NULL
        jne     .last_elm
        cmp     [edi+VARIABLESTRUCT.next], NULL
        jne     .last_elm
        ;; below is second element
        mov     esi, eax
        mov     [edi+VARIABLESTRUCT.prev], esi
        mov     [edi+VARIABLESTRUCT.next], esi
        mov     [eax+VARIABLESTRUCT.next], edi
        mov     [eax+VARIABLESTRUCT.prev], edi
        jmp     .exit
      .first_elm:
        mov     ecx, sizeof.VARIABLESTRUCT shr 2
        mov     esi, eax
        rep     movsd
        mov     eax, edi
        jmp     .exit
      .last_elm:
        mov     esi, eax
        mov     eax, dword [edi+VARIABLESTRUCT.prev]
        mov     dword [eax+VARIABLESTRUCT.next], esi
        mov     dword [edi+VARIABLESTRUCT.prev], esi
        mov     dword [esi+VARIABLESTRUCT.prev], eax
        mov     dword [esi+VARIABLESTRUCT.next], edi
        jmp     .exit
    @@:
        mov     edi, eax
        stdcall removeNode, [edi+VARIABLESTRUCT.hValue]
        stdcall cpyNode, [theval]
        mov     [edi+VARIABLESTRUCT.hValue], eax
    .exit:
        ret
endp

proc quote uses esi, hQuote, results, envs
        mov     esi, [results]
        mov     eax, [hQuote]
        cmp     esi, 0
        je      argnum_error
        cmp     [esi+NODE.next], 0
        je      argnum_error
        cmp     [esi+NODE.next], eax
        jne     argnum_error
        stdcall cpyNode, esi
        ret
endp

proc setq uses esi edi ebx ecx, hSetq, listarg, envs
        mov     ebx, [hSetq]
        mov     esi, [listarg]
        mov     eax, esi
        cmp     ebx, [esi+NODE.next]
        jne     @f
    .argnum_error:
        jmp     argnum_error
    @@:
        cmp     [esi+NODE.type], SYMBOL
        je      @f
    .type_error:
        jmp     type_error
    @@:
        stdcall symbolLength, esi
        dec     eax
        shl     eax, 2
        lea     edi, [VARTABLE+eax]
        mov     edi, [edi]
        mov     eax, [esi+NODE.next]
        cmp     eax, ebx
        je      .argnum_error
        ;; assignVarToTable(varstruct, esi=symbolnode, node)
        stdcall eval, 0, eax, [envs]
        stdcall assignVarToTable, edi, esi, eax
        mov     esi, [esi+NODE.next]
        cmp     [esi+NODE.next], ebx
        je      .exit
        mov     esi, [esi+NODE.next]
        cmp     [esi+NODE.type], SYMBOL
        jne     .type_error
        jmp     @b
    .exit:
        ret
endp

proc cons uses esi edi ebx ecx edx, hCons, atoms, envs
        local   header:DWORD
        mov     ebx, [hCons]
        mov     esi, [atoms]
        mov     edi, [esi+NODE.next]
        cmp     [edi+NODE.next], ebx
        jne     .argnum_error
        stdcall cpyNode, esi
        stdcall eval, 0, eax, [envs]
        mov     ebx, eax
        stdcall cpyNode, edi
        stdcall eval, 0, eax, [envs]
        mov     edx, eax
        cmp     [eax+NODE.type], PAIR
        jne     @f
        jmp     .make_pair
    @@:
        cmp     [eax+NODE.type], LIST
        jne     @f
        stdcall pushNode, [eax+NODE.value], ebx
        mov     [edx+NODE.value], eax
        mov     eax, edx
        jmp     .exit
    @@:
    .make_pair:
        push    ebx edx
        invoke  HeapAlloc, [procHeap], HEAPFLAG, sizeof.NODE
        mov     [header], eax
        pop     edx ebx
        stdcall addNode, eax, ebx
        stdcall addNode, eax, edx
        stdcall allocateNode, [header], PAIR
    .exit:
        ret
    .argnum_error:
        jmp     argnum_error
endp

proc isAtom, atom
        mov     eax, [atom]
        cmp     eax, 0
        je      .is_false
        mov     eax, [eax+NODE.type]
        cmp     eax, PAIR
        je      .is_false
        cmp     eax, LIST
        je      .is_false
        cmp     eax, LAMBDA
        je      .is_false
        cmp     eax, FUNCTION
        je      .is_false
    .is_true:
        mov     eax, TRUE
        jmp     .exit
    .is_false:
        mov     eax, FALSE
    .exit:
        ret
endp

proc isList, elems
        stdcall isAtom, [elems]
        cmp     eax, TRUE
        je      .is_false
        stdcall isNull, [elems]
        cmp     eax, TRUE
        je      .is_false
        mov     eax, [elems]
        mov     eax, [eax+NODE.type]
        cmp     eax, LIST
        je      .is_true
        cmp     eax, PAIR
        je      .is_true
        cmp     eax, LAMBDA
        je      .is_false
        cmp     eax, FUNCTION
        je      .is_false
    .is_true:
        mov     eax, TRUE
        jmp     .exit
    .is_false:
        mov     eax, FALSE
    .exit:
        ret
endp

macro makeBool {
        lea     esi, [toread]
        stdcall readExpression, esi
}

proc true uses esi
        locals
            toread du '#t',0
        endl
        makeBool
        ret
endp

proc false uses esi
        locals
            toread du '#f',0
        endl
        makeBool
        ret
endp

proc null uses esi
        locals
            toread du '()',0
        endl
        makeBool
        ret
endp

proc isTrue uses ebx, thenode
        mov     ebx, [thenode]
        stdcall isAtom, [thenode]
        cmp     eax, TRUE
        jne     .is_false
        stdcall false
        mov     ebx, eax
        stdcall equalNode, [thenode], eax
        cmp     eax, TRUE
        je      @f
        mov     eax, TRUE
        jmp     .exit
    @@:
    .is_false:
        mov     eax, FALSE
    .exit:
        stdcall removeNode, ebx
        ret
endp

proc atomp uses esi, hAtom, atomelem, envs
        cmp     [atomelem], 0
        je      argnum_error
        mov     esi, [atomelem]
        mov     eax, [hAtom]
        cmp     [esi+NODE.next], eax
        jne     argnum_error
        stdcall eval, 0, esi, [envs]
        stdcall isAtom, eax
        cmp     eax, TRUE
        je      .is_true
        stdcall false
        jmp     .exit
    .is_true:
        stdcall true
    .exit:
        ret
endp

proc isNull uses esi, lists
        mov     esi, [lists]
        stdcall isAtom, esi
        cmp     eax, TRUE
        je      @f
        cmp     [esi+NODE.type], LAMBDA
        je      @f
        mov     esi, [esi+NODE.value]
        cmp     [esi+NODE.value], NULL
        jne     @f
        mov     eax, TRUE
        jmp     .exit
    @@:
        mov     eax, FALSE
    .exit:
        ret
endp

proc nullp uses esi, hNullp, lists, envs
        mov     esi, [lists]
        cmp     esi, 0
        je      argnum_error
        mov     eax, [hNullp]
        cmp     [esi+NODE.next], eax
        jne     argnum_error
        stdcall eval, 0, esi, [envs]
        mov     ebx, eax
        stdcall isAtom, eax
        cmp     eax, FALSE
        je      .is_true
        stdcall false
        jmp     .exit
    .is_true:
        stdcall isNull, ebx
        cmp     eax, TRUE
        je      @f
        stdcall false
        jmp     .exit
    @@:
        stdcall true
    .exit:
        ret
endp

macro toCheckInit struct1, struct2 {
        xor     ecx, ecx
        mov     eax, FALSE
        mov     esi, struct1
        mov     edi, struct2
        cmp     esi, edi
        je      .is_true
}

macro toCheckEnd {
    .is_true:
        mov     eax, TRUE
    .exit:
}

proc equalString uses esi edi ebx ecx, strstruct1, strstruct2
        toCheckInit [strstruct1], [strstruct2]
        mov     ecx, [esi+STRINGSTRUCT.size]
        cmp     ecx, [edi+STRINGSTRUCT.size]
        jne     .exit
        add     esi, STRINGSTRUCT.text
        add     edi, STRINGSTRUCT.text
    @@:
        mov     bx, word [esi]
        cmp     bx, word [edi]
        jne     .exit
        inc2w   esi, edi
        dec     ecx
        jnz     @b
        toCheckEnd
        ret
endp

macro toCheckEqualString struct1, struct2, namestruct {
        toCheckInit struct1, struct2
        mov     esi, dword [esi+namestruct#.hString]
        mov     edi, dword [edi+namestruct#.hString]
        stdcall equalString, esi, edi
        jmp     .exit
        toCheckEnd
}

proc equalSymbol uses esi edi ebx ecx, symstruct1, symstruct2
        toCheckEqualString [symstruct1], [symstruct2], SYMBOLSTRUCT
        ret
endp

proc equalChar uses esi edi ebx ecx, charstruct1, charstruct2
        toCheckEqualString [charstruct1], [charstruct2], CHARSTRUCT
        ret
endp

proc equalNumber uses esi edi ebx ecx edx, numstruct1, numstruct2
        toCheckInit [numstruct1], [numstruct2]
        mov     edx, [esi+NUMBERSTRUCT.type]
        mov     ebx, [esi+NUMBERSTRUCT.value]
        mov     ecx, [edi+NUMBERSTRUCT.value]
        cmp     edx, INTEGER
        jne     @f
        mov     ebx, [ebx]
        mov     ecx, [ecx]
        cmp     ebx, ecx
        je      .is_true
        jmp     .exit
    @@:
        cmp     edx, LONG
        jne     @f
        fild    qword [ebx]
        fild    qword [ecx]
        fcomip  st, st1
        je      .is_true
        jmp     .exit
    @@:
        finit
        fld     qword [ebx]
        fld     qword [ecx]
        fcomip  st, st1
        jne     .exit
        toCheckEnd
        ret
endp

proc equalList uses esi edi ebx ecx, list1, list2
        toCheckInit [list1], [list2]
        stdcall internalLength, esi
        mov     ebx, eax
        stdcall internalLength, edi
        cmp     eax, ebx
        je      @f
        mov     eax, FALSE
        jmp     .exit
    @@:
        mov     eax, FALSE
        mov     ebx, esi
        mov     ecx, edi
        cmp     ebx, 0
        je      .exit
        cmp     ecx, 0
        je      .exit
        stdcall equalNode, esi, edi
        cmp     eax, TRUE
        jne     .exit
        cmp     [esi+NODE.next], 0
        jne     @f
        cmp     [edi+NODE.next], 0
        je      .exit
    @@:
    .startloop:
        mov     esi, [esi+NODE.next]
        mov     edi, [edi+NODE.next]
        stdcall equalNode, esi, edi
        cmp     eax, TRUE
        jne     .exit
        cmp     esi, ebx
        jne     @f
        cmp     edi, ecx
        je      .is_true
    @@:
        jmp     .startloop
        toCheckEnd
        ret
endp

macro equalNodeChecking [thetype, thename] {
        cmp     ecx, thetype
        jne     @f
        stdcall equal#thename, esi, edi
        jmp     .exit
    @@:
}

proc equalNode uses esi edi ebx ecx, node1, node2
        toCheckInit [node1], [node2]
        mov     ecx, [esi+NODE.type]
        cmp     ecx, [edi+NODE.type]
        jne     .is_false
        mov     esi, [esi+NODE.value]
        mov     edi, [edi+NODE.value]
        equalNodeChecking\
            SYMBOL,  Symbol,\
            CHAR,      Char,\
            STRING,  String,\
            INTEGER, Number,\
            LONG,    Number,\
            FLOAT,   Number,\
            LIST,      List
    .is_false:
        mov     eax, FALSE
        jmp     .exit
        toCheckEnd
        ret
endp

macro equalCheckInit theheader, thelists {
        mov     esi, thelists
        mov     eax, theheader
        cmp     esi, 0
        je      argnum_error
        cmp     [esi+NODE.next], 0
        je      argnum_error
        cmp     [esi+NODE.next], eax
        je      argnum_error
        mov     edi, [esi+NODE.next]
        stdcall eval, 0, esi, [envs]
        mov     ebx, eax
        stdcall eval, 0, edi, [envs]
}

macro equalCheckEnd {
        stdcall true
        jmp     .exit
    .is_false:
        stdcall false
    .exit:
}

proc equalMem uses esi edi ebx, hEq, elements, envs
        equalCheckInit [hEq], [elements]
        cmp     ebx, eax
        jne     .is_false
        equalCheckEnd
        ret
endp

proc equalValue uses esi edi ebx ecx, hEqual, elements, envs
        equalCheckInit [hEqual], [elements]
        mov     ecx, [eax+NODE.type]
        cmp     ecx, [ebx+NODE.type]
        jne     .is_false
        ;mov     ecx, [eax+NODE.value]
        ;mov     edx, [ebx+NODE.value]
        stdcall equalNode, ebx, eax
        jne     .is_false
        equalCheckEnd
        ret
endp

proc getVarFromTable uses esi edi ebx, initialtable, symbolnode
        xor     eax, eax
        mov     esi, [initialtable]
        mov     edi, [symbolnode]
        stdcall symbolLength, edi
        cmp     eax, VARTABLESIZE
        jle     @f
        mov     eax, VARTABLESIZE
    @@:
        dec     eax
        lea     eax, [eax*4]
        lea     esi, [esi+eax]
        mov     esi, [esi]
        stdcall lookup, esi, edi
        ret
endp

proc eval uses esi edi ebx ecx edx, hEval, atoms, envs
        local   locvars:DWORD, evalnode:DWORD, header:DWORD
        mov     ebx, [atoms]
        mov     eax, [hEval]
        cmp     ebx, NULL
        je      argnum_error
        cmp     eax, NULL       ; to check whether eval is invoked
        je      @f              ; internally
        cmp     [ebx+NODE.next], eax    ; to check whether the eval
        jne     argnum_error           ; invoked from expression has
    @@:                                 ; correct args
        stdcall isAtom, ebx
        cmp     eax, TRUE
        je      .return_atom
        stdcall isNull, ebx
        cmp     eax, TRUE
        je      .return_atom
        cmp     [ebx+NODE.type], LAMBDA
        je      .return_itself
        mov     esi, [ebx+NODE.value]
        cmp     [esi+NODE.type], LIST
        jne     @f
        mov     eax, [esi+NODE.value]
        cmp     [eax+NODE.type], LAMBDA
        je      .proc_lambda
        push    esi
        stdcall readExpression, funcallname
        pushToFUNSTACKTRACE eax
        pop     esi
        stdcall funcall, esi, esi, [envs]
        jmp     .exit
      .proc_lambda:
        ;stdcall evalLambda, esi
        jmp     .return_itself
        jmp     .exit
    @@:
        stdcall lookup, [envs], [esi+NODE.value]
        ;stdcall lookup, [envs], esi
        cmp     eax, NULL
        je      @f
        jmp     .proc_func
    @@:
        stdcall getVarFromTable, VARTABLE, esi
        cmp     eax, NULL
        je      @f
        push    eax
        stdcall cpyNode, [eax+VARIABLESTRUCT.hVar]
        pushToFUNSTACKTRACE eax
        pop     eax
        mov     eax, [eax+VARIABLESTRUCT.hValue]
        mov     esi, [ebx+NODE.value]
        stdcall cpyNode, eax    ; copying lambda
        stdcall evalLambda, eax, esi, [envs]
        jmp     .exit
    @@:
        stdcall lookup, PRIMFUNTABLE, [esi+NODE.value]
        cmp     eax, NULL
        je      no_binding_error
        ;jmp     .proc_func
      .proc_func:
        pushad
        mov     ebx, eax
        stdcall readExpression, evalname
        mov     ecx, eax
        stdcall compVar, ebx, eax
        cmp     eax, 0
        jne     @f
        push    ecx
        stdcall cpyNode, ecx
        pushToFUNSTACKTRACE eax
        pop     ecx
        stdcall removeNode, ecx
        stdcall eval, esi, [esi+NODE.next], [envs]
        stdcall eval, 0, eax, [envs]
        mov     [evalnode], eax
        popad
        mov     eax, [evalnode]
        jmp     .exit
      @@:
        popad
        push    eax
        stdcall cpyNode, [eax+VARIABLESTRUCT.hVar]
        pushToFUNSTACKTRACE eax
        pop     eax
        mov     eax, [eax+VARIABLESTRUCT.hValue]
        cmp     [eax+NODE.type], LAMBDA
        jne     @f
        stdcall cpyNode, eax    ; copying lambda
        stdcall evalLambda, eax, esi, [envs]
        jmp     .exit
        @@:
        mov     eax, [eax+NODE.value]
        stdcall eax, esi, [esi+NODE.next], [envs]
        jmp     .exit
      .return_atom:
        cmp     [ebx+NODE.type], SYMBOL
        jne     .return_itself
        stdcall lookup, [envs], ebx
        cmp     eax, 0
        je      @f
        jmp     .proceed_symbol
    @@:
        stdcall getVarFromTable, VARTABLE, ebx
        cmp     eax, 0
        je      @f
        jmp     .proceed_symbol
    @@:
        stdcall lookup, PRIMFUNTABLE, ebx
        cmp     eax, 0
        je      no_binding_error
      .proceed_symbol:
        mov     eax, [eax+VARIABLESTRUCT.hValue]
        jmp     .exit
      .return_itself:
        mov     eax, [atoms]
    .exit:
        ret
endp

proc funcall uses esi ebx ecx, hFuncall, hArgs, envs
        mov     ebx, [hFuncall]
        mov     esi, [hArgs]
        mov     eax, esi
        mov     ecx, esi
        cmp     [esi+NODE.type], SYMBOL
        je      @f
        ;cmp     [esi+NODE.type], LAMBDA
        ;je      .proc_lambda
    @@:
        push    esi
        stdcall eval, 0, esi, [envs] ; 2nd arg after funcall
        mov     ecx, eax
        pop     esi
        stdcall isNull, eax
        cmp     eax, TRUE
        je      type_error
        cmp     [ecx+NODE.type], LAMBDA
        je      .proc_lambda
    @@:
        stdcall lookup, PRIMFUNTABLE, ecx
        cmp     eax, 0
        je      @f
        jmp     .proc_func
    @@:
        stdcall getVarFromTable, VARTABLE, eax
        cmp     eax, 0
        je      .no_binding_error
      .proc_func:
        mov     eax, [eax+VARIABLESTRUCT.hValue]
        mov     eax, [eax+NODE.value]
        mov     esi, [esi+NODE.next]
        stdcall eax, ebx, esi
        jmp     .exit
    .exit:
        ret
    .proc_lambda:
        ;stdcall evalLambda, ecx, [esi+NODE.next], [envs]
        stdcall evalLambda, ecx, esi, [envs]
        jmp     .exit
    .no_binding_error:
        ;checkNode eax
        jmp     no_binding_error
endp

proc quit uses esi ebx, hQuit, hArgs, envs
        xor     eax, eax
        jmp     finish
        ret
endp

proc car uses esi, noarg, thelist, envs
        mov     eax, [thelist]
        stdcall eval, 0, eax, [envs]
        mov     esi, eax
        stdcall isAtom, eax
        cmp     eax, TRUE
        je      type_error
        stdcall isNull, esi
        cmp     eax, TRUE
        je      type_error
        mov     eax, [esi+NODE.value]
        stdcall cpyNode, eax
        ret
endp

proc cdr uses esi edi ebx ecx edx, noarg, thelist, envs
        mov     eax, [thelist]
        stdcall isNull, eax
        cmp     eax, TRUE
        je      .return_null
        mov     eax, [thelist]
        stdcall eval, 0, eax, [envs]
        mov     esi, eax
        stdcall isAtom, eax
        cmp     eax, TRUE
        je      .type_error
        stdcall isNull, esi
        cmp     eax, TRUE
        je      .return_null
        mov     edx, [esi+NODE.value]
        cmp     [edx+NODE.next], 0
        je      .return_null
        stdcall cpyNode, esi
        mov     ebx, eax
        mov     ecx, [eax+NODE.value]
        mov     esi, [ecx+NODE.prev]
        mov     edi, [ecx+NODE.next]
        stdcall removeNode, ecx
        cmp     esi, 0
        jne     @f
        jmp     .return_null
    @@:
        cmp     edi, 0
        jne     @f
        jmp     .return_null
    @@:
        mov     [edi+NODE.prev], esi
        mov     [esi+NODE.next], edi
        mov     eax, edi
        mov     [ebx+NODE.value], eax
        mov     eax, ebx
    .exit:
        ret
    .type_error:
        jmp     type_error
    .return_null:
        stdcall null
        ;ret
        jmp     .exit
endp

proc lambda uses esi edi ebx ecx edx, hLambda, exprs, envs
        local   locvars:DWORD, lmdstruct:DWORD, envs_not_null:BYTE
        mov     [envs_not_null], FALSE
        cmp     [envs], 0
        je      @f
        mov     eax, [envs]
        mov     [locvars], eax
        mov     [envs_not_null], TRUE
        jmp     .init_var_ok
      @@:
        stdcall allocateVariable, NULL
        mov     [locvars], eax
      .init_var_ok:
        invoke  HeapAlloc, [procHeap], HEAPFLAG, sizeof.NODE
        FAILCHK
        mov     [lmdstruct], eax
        mov     [eax+NODE.hMemory], eax
        mov     esi, [exprs]            ; local vars, assumed a list
        mov     ebx, [esi+NODE.next]    ; handles expressions
        mov     edi, [esi+NODE.value]
        mov     ecx, edi
        stdcall isNull, esi
        cmp     eax, TRUE
        je      .no_locals
    @@: ; defining local environment
        stdcall assignVarToTable, [locvars], edi, NULL
        cmp     [edi+NODE.next], 0
        je      @f
        mov     edi, [edi+NODE.next]
        cmp     edi, ecx
        jne     @b
    @@:
    .no_locals:
        mov     edx, ebx
    @@: ; copying the instructions node
        stdcall cpyNode, ebx
        stdcall addNode, [lmdstruct], eax
        mov     ebx, [ebx+NODE.next]
        cmp     ebx, [hLambda]
        jne     @b
        invoke  HeapAlloc, [procHeap], HEAPFLAG, sizeof.LAMBDASTRUCT
        FAILCHK
        mov     [eax+LAMBDASTRUCT.hMemory], eax
        cmp     byte [envs_not_null], TRUE
        jne     @f
        jmp     .search_for_first_null_var
    @@:
    .finishing:
        mov     ecx, [locvars]
        mov     ebx, [lmdstruct]
        mov     [eax+LAMBDASTRUCT.hExpr], ebx
        mov     [eax+LAMBDASTRUCT.hVars], ecx
        stdcall allocateNode, eax, LAMBDA
    .exit:
        ret
    .search_for_first_null_var:
        mov     esi, [locvars]
      @@:
        cmp     [esi+VARIABLESTRUCT.hValue], 0
        je      @f
        cmp     [esi+VARIABLESTRUCT.next], 0
        je      @f
        mov     esi, [esi+VARIABLESTRUCT.next]
        jmp     @b
      @@:
        mov     [locvars], esi
        jmp     .finishing
endp

proc isCurry uses esi ecx edx, vars
        xor     eax, eax
        mov     esi, [vars]
        cmp     esi, 0
        je      .exit
    @@:
        cmp     [esi+VARIABLESTRUCT.hVar], 0
        je      .exit
        cmp     [esi+VARIABLESTRUCT.hValue], 0
        jne     .value_available
        inc     eax
      .value_available:
        cmp     [esi+VARIABLESTRUCT.next], 0
        je      @f
        mov     esi, [esi+VARIABLESTRUCT.next]
        cmp     esi, [vars]
        jne     @b
    @@:
    .exit:
        ret
endp

proc evalLambda uses esi edi ebx ecx edx, hEval, atoms, envs
        local args:DWORD, locvars:DWORD, header:DWORD, fromLambda:BYTE
        local unboundNum:DWORD
        mov     [fromLambda], FALSE
        mov     [args], 0
        mov     esi, [hEval]
        mov     ebx, [atoms]
        mov     edx, ebx
        ;stdcall eval, 0, [edx+NODE.prev], [envs]
        ;mov     [header], eax
        mov     esi, [esi+NODE.value]
        mov     ebx, [ebx+NODE.next]
        stdcall varlistLength, [esi+LAMBDASTRUCT.hVars]
        mov     [locvars], eax
        cmp     eax, 0
        jne     @f
        cmp     ebx, 0
        je      .eval_directly
        ;jmp     argnum_error
    @@:
        mov     ecx, [esi+LAMBDASTRUCT.hVars]
        mov     edi, ecx
        stdcall isCurry, ecx
        mov     [unboundNum], eax
        cmp     eax, 0
        je      .endbindings
    .startbindings:
        cmp     ebx, 0
        je      .endbindings
        ;je      @f
        ;cmp     [ecx+VARIABLESTRUCT.hValue], 0
        ;jne     @f
        stdcall cpyNode, ebx
        stdcall eval, 0, eax, [envs]
        mov     [ecx+VARIABLESTRUCT.hValue], eax
        inc     [args]
      @@:
        cmp     [ebx+NODE.next], 0
        je      .endbindings
        cmp     [ecx+VARIABLESTRUCT.next], 0
        je      .endbindings
        mov     ecx, [ecx+VARIABLESTRUCT.next]
        mov     ebx, [ebx+NODE.next]
        cmp     ecx, edi
        je      .endbindings
        ;cmp     ebx, [header]
        ;je      .endbindings
        ;cmp     ebx, [edx+NODE.prev]
        ;je      .endbindings
        cmp     ebx, [atoms]
        je      .endbindings
        jmp     .startbindings
    .endbindings:
        mov     eax, [args]
        cmp     [unboundNum], eax
        jl      argnum_error
        stdcall isCurry, ecx
        cmp     eax, 0
        jne     .make_lambda
        ;cmp     [envs], 0      ; This is to combine the environment
        ;je      @f             ; just by checking the parent envs
        ;mov     eax, [hEval]
        ;mov     ecx, [ecx+NODE.value]
        ;jmp     .combine_envs
    @@:
    .proceed_the_lambda:
        mov     eax, [hEval]
        mov     eax, [eax+NODE.value]
        mov     ebx, [eax+LAMBDASTRUCT.hExpr]
        mov     ecx, [eax+LAMBDASTRUCT.hVars]
        stdcall progn, 0, ebx, ecx
    .exit:
        ret
    .eval_directly:
        mov     eax, [hEval]
        mov     eax, [eax+NODE.value]
        mov     ebx, [eax+LAMBDASTRUCT.hExpr]
        mov     ecx, [eax+LAMBDASTRUCT.hVars]
        stdcall progn, 0, ebx, ecx
        ret
    .make_lambda:
        mov     eax, [hEval]
        mov     ecx, [eax+NODE.value]
        mov     [fromLambda], TRUE
        cmp     [envs], 0
        jne     .combine_envs
        jmp     .look_for_unbound
    .combine_envs:
        mov     ebx, [envs]
    @@:
        stdcall assignVarToTable, [ecx+LAMBDASTRUCT.hVars],\
            [ebx+VARIABLESTRUCT.hVar], [ebx+VARIABLESTRUCT.hValue]
        cmp     [ebx+VARIABLESTRUCT.next], 0
        je      @f
        mov     ebx, [ebx+VARIABLESTRUCT.next]
        cmp     ebx, [envs]
        jne     @b
    @@:
    .look_for_unbound:
        mov     ebx, [ecx+LAMBDASTRUCT.hVars]
    @@: ;; changing into the first unbound variable
        cmp     [ebx+VARIABLESTRUCT.hValue], 0
        je      @f
        cmp     [ebx+VARIABLESTRUCT.next], 0
        je      @f
        cmp     [ebx+VARIABLESTRUCT.prev], 0
        je      @f
        mov     ebx, [ebx+VARIABLESTRUCT.next]
        cmp     ebx, [ecx+LAMBDASTRUCT.hVars]
        jne     @b
    @@:
        mov     [ecx+LAMBDASTRUCT.hVars], ebx
        cmp     [fromLambda], TRUE
        je      @f
        jmp     .proceed_the_lambda
    @@:
        ret

endp

proc progn uses esi edi ebx ecx edx, stopper, elms, envs
        mov     esi, [elms]
        mov     edx, esi
        cmp     [stopper], 0
        je      @f
        mov     edx, [stopper]
    @@:
        mov     ebx, [envs]
    @@:
        stdcall eval, 0, esi, [envs]
        cmp     [esi+NODE.next], NULL
        je      @f
        mov     esi, [esi+NODE.next]
        cmp     esi, edx
        jne     @b
    @@:
        ret
endp

proc length uses esi edi ebx, hLength, thelist, envs
        xor     ebx, ebx
        mov     esi, [thelist]
        mov     eax, [hLength]
        cmp     [esi+NODE.next], eax
        jne     argnum_error
        stdcall isAtom, esi
        cmp     eax, TRUE
        je      type_error
        stdcall eval, 0, [thelist], [envs]
        mov     esi, eax
        mov     eax, [eax+NODE.value]
        mov     edi, eax
        cmp     [eax+NODE.next], 0
        je      .exit
        mov     esi, [eax+NODE.next]
        inc     ebx
    @@:
        mov     esi, [esi+NODE.next]
        inc     ebx
        ;cmp     esi, [edi+NODE.prev]
        ;je      .exit
        ;cmp     edi, [esi+NODE.next]
        ;je      .exit
        cmp     esi, edi
        jnz     @b
    .exit:
        stdcall allocateInteger, ebx
        stdcall allocateNode, eax, INTEGER
        ret
endp

proc internalLength uses esi ebx ecx edx, nodeslist
        ; nodeslist is a list node which already
        ; extracted from node.value
        xor     eax, eax
        mov     esi, [nodeslist]
        mov     ebx, esi
        cmp     esi, 0
        je      .exit
        ;inc     eax
        cmp     [esi+NODE.next], 0
        je      .exit
    @@:
        mov     esi, [esi+NODE.next]
        inc     eax
        cmp     esi, ebx
        jnz     @b
    @@:
    .exit:
        ret
endp

proc varlistLength uses esi edi, vars
        xor     eax, eax
        mov     esi, [vars]
        mov     edi, esi
        cmp     esi, 0
        je      .exit
        cmp     [esi+VARIABLESTRUCT.hVar], 0
        je      .exit
        inc     eax
        cmp     [esi+VARIABLESTRUCT.next], 0
        je      .exit
    @@:
        mov     esi, [esi+VARIABLESTRUCT.next]
        inc     eax
        cmp     esi, edi
        jnz     @b
    .exit:
        ret
endp

proc cond uses esi edi ebx ecx edx, hCond, args, envs
        mov     esi, [args]
        cmp     esi, 0
        je      argnum_error
        stdcall isAtom, esi
        cmp     eax, TRUE
        je      type_error
        mov     edx, esi
        mov     ebx, esi
    .startloop:
        mov     esi, [esi+NODE.value]
        cmp     esi, 0
        je      .return_null
        cmp     [esi+NODE.next], 0
        je      .return_null
        mov     edi, [esi+NODE.next]
        stdcall cpyNode, esi
        stdcall eval, 0, esi, [envs]
        stdcall isTrue, eax
        cmp     eax, TRUE
        jne     @f
        stdcall cpyNode, edi
        stdcall eval, 0, edi, [envs]
        jmp     .exit
    @@:
        cmp     [ebx+NODE.next], 0
        je      @f
        mov     ebx, [ebx+NODE.next]
        mov     esi, ebx
        cmp     ebx, [hCond]
        jne     .startloop
    @@:
    .exit:
        ret
    .return_null:
        stdcall null
        jmp     .exit
endp

proc thedisplay, noargs, thenode, envs
        stdcall printNode, [outhnd], [thenode]
        ret
endp

macro numMakeLambda {
        stdcall allocateSymbol, lambdaname
        stdcall allocateNode, eax, SYMBOL
        mov     ebx, eax
        lea     esi, [varx]
        stdcall allocateSymbol, esi         ; make a arg lambda x
        stdcall allocateNode, eax, SYMBOL
        mov     ecx, eax
        stdcall pushNode, [head], eax       ; (cons exprs (cons x null))
        stdcall allocateSymbol, esi
        stdcall allocateNode, eax, SYMBOL
        stdcall allocateNode, eax, LIST     ; (x)
        stdcall pushNode, ebx, eax          ; (lambda (x))
        stdcall allocateNode, [head], LIST
        stdcall pushNode, ebx, eax          ; (lambda (x) exprs)
        stdcall lambda, ebx, [ebx+NODE.next], [envs]
}

macro numToFinish floattype {
    if floattype eq
        cmp     [isthisfloat], TRUE
        je      @f
        push    eax
        ficom   [int32_max]
        fstsw   ax
        sahf
        jnc     .to_long
        ficom   [int32_min]
        fstsw   ax
        sahf
        jc      .to_long
        pop     eax
        fistp   [tempint]
        stdcall allocateInteger, [tempint]
        stdcall allocateNode, eax, INTEGER
        ret
        .to_long:
        pop     eax
        fild    qword [int64_max]
        fcomip  st, st1
        jc      overflow_error
        fild    qword [int64_min]
        fcomip  st,st1
        jnz     .not_same
        jmp     overflow_error
        .not_same:
        jnc     overflow_error
        pop     eax
        fistp   qword [templng]
        lea     edi, [templng]
        stdcall allocateLong, edi
        stdcall allocateNode, eax, LONG
        ret
    @@:
    else
    end if
        fstp    qword [tempval]
        lea     edi, [tempval]
        stdcall allocateFloat, edi
        stdcall allocateNode, eax, FLOAT
        ret
}

macro makeNum command, floattype, onlyonearg, fortan {
        locals
            tempval dq  0.0
            tempint dd  0
            templng dq  0
            varx    du  'x',0
            isthisfloat db  FALSE
        endl
        cmp     [nums], 0
        je      argnum_error
        mov     esi, [nums]
        mov     ebx, [head]
    if onlyonearg eq
        cmp     [esi+NODE.next], ebx
        je      .make_lambda
    else
        cmp     [esi+NODE.next], ebx
        jne     argnum_error
    end if
        finit
        stdcall eval, 0, esi, [envs]
        cmp     [eax+NODE.type], INTEGER
        jne     @f
        mov     eax, [eax+NODE.value]
        mov     eax, [eax+NUMBERSTRUCT.value]
        fild    dword [eax]
    if onlyonearg eq
    else
        jmp     .next_loop
    end if
        jmp     .start_loop
    @@:
        cmp     [eax+NODE.type], LONG
        jne     @f
        mov     eax, [eax+NODE.value]
        mov     eax, [eax+NUMBERSTRUCT.value]
        fild    qword [eax]
    if onlyonearg eq
    else
        jmp     .next_loop
    end if
        jmp     .start_loop
    @@:
        cmp     [eax+NODE.type], FLOAT
        jne     type_error
        mov     eax, [eax+NODE.value]
        mov     eax, [eax+NUMBERSTRUCT.value]
    if floattype eq
        mov     [isthisfloat], TRUE
    end if
        fld     qword [eax]
    if onlyonearg eq
    else
        jmp     .next_loop
    end if
    .start_loop:
        mov     esi, [esi+NODE.next]
        stdcall eval, 0, esi, [envs]
        mov     ecx, [eax+NODE.type]
        mov     eax, [eax+NODE.value]
        mov     eax, [eax+NUMBERSTRUCT.value]
        cmp     ecx, INTEGER
        jne     @f
        fild    dword [eax]
        jmp     .next_loop
    @@:
        cmp     ecx, LONG
        jne     @f
        fild    qword [eax]
        jmp     .next_loop
    @@:
        cmp     ecx, FLOAT
        jne     type_error
    if floattype eq
        mov     [isthisfloat], TRUE
    end if
        fld     qword [eax]
    .next_loop:
    if command eq fdivp
        fldz
        fcomip  st, st1
        je      zero_division_error
    end if
    if onlyonearg eq
        command st1, st
    else
        command
    end if
        cmp     [esi+NODE.next], ebx
        jne     .start_loop
    .finish:
    if fortan eq
    else if fortan eq cotan
        fdivrp
    else
        fstp    qword [tempval]
    end if
        numToFinish floattype
    .make_lambda:
        numMakeLambda
        ret
}

proc addNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum faddp
endp

proc subNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum fsubp
endp

proc mulNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum fmulp
endp

proc divNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum fdivp, yes
endp

proc cosNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum fcos, yes, yes
endp

proc sinNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum fsin, yes, yes
endp

proc tanNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum fptan, yes, yes, yes
endp

proc cotanNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum fptan, yes, yes, cotan
endp

proc absNum uses esi edi ebx ecx edx, head, nums, envs
        makeNum fabs, , yes
endp

proc sqrt uses esi edi ebx ecx edx, head, nums, envs
        makeNum fsqrt, yes, yes
endp

macro compareString struct1, struct2, cmptype {
    local .return_false, .return_true, .start_loop, .end_loop, .exit
        mov     esi, struct1
        mov     edi, struct2
        cmp     esi, edi
        je      .return_false
        mov     ecx, [esi+STRINGSTRUCT.size]
        mov     edx, [edi+STRINGSTRUCT.size]
        cmp     ecx, edx
    if cmptype in <less, lessEqual>
        jg      .return_false
        jl      .return_true
    else if cmptype in <more, moreEqual>
        jl      .return_false
        jg      .return_true
    end if
        add     esi, STRINGSTRUCT.text
        add     edi, STRINGSTRUCT.text
    .start_loop:
        mov     ax, word [esi]
        cmp     ax, word [edi]
    if cmptype in <less, lessEqual>
        jl      .return_true
        jg      .return_false
    else if cmptype in <more, moreEqual>
        jg      .return_true
        jl      .return_false
    end if
        inc2w   esi, edi
        dec     ecx
        jnz     .start_loop
    .end_loop:
    if cmptype in <lessEqual, moreEqual>
        jmp     .return_true
    end if
    .return_false:
        mov     eax, FALSE
        jmp     .exit
    .return_true:
        mov     eax, TRUE
    .exit:
}

macro procCmps cmptype, structtype {
    proc cmptype#structtype uses esi edi ebx ecx edx, thestruct1,thestruct2
        mov     esi, [thestruct1]
        mov     edi, [thestruct2]
    if structtype eq Char
        mov     esi, [esi+CHARSTRUCT.hString]
        mov     edi, [edi+CHARSTRUCT.hString]
        compareString esi, edi, cmptype
    else
        compare#structtype esi, edi, cmptype
    end if
        ret
    endp
}

procCmps less,      String
procCmps lessEqual, String
procCmps more,      String
procCmps moreEqual, String

procCmps less,      Char
procCmps lessEqual, Char
procCmps more,      Char
procCmps moreEqual, Char

macro toCheckNumber what, thesource, towhere, [thetype] {
    common x = 0
    forward x = x + 1
    common b = 0
    forward
    local .stop1, .stop2
        b = b + 1
    .stop1:
        cmp     what, thetype
        jne     .stop2
    if thetype eq INTEGER
        fild    dword [thesource]
    else if thetype eq LONG
        fild    qword [thesource]
    else
        fld     qword [thesource]
    end if
    if b <> x
        jmp     towhere
    end if
    .stop2:
}

macro compareNumber struct1, struct2, cmptype {
    local .next_check, .comparison_check
    local .return_false, .return_true
    local .exit
        mov     esi, struct1
        mov     edi, struct2
        mov     ecx, [esi+NUMBERSTRUCT.type]
        mov     edx, [edi+NUMBERSTRUCT.type]
        mov     eax, [esi+NUMBERSTRUCT.value]
        toCheckNumber ecx, eax, .next_check, INTEGER, LONG, FLOAT
    .next_check:
        mov     eax, [edi+NUMBERSTRUCT.value]
        toCheckNumber edx, eax, .comparison_check, INTEGER, LONG, FLOAT
    .comparison_check:
        fcomip  st, st1
    if cmptype eq less
        jc      .return_false
        jnz     .return_true
    else if cmptype eq lessEqual
        jnc .return_true
        jz  .return_true
    else if cmptype eq more
        jnc     .return_false
        jmp     .return_true
    else if cmptype eq moreEqual
        jc  .return_true
        jz  .return_true
    end if
    .return_false:
        mov     eax, FALSE
        jmp     .exit
    .return_true:
        mov     eax, TRUE
    .exit:
}

procCmps less,      Number
procCmps lessEqual, Number
procCmps more,      Number
procCmps moreEqual, Number

macro toCheckIsNumber thenode, [thetype] {
        cmp     [thenode+NODE.type], thetype
        je      .return_false
}

proc isNumber uses esi edi ebx ecx edx, thenode
        mov     eax, TRUE
        stdcall isAtom, [thenode]
        cmp     eax, TRUE
        jne     .exit
        mov     esi, [thenode]
        toCheckIsNumber esi, SYMBOL, STRING, CHAR
        jmp     .exit
    .return_false:
        mov     eax, FALSE
    .exit:
        ret
endp

macro toCheckCmpNode thedest, thesrc, structtype {
        stdcall eval, 0, thesrc, [envs]
        mov     thedest, eax
        stdcall isAtom, eax
        cmp     eax, FALSE
        je      type_error
    if structtype eq Number
        stdcall isNumber, thedest
        cmp     eax, FALSE
        je      type_error
    else if structtype eq String
        cmp     [thedest+NODE.type], STRING
        jne     type_error
    else
        cmp     [thedest+NODE.type], CHAR
        jne     type_error
    end if
}

macro cmpNodeStruct cmptype, structtype {
    local .start_loop, .end_loop, .return_false, .exit
        mov     esi, [args]
        mov     eax, [head]
        cmp     esi, 0
        je      argnum_error
        cmp     [esi+NODE.next], eax
        je      argnum_error
        toCheckCmpNode ebx, esi, structtype
    .start_loop:
        mov     edi, [esi+NODE.next]
        cmp     edi, [head]
        je      .end_loop
        toCheckCmpNode ecx, edi, structtype
        stdcall cmptype#structtype, [ebx+NODE.value], [ecx+NODE.value]
        mov     ebx, ecx
        cmp     eax, FALSE
        je      .return_false
        mov     esi, edi
        jmp     .start_loop
    .end_loop:
        stdcall true
        jmp     .exit
    .return_false:
        stdcall false
    .exit:
}

macro procCmpsNode cmptype, structtype {
    proc cmptype#Node#structtype uses esi edi ebx ecx edx, head, args, envs
        cmpNodeStruct cmptype, structtype
        ret
    endp
}

procCmpsNode less,      Number
procCmpsNode lessEqual, Number
procCmpsNode more,      Number
procCmpsNode moreEqual, Number

procCmpsNode less,      String
procCmpsNode lessEqual, String
procCmpsNode more,      String
procCmpsNode moreEqual, String

procCmpsNode less,      Char
procCmpsNode lessEqual, Char
procCmpsNode more,      Char
procCmpsNode moreEqual, Char

qquotecount dd 0
proc quasiquote uses esi edi ebx ecx, hQQuote, results, envs
        ;local   header:DWORD
        inc     [qquotecount]
        ;memalloc sizeof.NODE
        ;mov     [header], eax
        mov     esi, [results]
        mov     eax, [hQQuote]
        cmp     esi, 0
        je      argnum_error
        cmp     [esi+NODE.next], 0
        je      argnum_error
        cmp     [esi+NODE.next], eax
        jne     argnum_error
        stdcall isAtom, esi
        cmp     eax, TRUE
        je      .return_as_is
        stdcall isNull, esi
        cmp     eax, TRUE
        je      .return_as_is
        stdcall isList, esi
        cmp     eax, FALSE
        je      .return_as_is
    .as_list:
        stdcall qqlist, [esi+NODE.value], [envs]
        cmp     [eax+NODE.next], 0
        je      @f
        stdcall allocateNode, eax, LIST
    @@:
        jmp     .exit
    .return_as_is:
        stdcall cpyNode, [results]
    .exit:
        dec     [qquotecount]
        ret
endp

macro tosamewithname thesrcsym {
    local .return_false, .exit
        mov     esi, [symbolnode]
        stdcall readExpression, thesrcsym
        mov     ebx, eax
        stdcall equalSymbol, [esi+NODE.value], [eax+NODE.value]
        cmp     eax, TRUE
        je      .exit
    .return_false:
        mov     eax, FALSE
    .exit:
        stdcall removeNode, ebx
}

proc samewithunqname uses esi edi ebx, symbolnode
        locals
            unqname du 'unquote',0
        endl
        lea     edi, [unqname]
        tosamewithname edi
        ret
endp

proc samewithqqname uses esi edi ebx, symbolnode
        tosamewithname qquotename
        ret
endp

proc samewithunqsname uses esi edi ebx, symbolnode
        locals
            unqname du 'unquote-splicing',0
        endl
        lea     edi, [unqname]
        tosamewithname edi
        ret
endp

macro splicinglist thedest, thesrc {
    local .splicing, .end_splicing
        push    ebx ecx
        mov     ebx, thesrc
        mov     ecx, thesrc
    .splicing:
        stdcall cpyNode, ecx
        stdcall pushNode, thedest, eax
        mov     ecx, [ecx+NODE.next]
        cmp     ecx, 0
        je      .end_splicing
        cmp     ecx, ebx
        jne     .splicing
    .end_splicing:
        stdcall removeNode, edx
        pop     ecx edx
}

proc qqlist uses esi edi ebx ecx edx, elems, envs
        local header:DWORD, inlist:BYTE
        mov     [inlist], FALSE
        memalloc sizeof.NODE
        mov     [header], eax
        mov     esi, [elems]
        mov     edi, esi
    .start_loop:
        stdcall isList, esi
        cmp     eax, TRUE
        jne     @f
        stdcall qqlist, [esi+NODE.value], [envs]
        cmp     [eax+NODE.type], LIST
        je      .proceed_to_next
        cmp     [eax+NODE.next], 0
        je      .proceed_to_next
        splicinglist [header], eax
        jmp     .next_loop
      .proceed_to_next:
        stdcall pushNode, [header], eax
        jmp     .next_loop
    @@:
        stdcall samewithunqname, esi
        cmp     eax, TRUE
        jne     @f
        ;; process unquote
        cmp     [qquotecount], 1
        jg      .bypass
        stdcall eval, 0, [esi+NODE.next], [envs]
        stdcall pushNode, [header], eax
        mov     esi, [esi+NODE.next]
        jmp     .exit
        jmp     .next_loop
    @@:
        stdcall samewithunqsname, esi
        cmp     eax, TRUE
        jne     @f
        ;; process unquote-splicing
        cmp     [qquotecount], 1
        jg      .bypass
        stdcall eval, 0, [esi+NODE.next], [envs]
        mov     edx, eax
        cmp     [eax+NODE.type], LIST
        ;jmp     .not_splicing
        jne     .not_splicing
        splicinglist [header], [eax+NODE.value]
        mov     esi, [esi+NODE.next]
        jmp     .exit
        ;jmp     .next_loop
      .not_splicing:
        stdcall pushNode, [header], eax
        jmp     .exit
    @@:
        stdcall samewithqqname, esi
        cmp     eax, TRUE
        jne     @f
        ;; process quasiquoting again
        stdcall cpyNode, esi
        stdcall pushNode, [header], eax
        stdcall quasiquote, esi, [esi+NODE.next], [envs]
        stdcall pushNode, [header], eax
        mov     esi, [esi+NODE.next]
        jmp     .next_loop
    @@:
        jmp     .copy_node
    .bypass:
        dec     [qquotecount]
    .copy_node:
        stdcall cpyNode, esi
        stdcall pushNode, [header], eax
    .next_loop:
        cmp     [esi+NODE.type], LIST
        jne     @f
        mov     [inlist], TRUE
    @@:
        mov     esi, [esi+NODE.next]
        cmp     esi, edi
        jne     .start_loop
    .exit:
        mov     eax, [header]
        cmp     [inlist], TRUE
        jne     .not_list
        stdcall allocateNode, eax, LIST
      .not_list:
        ret
endp
