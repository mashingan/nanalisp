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

;; WRONG
;str1    du '(progn (setq hello (lambda () (display "hello world")'
        ;du ' (hello)))'
        ;du ' (hello))',0


;; ERROR


;; OK
;str1    du  '(progn (setq hello (lambda (x y) (cons x (cons y null))))'
        ;du  ' (hello 1 2))',0
;str1    du  '((progn (setq hello (lambda (x y) (cons x (cons y null))))'
        ;du  ' (hello (quote hello-world))) (quote nice-going))',0
;str1    du  '((progn (setq hello (lambda (x y) (cons x (cons y null))))'
        ;du  ' (hello 1)) 2)',0
;str1    du  '(progn (setq hello (lambda (x y) (cons x (cons y null))))'
        ;du  ' (hello 1))',0
;str1    du  '(((progn (setq hello (lambda (x y) (cons x (cons y null))))'
        ;du  ' (hello 1))) 2)',0
;str1    du  '(((progn (setq hello (lambda (x y) (cons x (cons y null))))'
        ;du  ' (hello 1))) 2 3 4 5)',0
;str1    du  '(progn (setq hello (lambda () (quote world))) (hello))',0
;str1    du  '(cons 1 (cons 2 null))',0
;str1    du  '(cdr (quote (1 2 3 4 5)))',0
;str1    du  '(progn (set! hello (quote (1 2 3 4)))'
        ;du  '  (cons (car hello) (cdr hello)))',0
;str1    du  '(cond (5 true) (true false))',0
;str1    du  '(cond ((atom? (quote (1 2 3))) false) (true (quote (1 2))))',0
;str1    du  '((lambda (x) (cons (cdr x) (car x))) (quote (1 2 3 4)))',0
;str1    du  '(lambda (x) (cons (cdr x) (car x)))',0
;str1    du  '(begin (setf greeting #f)'
        ;du  ' (cond (greeting (quote (hello world)))'
        ;du         '(true (quote (nice going)))))',0
;str1    du  '(cdr (quote (1)))',0
;str1    du  '(car null)',0
;str1    du  '(display 5.5)',0
;str1    du  '(eq? 1.25 1.25)',0
;str1    du  '(equal? 2.235 2.235)',0
;str1    du '(progn (setq hello (quote (1 2.235 3)))'
        ;du ' (equal? hello (quote (1 2.235 3))))',0
;str1    du  '(+ 1 2 3)',0
;str1    du  '(+ 1)',0
;str1    du  '((+ 1) 2)',0
;str1    du  '(length (quote (1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)))',0
;str1    du  '(+ 1.5 1.5 3)',0
;str1    du  '((+ 1) 2.5)',0
;str1 du '0',0
;str1    du  '(null? (quote ()))',0
;str1 du '(cdr (quote ()))',0
;str1 du '(cons (quote ()) (quote hello-world))',0
;str1 du '(car null)',0
;str1 du '(cdr null)',0
;str1 du '(cdr ())',0
;str1 du '(cdr (quote ((quote ()))))',0
;str1 du '(begin (setq x (quote ((quote ())))) (cdr x))',0
;str1 du '(null? (cdr (quote ((quote ())))))',0
;str1 du '(null? (quote (( ()))))',0
;str1    du  '(progn (setq hello (lambda (x)'
        ;du  '(cond ((null? x) null)'
        ;du  '(true (cons (car x) (hello (cdr x)))))))'
        ;du  '(hello (quote (1 nice 2 world (quote ()) ))))',0
;str1 du '(cdr ((quote ())))',0
;str1    du  '(progn (setq hello (lambda (x)'
        ;du  '(cond ((null? x) null)'
        ;du  '(true (cons (car x) (hello (cdr x)))))))'
        ;du  '(hello (quote (1 nice 2 world () ))))',0
;str1    du  '(progn (setq hello (lambda (x)'
        ;du  '(cond ((null? x) null)'
        ;du  '(true (cons (cons (car x) (quote ())) (hello (cdr x)))))))'
        ;du  '(hello (quote (1 2))))',0
str1 du '(begin (setq 1+ (lambda (x) (+ x 1))'
     du ' hello (lambda (x) (cond ((null? x) (quote ()))'
     du ' (true (cons (1+ (car x)) (hello (cdr x)))))))'
     du ' (hello (quote (1 2 3 4))))',0
lenstr1 = $ - str1
;str1    du  '1.45e10',0
;str2    du  '1.45',0

theresult du '=> ',0

section '.text' code readable executable
start:
        initProgram

        stdcall varlistLength, PRIMFUNTABLE
        checkItr eax
        checkItr lenstr1
        stdcall checkInput, str1
        cmp     [parenthesis], 0
        jne     .invalid_expression
        stdcall normalizeInput, str1
        stdcall readExpression, str1
        mov     ebx, eax
        checkNode eax
        stdcall eval, 0, eax, 0
        push    eax ebx
        writeIt [outhnd], theresult
        pop     ebx eax
        checkNode eax
        stdcall removeNode, eax
        stdcall removeNode, ebx
        
        checkFUNSTACKTRACE

        xor     eax, eax
        jmp     finish
    .invalid_expression:
        writeLn [outhnd], invldexprerror
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
            no_binding_error, nobindingerror, NO_BINDING_ERROR,\
            argnum_error, argnumerror, ARGNUM_ERROR,\
            type_error, typeerror, TYPE_ERROR,\
            stack_error, stackerror, STACK_OVERFLOW_ERROR,\
            unimplemented_error, unimplerror, UNIMPLEMENTED_ERROR


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
        writeIt [outhnd], edi   ; Only protects protected-registers
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
        newline [outhnd]
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
