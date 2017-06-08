#ifndef PSGQUEUE_H
#define PSGQUEUE_H

#include "common.h"

typedef struct
{
    int     psg_reg;
    u64     clock;
    u32      value;
} psgqueue_struct;


#define PSGQUEUE_SIZE       32768

extern psgqueue_struct psgqueue[PSGQUEUE_SIZE];

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

void psgqueue_add(int psg_reg, u64 cpuclock, u32 value);
bool psgqueue_read(u64 cpuclock, int *psg_reg, u32 *value);
bool psgqueue_read_next(u64 *cpuclock, int *psg_reg, u32 *value);
bool psgqueue_peek_next(u64 *cpuclock, int *psg_reg, u32 *value);
bool psgqueue_isempty();
void reset_psgqueue();

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 

#endif
