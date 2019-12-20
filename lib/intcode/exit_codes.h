#ifndef INTCODE_EXIT_CODES_H_
#define INTCODE_EXIT_CODES_H_

#define EXIT__NORMAL		0	// successful halt (executed op code IC_OP__HLT)
#define EXIT__ABNORMAL		1	// unknown operation attempted
#define EXIT__INPUT_EMPTY	2	// hit end of input buffer
#define EXIT__OUTPUT_FULL	3	// hit end of output buffer
#define EXIT__MISSING_INPUT	4	// input buffer element is not set to dirty
#define EXIT__OUTPUT_READY	5	// output stream has new value, halted to allow access

#endif
