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


macro enumsymbols enumname, [enums] {
    common x = 0
    forward
        enums = x
        x = x + 1
    forward
        enums#text du #`enums,0
    common
    enumname:
    forward
        dd  enums#text
}

enumsymbols errorenum,\
    NO_ERROR, TYPE_ERROR, ARGNUM_ERROR, NO_BINDING_ERROR,\
    INVALID_EXPRESSION_ERROR, STACK_OVERFLOW_ERROR,\
    UNIMPLEMENTED_ERROR, ZERO_DIVISION_ERROR, OVERFLOW_ERROR

VARTABLESIZE = 32
VARTABLE    dd  VARTABLESIZE dup (0)
PRIMFUNTABLE    VARIABLESTRUCT <0>
FUNSTACKTRACE   dd 0

enumsymbols datatypeenum,\
    UNDEF, ATOM, SYMBOL, CHAR, STRING, INTEGER, LONG,\
    FLOAT, NUMBER, PAIR, LIST, EXPRS, LAMBDA, FUNCTION
