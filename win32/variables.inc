inhnd   dd  ?
outhnd  dd  ?
procHeap dd 0
numwrtn dd  0
parenthesis dd  0
hBuffin dd  0
hDummy  dd  0
aphostrophs db  FALSE
fmtflt  du  '%g',0
fmtlng  du  '%lld',0
prompt  du  '? ',0

;hTbyte  dt  0.0

INT32_MAX = +2147483647
INT32_MIN = -2147483648
INT32_LEN = 10
INT64_MAX = +9223372036854775807
INT64_MIN = -9223372036854775808
INT64_LEN = 18

int32_max   dd  INT32_MAX
int32_min   dd  INT32_MIN
int64_max   dq  INT64_MAX
int64_min   dq  INT64_MIN


macro enumsymbols [enums] {
    common x = 0
    forward
        enums = x
        x = x + 1
}

enumsymbols NO_ERROR, TYPE_ERROR, ARGNUM_ERROR, NO_BINDING_ERROR,\
            INVALID_EXPRESSION_ERROR, STACK_OVERFLOW_ERROR,\
            UNIMPLEMENTED_ERROR, ZERO_DIVISION_ERROR, OVERFLOW_ERROR

typeerror       du 'TYPE ERROR',0
argnumerror     du 'ARGNUM ERROR',0
nobindingerror  du 'NO BINDING ERROR',0
invldexprerror  du 'INVALID EXPRESSION ERROR',0
stackerror      du 'STACK OVERFLOW ERROR',0
unimplerror     du 'UNIMPLEMENTED ERROR',0
zerodiverror    du 'ZERO DIVISION ERROR',0
overflowerror   du 'OVERFLOW ERROR',0

VARTABLESIZE = 32
VARTABLE    dd  VARTABLESIZE dup (0)
PRIMFUNTABLE    VARIABLESTRUCT <0>
FUNSTACKTRACE   dd 0

enumsymbols UNDEF, SYMBOL, CHAR, STRING, INTEGER, LONG,\
            FLOAT, PAIR, LIST, EXPRS, LAMBDA, FUNCTION

;undef   du  'UNDEFINED',0
;symb    du  'SYMBOL',0
;chr     du  'CHAR',0
;strg    du  'STRING',0
;intg    du  'INTEGER',0
;flot    du  'FLOAT',0
;pairtyp du  'PAIR',0
;lsttype du  'LIST',0
;exprs   du  'EXPRESSION',0
;lmbtyp  du  'LAMBDA',0
;function du  'FUNCTION',0
;;
;nametype    dd  undef, symb, intg, flot, strg, pairtyp, lsttype, exprs
            ;dd  lmbtyp
;typefmt du 'type for %s is %s',0


;list    NODE    <0>
