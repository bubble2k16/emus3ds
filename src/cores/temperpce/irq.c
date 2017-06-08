#include "common.h"

irq_struct irq;

u32 irq_read_enable()
{
  irq.status &= ~IRQ_TIMER;
  memory.io_buffer = irq.enable | (memory.io_buffer & ~0x7);
  return memory.io_buffer;
}

u32 irq_read_status()
{
  memory.io_buffer = (memory.io_buffer & ~(IRQ_TIMER | IRQ_VDC | IRQ_CD)) |
   (irq.status & (IRQ_TIMER | IRQ_VDC | IRQ_CD));
  return memory.io_buffer;
}

void irq_write_enable(u32 value)
{
  u32 old_irq_status =
   ((~irq.enable) & irq.status) & (IRQ_TIMER | IRQ_VDC | IRQ_CD);
  memory.io_buffer = value;
  irq.enable = value;

  if((((~irq.enable) & irq.status) & (IRQ_TIMER | IRQ_VDC | IRQ_CD)) !=
   old_irq_status)
  {
    cpu.alert = 1;
    cpu.irq_raised = irq.status & (IRQ_TIMER | IRQ_VDC | IRQ_CD);
  }
}

void irq_write_status(u32 value)
{
  memory.io_buffer = value;
  irq.status = (irq.status & (IRQ_VDC | IRQ_CD)) |
   ~(IRQ_TIMER | IRQ_VDC | IRQ_CD);
  cpu.irq_raised &= irq.status;
}

void raise_interrupt(u32 irq_bit)
{
  irq.status |= irq_bit;
  cpu.alert = 1;
  cpu.irq_raised |= irq_bit;
}

void initialize_irq()
{
}

void reset_irq()
{
  irq.status = (irq.status & (IRQ_VDC | IRQ_CD)) |
   ~(IRQ_TIMER | IRQ_VDC | IRQ_CD);
  irq.enable = 0x0;
}

#define irq_savestate_builder(type, type_b, version_gate)                     \
void irq_##type_b##_savestate(savestate_##type_b##_type savestate_file)       \
{                                                                             \
  file_##type##_variable(savestate_file, irq.enable);                         \
  file_##type##_variable(savestate_file, irq.status);                         \
}                                                                             \

build_savestate_functions(irq);

