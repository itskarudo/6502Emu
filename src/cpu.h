#ifndef __CPU_H__
#define __CPU_H__

#include "bus.h"
#include "opcode.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum 
{
  C = (1 << 0), // carry
  Z = (1 << 1), // zero
  I = (1 << 2), // IRQ disable
  D = (1 << 3), // decimal mode
  B = (1 << 4), // brk
  U = (1 << 5), // unused
  V = (1 << 6), // overflow
  N = (1 << 7)  // negative
} CPUFlags;

typedef struct CPU
{
  Bus* bus;

  uint8_t a;   // Accumulator register
  uint8_t x;   // X general register
  uint8_t y;   // Y general register
  uint8_t sp;  // Stack pointer register
  uint16_t pc; // Program counter register
  uint8_t st;  // Status register

  uint8_t cycles;
  uint8_t opcode;
  uint8_t fetched;

  uint16_t addr_abs;
  uint16_t addr_rel;
  uint16_t temp;

  Opcode* lookup; // pointer to opcode lookup table

} CPU;

CPU*    cpu_create(void);
void    cpu_destroy(CPU* cpu);
void    cpu_connect_bus(CPU* cpu, Bus* bus);
void    cpu_reset(CPU* cpu);
uint8_t cpu_read(CPU* cpu, uint16_t address);
void    cpu_write(CPU* cpu, uint16_t address, uint8_t value);
bool    cpu_flag_get(CPU* cpu, CPUFlags flag);
void    cpu_flag_set(CPU* cpu, CPUFlags flag, bool value);
void    cpu_clock(CPU* cpu);
uint8_t cpu_fetch(CPU* cpu);
void    cpu_irq(CPU* cpu);
void    cpu_nmi(CPU* cpu);
bool    cpu_complete(CPU* cpu);
char*   cpu_disassemble(CPU* cpu);


#endif /* __CPU_H__ */
