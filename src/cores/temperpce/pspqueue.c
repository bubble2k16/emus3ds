// TemperPCE for 3DS
//

#include "psgqueue.h"

psgqueue_struct psgqueue[PSGQUEUE_SIZE];
int     psgqueue_read_ptr = 0;
int     psgqueue_write_ptr = 0;


void psgqueue_add(int psg_reg, u64 cpuclock, u32 value)
{
    psgqueue[psgqueue_write_ptr].psg_reg = psg_reg;
    psgqueue[psgqueue_write_ptr].clock = cpuclock;
    psgqueue[psgqueue_write_ptr].value = value;

    psgqueue_write_ptr = (psgqueue_write_ptr + 1) % PSGQUEUE_SIZE;
}


bool psgqueue_read(u64 cpuclock, int *psg_reg, u32 *value)
{
    if (psgqueue_read_ptr == psgqueue_write_ptr)
        return false;

    if (cpuclock > psgqueue[psgqueue_read_ptr].clock)
    {
        *psg_reg = psgqueue[psgqueue_read_ptr].psg_reg;
        *value = psgqueue[psgqueue_read_ptr].value;
        psgqueue_read_ptr = (psgqueue_read_ptr + 1) % PSGQUEUE_SIZE;
        return true;
    }
    return false;
}

bool psgqueue_read_next(u64 *cpuclock, int *psg_reg, u32 *value)
{
    if (psgqueue_read_ptr == psgqueue_write_ptr)
        return false;

    *psg_reg = psgqueue[psgqueue_read_ptr].psg_reg;
    *value = psgqueue[psgqueue_read_ptr].value;
    *cpuclock = psgqueue[psgqueue_read_ptr].clock;
    psgqueue_read_ptr = (psgqueue_read_ptr + 1) % PSGQUEUE_SIZE;
    return true;
}

bool psgqueue_peek_next(u64 *cpuclock, int *psg_reg, u32 *value)
{
    if (psgqueue_read_ptr == psgqueue_write_ptr)
        return false;

    *psg_reg = psgqueue[psgqueue_read_ptr].psg_reg;
    *value = psgqueue[psgqueue_read_ptr].value;
    *cpuclock = psgqueue[psgqueue_read_ptr].clock;
    return true;
}

bool psgqueue_isempty()
{
    return (psgqueue_read_ptr == psgqueue_write_ptr);
}


void reset_psgqueue()
{
    psgqueue_read_ptr = 0;
    psgqueue_write_ptr = 0;
    
}
