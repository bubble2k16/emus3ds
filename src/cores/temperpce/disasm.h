#ifndef DISASM_H
#define DISASM_H

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 


u32 disasm_instruction(char *output, u32 *_pc);
void disasm_function(u32 pc);
void disasm_block(u32 pc, u32 count);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
