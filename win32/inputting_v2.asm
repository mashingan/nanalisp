format PE console
entry start
include 'win32w.inc'
include 'miscmacros.inc'
include 'convert.inc'
include 'parser.inc'
include 'linkedlist.inc'
include 'primitives.inc'

MAXBUFFER = 512

section '.data' data readable writeable
include 'variables.inc'

theresult du '=> ',0

;; WRONG
;str1    du '(progn (setq hello (lambda () (display "hello world")'
        ;du ' (hello)))'
        ;du ' (hello))',0
;str1 du '(begin (setf factorial (lambda (x) (cond ((equal? x 1) 1)'
     ;du '(true (* x (factorial (- x 1))))))) (factorial 10))',0


;; ERROR


;; OK
;str1 du '(begin (setf factorial (lambda (x y) (cond ((equal? x 1) y)'
     ;du '(true (factorial (- x 1) (* x y)))))) (factorial 5 1))',0
;str1 du '(begin (setf factorial (lambda (x) (cond ((equal? x 1) 1)'
     ;du '(true (* (factorial (- x 1)) x))))) (factorial 5))',0
;str1 du '(progn (setq 1+ (lambda (x) (+ x 1)) map (lambda (f x) (cond'
     ;du '((null? x) (quote ())) (true (cons (f (car x)) (map f'
     ;du ' (cdr x))))))) (map 1+ (quote (1 2 3))))',0
;str1 du '(begin (setf factorial (lambda (x y) (cond ((equal? x 1) y)'
     ;du '(true (factorial (- x 1) (* x y)))))) (factorial 20 1))',0
;str1 du '(/ 5 10 0 13)',0
;; return overflow
;str1 du '(begin (setf factorial (lambda (x) (cond ((equal? x 1) 1)'
     ;du '(true (* (factorial (- x 1)) x))))) (factorial 40))',0
;str1 du '(< 5 5.5 6 6.1 7)',0
;str1 du '(< 5 5 6 6 7)',0
;str1 du '(<= 5 5 6 6 7 )',0
;str1 du '(< 5)',0   ; return argnum error
;str1 du '(string<? "helli" "hello")',0
;str1 du '(string<=? "helli" "hello")',0
;str1 du '(string>? "helli" "hello")',0
;str1 du '(string>=? "helli" "hello")',0
;str1 du '(string>=? "hello" "hello")',0
;str1 du '(quasiquote hello)',0
;str1 du '(quasiquote (quote hello))',0
;str1 du '(quasiquote (unquote (+ 1 2)))',0
;str1 du '(quasiquote (list (unquote (+ 1 2)) 4))',0
;str1 du '((lambda (name) (quasiquote (list (unquote name) (quote '
     ;du '(unquote name))))) (quote a))',0
str1 du "((lambda (name) `(list (unquote name) ',name)) 'a)",0
;str1 du '(begin (setf map (lambda (f x) (cond ((null? x) (quote ())) (true '
     ;du '(cons (f (car x)) (map f (cdr x))))))) (map abs (quote '
     ;du '(4 -5 6))))',0
;str1 du '(begin (setf map (lambda (f x) (cond ((null? x) (quote ())) (true '
     ;du '(cons (f (car x)) (map f (cdr x))))))) (quasiquote (a (unquote '
     ;du '(+ 1 2)) (unquote-splicing (map abs (quote (4 -5 6)))) b)))',0
;str1 du '(quote (hello . world))',0
;str1 du '(quasiquote ((foo (unquote (- 10 3))) (cons (unquote-splicing'
     ;du ' (cdr (quote (c)))) (unquote (car (quote (cons)))))))',0
;str1 du '(quasiquote (cons (foo (unquote (- 10 3))) (cons ('
     ;du 'unquote-splicing (cdr (quote (c)))) (unquote (car (quote '
     ;du ' (cons)))))))',0
;str1 du '(cons (quote (foo 7)) (car (quote (cons))))',0
;str1 du '(quasiquote ((foo (unquote (- 10 3))) (unquote-splicing (cdr '
     ;du '(quote (c)))) . (unquote (car (quote (cons))))))',0
;str1 du "`((foo ,(- 10 3) ,@(cdr '(c))) . (unquote (car '(cons))))",0
;str1 du '(quasiquote (a (quasiquote (b (unquote (foo (unquote (+ 1 3)) d))'
     ;du ' e)) f))',0
;str1 du '`(a `(b ,(foo ,(+ 1 3) d) e) f)',0
;str1 du '(begin (setf map (lambda (f x) (cond ((null? x) (quote ()))'
     ;du ' (true (cons (f (car x)) (map f (cdr x))))))) '
     ;du '(quasiquote (10 5 (unquote (sqrt 4)) (unquote-splicing ('
     ;du 'map sqrt (quote (16 9)))) 8)))',0
;str1 du "(begin (setf map (lambda (f x) (cond ((null? x) '())"
     ;du " (true (cons (f (car x)) (map f (cdr x))))))) "
     ;du "`(10 5 ,(sqrt 4) ,@(map sqrt '(16 9)) 8))",0
;str1 du '(quasiquote (quote hello-world))',0
;str1 du "(quote (hello-world))",0
;str1 du "'hello-world",0
;str1 du "'(hello world)",0
;str1 du "(quasiquote '(hello-world))",0
;str1 du "'()",0
;str1 du "(begin (set! greeting #t) (cond (greeting 'hello-world)"
     ;du " (true 'nice-day)))",0
