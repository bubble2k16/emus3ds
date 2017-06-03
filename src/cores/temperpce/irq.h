#ifndef IRQ_H
#define IRQ_H

typedef enum
{
  IRQ_CD    = 0x1,
  IRQ_VDC   = 0x2,
  IRQ_TIMER = 0x4
} irq_type_enum;

// 8B

typedef struct
{
  u32 enable;
  u32 status;
} irq_struct;

extern irq_struct irq;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 


u32 irq_read_enable();
u32 irq_read_status();
void irq_write_enable(u32 value);
void irq_write_status(u32 value);
void raise_interrupt(u32 irq_bit);

void initialize_irq();
void reset_irq();

void irq_load_savestate(savestate_load_type savestate_file);
void irq_store_savestate(savestate_store_type savestate_file);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
