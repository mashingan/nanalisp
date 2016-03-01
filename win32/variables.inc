inhnd   dd  ?
outhnd  dd  ?
numwrtn dd  0
parenthesis dd  0
hBuffin dd  0
hDummy  dd  0
aphostrophs db  FALSE
fmtflt  du  '%g',0
prompt  du  '? ',0

;hTbyte  dt  0.0


NO_ERROR                 = 0x00
TYPE_ERROR               = 0x01
ARGNUM_ERROR             = 0x02
NO_BINDING_ERROR         = 0x03
INVALID_EXPRESSION_ERROR = 0x04
STACK_OVERFLOW_ERROR     = 0x05
UNIMPLEMENTED_ERROR      = 0x06

typeerror       du 'TYPE ERROR',0
argnumerror     du 'ARGNUM ERROR',0
nobindingerror  du 'NO BINDING ERROR',0
invldexprerror  du 'INVALID EXPRESSION ERROR',0
stackerror      du 'STACK OVERFLOW ERROR',0
unimplerror     du 'UNIMPLEMENTED ERROR',0

VARTABLESIZE = 32
VARTABLE    dd  VARTABLESIZE dup (0)
PRIMFUNTABLE    VARIABLESTRUCT <0>
FUNSTACKTRACE   dd 0

UNDEF   = 0x00
SYMBOL  = 0x01
CHAR    = 0x02
STRING  = 0x03
INTEGER = 0x04
FLOAT   = 0x05
PAIR    = 0x06
LIST    = 0x07
EXPRS   = 0x08
LAMBDA  = 0x09
FUNCTION= 0x0A

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
procHeap dd     0