;str1 du "(begin (setf hello 'world) `(hello ,hello nice day))",0
;str1 du "''hello",0
;str1 du "''(hello)",0
;str1 du "(begin (setf name 'andy) `'(,name))",0
;str1 du "(quasiquote (quote (hello-world)))",0
;str1 du "`'(hello-world)",0
;str1 du "(quasiquote ((hello world)))",0
;str1 du '`((hello world))',0
;str1 du "(setq 'hello 'world)",0
;str1 du "(+ '(hello) 4)",0
;str1 du "(<= 5 'hello)",0

lenstr1 = $ - str1

section '.text' code readable executable
start:
        initProgram

    .startloop:    ; total 42 lines
        mov     edi, [hBuffin]
        clearBuffer edi, MAXBUFFER
        writeIt prompt
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
        stdcall convertToQuote, edi
        invoke  lstrlenW, edi
        lea     eax, [eax*2]
        mov     word [edi+eax-4], 0
        ;cinvoke wsprintf, bufout, fmt, buffin
        ;writeLn [outhnd], bufout

        stdcall readExpression, edi
        mov     ebx, eax
        stdcall eval, 0, eax, 0
        checkNode eax
        ;stdcall removeNode, eax
    .to_finish:
        stdcall removeNode, ebx
        ;checkFUNSTACKTRACE
        ;cleanFUNSTACKTRACE
        jmp     .startloop

        ;stdcall varlistLength, PRIMFUNTABLE
        ;checkItr eax
        ;checkItr lenstr1
        ;stdcall checkInput, str1
        ;cmp     [parenthesis], 0
        ;jne     .invalid_expression
        ;stdcall normalizeInput, str1
        ;stdcall convertToQuote, str1
        ;writeLn str1
        ;stdcall  checkInput, str1
        ;cmp     [parenthesis], 0
        ;jne     .invalid_expression
        ;stdcall readExpression, str1
        ;mov     ebx, eax
        ;checkNode eax
        ;stdcall eval, 0, eax, 0
        ;push    eax ebx
        ;writeIt theresult
        ;pop     ebx eax
        ;checkNode eax
        ;stdcall removeNode, eax
    ;.to_finish:
        ;stdcall removeNode, ebx
        ;checkFUNSTACKTRACE
;
        ;xor     eax, eax
        ;jmp     finish
    .invalid_expression:
        writeLn INVALID_EXPRESSION_ERRORtext
        mov     eax, INVALID_EXPRESSION_ERROR
        jmp     exit
    
    finish:
        push    eax
        cleanupProgram
        pop     eax

    exit:
        invoke  ExitProcess, eax
        ret

        the_error\
            no_binding_error, NO_BINDING_ERROR,\
            argnum_error, ARGNUM_ERROR,\
            type_error, TYPE_ERROR,\
            stack_error, STACK_OVERFLOW_ERROR,\
            zero_division_error, ZERO_DIVISION_ERROR,\
            unimplemented_error, UNIMPLEMENTED_ERROR,\
            overflow_error, OVERFLOW_ERROR


proc initVarTable uses esi eax ebx ecx
        mov     ebx, 0
        mov     ecx, VARTABLESIZE
    @@:
        lea     esi, [VARTABLE+ebx]
        stdcall allocateVariable, NULL
        mov     [esi], eax
        add     ebx, 4
        dec     ecx
        jnz     @b
        ret
endp

proc checkVARLIST uses esi edi eax ebx ecx edx, varlist
        locals
            colon du ':',0
        endl
        mov     esi, [varlist]
        cmp     esi, 0
        je      .exit
        mov     ebx, esi
    @@:
        mov     eax, [esi+VARIABLESTRUCT.hVar]
        cmp     eax, NULL
        je      @f              ; No variables list
        stdcall printNode, [outhnd], eax
        lea     edi, [colon]
        writeIt edi   ; Only protects protected-registers
        mov     eax, [esi+VARIABLESTRUCT.hValue]
        cmp     eax, 0
        je      .null_value
        stdcall printNode, [outhnd], eax
      .null_value:
        spacing [outhnd]
      .null_node:
        mov     esi, [esi+VARIABLESTRUCT.next]
        ;pop     esi
        cmp     esi, NULL
        je      @f
        cmp     esi, ebx
        je      @f
        jmp     @b
    @@:
        newline
    .exit:
        ret
endp

proc checkVARTABLE uses esi edi eax ebx ecx
        mov     ecx, VARTABLESIZE
        mov     ebx, 0
    .startloop:
        lea     esi, [VARTABLE+ebx]
        mov     esi, [esi]
        cmp     esi, 0
        je      @f
        cmp     [esi+VARIABLESTRUCT.hVar], NULL
        je      @f
        stdcall checkVARLIST, esi
    @@:
        add     ebx, 4
        dec     ecx
        jnz     .startloop
        ;loop    .startloop
        ret
endp

section '.idata' import data readable
library kernel32, 'kernel32.dll',\
        user32, 'user32.dll',\
        msvcrt, 'msvcrt.dll'

include 'api\kernel32.inc'
include 'api\user32.inc'
import msvcrt, wprintf, 'wprintf'
