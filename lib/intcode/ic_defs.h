#ifndef INTCODE_DEFS_H_
#define INTCODE_DEFS_H_

#define IC_VERBOSE	1
#define IC_QUIET	0

#define IC_OP__ADD	1
#define IC_OP__MUL	2
#define IC_OP__INP	3
#define IC_OP__OUT	4
#define IC_OP__JNZ	5
#define IC_OP__JEZ	6
#define IC_OP__LES	7
#define IC_OP__EQL	8
#define IC_OP__SRL	9
#define IC_OP__HLT	99

#define IC_BF__DRT	0x1		// buffer attr dirty bit
#define IC_BF__NCE	0x2		// buffer attr mode ((O)NCE = check this position then inc, ~(O)NCE = repeatedly check this position)

#define IC_PC__INC_ENA	0xFF
#define IC_PC__INC_DIS	0x0

#endif
