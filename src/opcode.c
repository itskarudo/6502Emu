#include "cpu.h"
#include "opcode.h"

uint8_t addr_abs(CPU* cpu)
{
  uint8_t lo = cpu_read(cpu, cpu->pc);
  cpu->pc++;
  uint8_t hi = cpu_read(cpu, cpu->pc);
  cpu->pc++;

  cpu->addr_abs = (hi << 8) | lo;

  return 0;
}

uint8_t addr_abx(CPU* cpu)
{

  uint8_t lo = cpu_read(cpu, cpu->pc);
  cpu->pc++;
  uint8_t hi = cpu_read(cpu, cpu->pc);
  cpu->pc++;

  cpu->addr_abs = (hi << 8) | lo;
  cpu->addr_abs += cpu->x;

  if ((cpu->addr_abs & 0xFF00) != (hi << 8))
    return 1;
  else
    return 0;
}

uint8_t addr_aby(CPU* cpu)
{

  uint8_t lo = cpu_read(cpu, cpu->pc);
  cpu->pc++;
  uint8_t hi = cpu_read(cpu, cpu->pc);
  cpu->pc++;

  cpu->addr_abs = (hi << 8) | lo;
  cpu->addr_abs += cpu->y;

  if ((cpu->addr_abs & 0xFF00) != (hi << 8))
    return 1;
  else
    return 0;
}

uint8_t addr_imm(CPU* cpu)
{
  cpu->addr_abs = cpu->pc++;
  return 0;
}

uint8_t addr_imp(CPU* cpu)
{
  cpu->fetched = cpu->a;
  return 0;
}

uint8_t addr_ind(CPU* cpu)
{
  uint8_t ptr_lo = cpu_read(cpu, cpu->pc);
  cpu->pc++;
  uint8_t ptr_hi = cpu_read(cpu, cpu->pc);
  cpu->pc++;

  uint16_t ptr = (ptr_hi << 8) | ptr_lo;

  if (ptr_lo == 0xFF)
    cpu->addr_abs = (cpu_read(cpu, ptr & 0xFF00) << 8) | cpu_read(cpu, ptr);
  else
    cpu->addr_abs = (cpu_read(cpu, ptr+1) << 8) | cpu_read(cpu, ptr);

  return 0;
}

uint8_t addr_izx(CPU* cpu)
{
  uint16_t addr = cpu_read(cpu, cpu->pc);
  cpu->pc++;

  uint16_t lo = cpu_read(cpu, (addr + cpu->x) & 0xFF);
  uint16_t hi = cpu_read(cpu, (addr + cpu->x + 1) & 0xFF);

  cpu->addr_abs = (hi << 8) | lo;

  return 0;
}

uint8_t addr_izy(CPU* cpu)
{
  uint16_t addr = cpu_read(cpu, cpu->pc);
	cpu->pc++;

	uint16_t lo = cpu_read(cpu, addr & 0x00FF);
	uint16_t hi = cpu_read(cpu, (addr + 1) & 0x00FF);

	cpu->addr_abs = (hi << 8) | lo;
	cpu->addr_abs += cpu->y;
	
	if ((cpu->addr_abs & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}

uint8_t addr_rel(CPU* cpu)
{
  cpu->addr_rel = cpu_read(cpu, cpu->pc);
  cpu->pc++;

  if (cpu->addr_rel & 0x80)
    cpu->addr_rel |= 0xFF00;

  return 0;
}

uint8_t addr_zp0(CPU* cpu)
{
  cpu->addr_abs = cpu_read(cpu, cpu->pc);
  cpu->pc++;
  cpu->addr_abs &= 0x00FF;
  return 0;
}

uint8_t addr_zpx(CPU* cpu)
{
  cpu->addr_abs = cpu_read(cpu, cpu->pc) + cpu->x;
  cpu->pc++;
  cpu->addr_abs &= 0x00FF;
  return 0;
}

uint8_t addr_zpy(CPU* cpu)
{
  cpu->addr_abs = cpu_read(cpu, cpu->pc) + cpu->y;
  cpu->pc++;
  cpu->addr_abs &= 0x00FF;
  return 0;
}

uint8_t handle_ADC(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = cpu->a + cpu->fetched + cpu_flag_get(cpu, C);

  cpu_flag_set(cpu, C, cpu->temp > 0xFF);
  cpu_flag_set(cpu, Z, (cpu->temp & 0x00FF) == 0);
  cpu_flag_set(cpu, V, (~(cpu->a ^ cpu->fetched) & (cpu->a ^ cpu->temp)) & 0x80);
  cpu_flag_set(cpu, N, cpu->fetched & 0x80);

  cpu->a = cpu->temp & 0x00FF;

  return 1;
}

uint8_t handle_SBC(CPU* cpu)
{
  cpu_fetch(cpu);

  uint16_t value = cpu->fetched ^ 0x00FF;

  cpu->temp = cpu->a + value + cpu_flag_get(cpu, C);

  cpu_flag_set(cpu, C, cpu->temp > 0xFF);
  cpu_flag_set(cpu, Z, (cpu->temp & 0x00FF) == 0);
  cpu_flag_set(cpu, V, (cpu->temp ^ cpu->a) & (cpu->temp ^ value) & 0x80);
  cpu_flag_set(cpu, N, cpu->temp & 0x80);

  cpu->a = cpu->temp & 0x00FF;

  return 1;

  cpu_flag_set(cpu, C, cpu->temp > 0xFF);
  cpu_flag_set(cpu, Z, (cpu->temp & 0x00FF) == 0);
  cpu_flag_set(cpu, V, (~(cpu->a ^ cpu->fetched) & (cpu->a ^ cpu->temp)) & 0x80);
  cpu_flag_set(cpu, N, cpu->fetched & 0x80);

  cpu->a = cpu->temp & 0x00FF;

  return 1;

}

uint8_t handle_AND(CPU* cpu)
{
  cpu_fetch(cpu);
  cpu->a &= cpu->fetched;
  cpu_flag_set(cpu, Z, cpu->a == 0x00);
  cpu_flag_set(cpu, N, cpu->a & 0x80);
}

uint8_t handle_ASL(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = (cpu->fetched << 1);
  cpu_flag_set(cpu, Z, cpu->temp == 0);
  cpu_flag_set(cpu, N, cpu->temp & 0x80);
  cpu_flag_set(cpu, C, cpu->temp > 0xFF);

  if (cpu->lookup[cpu->opcode].addrmode == &addr_imp)
    cpu->a = cpu->temp & 0x00FF;
  else
    cpu_write(cpu, cpu->addr_abs, cpu->temp & 0x00FF);

  return 0;
}

uint8_t handle_BCC(CPU* cpu)
{
  if (cpu_flag_get(cpu, C) == 0)
  {
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;

    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
      cpu->cycles++;

    cpu->pc = cpu->addr_abs;
  }

  return 0;
}

uint8_t handle_BCS(CPU* cpu)
{
  if (cpu_flag_get(cpu, C) == 1)
  {
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;

    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
      cpu->cycles++;

    cpu->pc = cpu->addr_abs;
  }

  return 0;
}

uint8_t handle_BEQ(CPU* cpu)
{
  if (cpu_flag_get(cpu, Z) == 1)
  {
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;

    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
      cpu->cycles++;

    cpu->pc = cpu->addr_abs;
  }

  return 0;
}

uint8_t handle_BIT(CPU* cpu)
{
  cpu_fetch(cpu);
  cpu->temp = cpu->a & cpu->fetched;

  cpu_flag_set(cpu, Z, (cpu->temp & 0xFF) == 0x00);
  cpu_flag_set(cpu, N, cpu->fetched & (1 << 7));
  cpu_flag_set(cpu, V, cpu->fetched & (1 << 6));

  return 0;
}

uint8_t handle_BMI(CPU* cpu)
{
  if (cpu_flag_get(cpu, N) == 1)
  {
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;

    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
      cpu->cycles++;

    cpu->pc = cpu->addr_abs;
  }

  return 0;
}

uint8_t handle_BNE(CPU* cpu)
{
  if (cpu_flag_get(cpu, Z) == 0)
  {
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;

    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
      cpu->cycles++;

    cpu->pc = cpu->addr_abs;
  }

  return 0;
}

uint8_t handle_BPL(CPU* cpu)
{
  if (cpu_flag_get(cpu, N) == 0)
  {
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;

    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
      cpu->cycles++;

    cpu->pc = cpu->addr_abs;
  }

  return 0;
}

uint8_t handle_BRK(CPU* cpu)
{
  cpu->pc++;

  cpu_flag_set(cpu, I, true);

  cpu_write(cpu, 0x100 + cpu->sp, (cpu->pc >> 8) & 0xFF);
  cpu->sp--;
  cpu_write(cpu, 0x100 + cpu->sp, cpu->pc & 0xFF);
  cpu->sp--;

  cpu_flag_set(cpu, B, true);
  
  cpu_write(cpu, 0x100 + cpu->sp, cpu->st);
  cpu->sp--;

  cpu->pc = cpu_read(cpu, 0xFFFE) | (cpu_read(cpu, 0xFFFF) << 8);
  return 0;
}

uint8_t handle_BVC(CPU* cpu)
{
  if (cpu_flag_get(cpu, V) == 0)
  {
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;

    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
      cpu->cycles++;

    cpu->pc = cpu->addr_abs;
  }

  return 0;
}
uint8_t handle_BVS(CPU* cpu)
{
  if (cpu_flag_get(cpu, V) == 1)
  {
    cpu->cycles++;
    cpu->addr_abs = cpu->pc + cpu->addr_rel;

    if ((cpu->addr_abs & 0xFF00) != (cpu->pc & 0xFF00))
      cpu->cycles++;

    cpu->pc = cpu->addr_abs;
  }

  return 0;
}

uint8_t handle_CLC(CPU* cpu)
{
  cpu_flag_set(cpu, C, false);
  return 0;
}

uint8_t handle_CLD(CPU* cpu)
{
  cpu_flag_set(cpu, D, false);
  return 0;
}

uint8_t handle_CLI(CPU* cpu)
{
  cpu_flag_set(cpu, I, false);
  return 0;
}

uint8_t handle_CLV(CPU* cpu)
{
  cpu_flag_set(cpu, V, false);
  return 0;
}

uint8_t handle_CMP(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = cpu->a - cpu->fetched;

  cpu_flag_set(cpu, Z, (cpu->temp & 0x00FF) == 0x00);
  cpu_flag_set(cpu, N, (cpu->temp & 0x80));

  return 1;
}

uint8_t handle_CPX(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = cpu->x - cpu->fetched;

  cpu_flag_set(cpu, Z, (cpu->temp & 0x00FF) == 0x00);
  cpu_flag_set(cpu, N, (cpu->temp & 0x80));

  return 1;
}
uint8_t handle_CPY(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = cpu->y - cpu->fetched;

  cpu_flag_set(cpu, Z, (cpu->temp & 0x00FF) == 0x00);
  cpu_flag_set(cpu, N, (cpu->temp & 0x80));

  return 1;
}

uint8_t handle_DEC(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = cpu->fetched - 1;

  cpu_flag_set(cpu, Z, cpu->temp == 0x00);
  cpu_flag_set(cpu, N, cpu->temp & 0x80);

  cpu_write(cpu, cpu->addr_abs, cpu->temp & 0xFF);
  return 0;
}

uint8_t handle_DEX(CPU* cpu)
{
  cpu->x--;
  cpu_flag_set(cpu, Z, cpu->x == 0);
  cpu_flag_set(cpu, N, cpu->x & 0x80);
  return 0;
}

uint8_t handle_DEY(CPU* cpu)
{
  cpu->y--;
  cpu_flag_set(cpu, Z, cpu->y == 0);
  cpu_flag_set(cpu, N, cpu->y & 0x80);
  return 0;
}

uint8_t handle_EOR(CPU* cpu)
{
  cpu_fetch(cpu);
  cpu->a ^= cpu->fetched;
  cpu_flag_set(cpu, Z, cpu->a == 0);
  cpu_flag_set(cpu, N, cpu->a & 0x80);

  return 1;
}

uint8_t handle_INC(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = cpu->fetched + 1;

  cpu_flag_set(cpu, Z, cpu->temp == 0x00);
  cpu_flag_set(cpu, N, cpu->temp & 0x80);

  cpu_write(cpu, cpu->addr_abs, cpu->temp & 0xFF);
  return 0;
}

uint8_t handle_INX(CPU* cpu)
{
  cpu->x++;
  cpu_flag_set(cpu, Z, cpu->x == 0);
  cpu_flag_set(cpu, N, cpu->x & 0x80);
  return 0;
}

uint8_t handle_INY(CPU* cpu)
{
  cpu->y++;
  cpu_flag_set(cpu, Z, cpu->y == 0);
  cpu_flag_set(cpu, N, cpu->y & 0x80);
  return 0;
}

uint8_t handle_JMP(CPU* cpu)
{
  cpu->pc = cpu->addr_abs; 

  return 0;
}

uint8_t handle_JSR(CPU* cpu)
{
  cpu->pc--;

  uint8_t lo = cpu->pc & 0xFF;
  uint8_t hi = (cpu->pc >> 8) & 0xFF;

  cpu_write(cpu, 0x100 + cpu->sp, hi);
  cpu->sp--;

  cpu_write(cpu, 0x100 + cpu->sp, lo);
  cpu->sp--;

  cpu->pc = cpu->addr_abs;

  return 0;
}

uint8_t handle_LDA(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->a = cpu->fetched;
  cpu_flag_set(cpu, Z, cpu->a == 0x00);
  cpu_flag_set(cpu, N, cpu->a & 0x80);
  return 1;
}

uint8_t handle_LDX(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->x = cpu->fetched;
  cpu_flag_set(cpu, Z, cpu->x == 0x00);
  cpu_flag_set(cpu, N, cpu->x & 0x80);
  return 1;
}

uint8_t handle_LDY(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->y = cpu->fetched;
  cpu_flag_set(cpu, Z, cpu->y == 0x00);
  cpu_flag_set(cpu, N, cpu->y & 0x80);
  return 1;
}

uint8_t handle_LSR(CPU* cpu)
{
  cpu_fetch(cpu);
  cpu_flag_set(cpu, C, cpu->fetched & 0x1);

  cpu->temp = cpu->fetched >> 1;
  cpu_flag_set(cpu, Z, cpu->temp == 0x00);
  cpu_flag_set(cpu, N, cpu->temp & 0x80);

  if (cpu->lookup[cpu->opcode].addrmode == &addr_imp)
    cpu->a = cpu->temp & 0xFF;
  else
    cpu_write(cpu, cpu->addr_abs, cpu->temp & 0xFF);
  
  return 0;
}

uint8_t handle_NOP(CPU* cpu)
{
  return 0;
}

uint8_t handle_ORA(CPU* cpu)
{
  cpu_fetch(cpu);
  cpu->a |= cpu->fetched;

  cpu_flag_set(cpu, Z, cpu->a == 0);
  cpu_flag_set(cpu, N, cpu->a & 0x80);

  return 0;
}

uint8_t handle_PHA(CPU* cpu)
{
  cpu_write(cpu, 0x100 + cpu->sp, cpu->a);
  cpu->sp--;

  return 0;
}

uint8_t handle_PHP(CPU* cpu)
{
  cpu_write(cpu, 0x100 + cpu->sp, cpu->st);
  cpu->sp--;

  return 0;
}

uint8_t handle_PLA(CPU* cpu)
{
  cpu->sp++;
  cpu->a = cpu_read(cpu, 0x100 + cpu->sp);
  cpu_flag_set(cpu, Z, cpu->a == 0x00);
  cpu_flag_set(cpu, N, cpu->a & 0x80);

  return 0;
}

uint8_t handle_PLP(CPU* cpu)
{
  cpu->sp++;
  cpu->st = cpu_read(cpu, 0x100 + cpu->sp);
  cpu_flag_set(cpu, Z, cpu->st == 0x00);
  cpu_flag_set(cpu, N, cpu->st & 0x80);

  return 0;
}

uint8_t handle_ROL(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = (cpu->fetched << 1) | cpu_flag_get(cpu, C);
  cpu_flag_set(cpu, C, cpu->temp & 0xFF00);
  cpu_flag_set(cpu, Z, cpu->temp == 0x00);
  cpu_flag_set(cpu, N, cpu->temp & 0x80);

  if (cpu->lookup[cpu->opcode].addrmode == &addr_imp)
    cpu->a = cpu->fetched & 0xFF;
  else
    cpu_write(cpu, cpu->addr_abs, cpu->fetched & 0xFF);

  return 0;
}

uint8_t handle_ROR(CPU* cpu)
{
  cpu_fetch(cpu);

  cpu->temp = (cpu->fetched >> 1) | (cpu_flag_get(cpu, C) << 7);
  cpu_flag_set(cpu, C, cpu->temp & 0x01);
  cpu_flag_set(cpu, Z, cpu->temp == 0x00);
  cpu_flag_set(cpu, N, cpu->temp & 0x80);

  if (cpu->lookup[cpu->opcode].addrmode == &addr_imp)
    cpu->a = cpu->fetched & 0xFF;
  else
    cpu_write(cpu, cpu->addr_abs, cpu->fetched & 0xFF);

  return 0;
}

uint8_t handle_RTI(CPU* cpu)
{
  cpu->sp++;
  cpu->st = cpu_read(cpu, 0x100 + cpu->sp);

  cpu->st &= ~B;
  cpu->st &= ~U;

  cpu->sp++;
  cpu->pc = cpu_read(cpu, 0x100 + cpu->sp);

  cpu->sp++;
  cpu->pc |= cpu_read(cpu, 0x100 + cpu->sp) << 8;

  cpu_flag_set(cpu, I, false);

  return 0;

}

uint8_t handle_RTS(CPU* cpu)
{
  cpu->sp++;
  uint8_t lo = cpu_read(cpu, 0x100 + cpu->sp);
  cpu->sp++;
  uint8_t hi = cpu_read(cpu, 0x100 + cpu->sp);

  cpu->pc = (hi << 8) | lo;
  cpu->pc++;

  return 0;
}

uint8_t handle_SEC(CPU* cpu)
{
  cpu_flag_set(cpu, C, true);
  return 0;
}

uint8_t handle_SED(CPU* cpu)
{
  cpu_flag_set(cpu, D, true);
  return 0;
}

uint8_t handle_SEI(CPU* cpu)
{
  cpu_flag_set(cpu, I, true);
  return 0;
}

uint8_t handle_STA(CPU* cpu)
{
  cpu_write(cpu, cpu->addr_abs, cpu->a);
  return 0;
}

uint8_t handle_STX(CPU* cpu)
{
  cpu_write(cpu, cpu->addr_abs, cpu->x);
  return 0;
}

uint8_t handle_STY(CPU* cpu)
{
  cpu_write(cpu, cpu->addr_abs, cpu->y);
  return 0;
}

uint8_t handle_TAX(CPU* cpu)
{
  cpu->x = cpu->a;
  cpu_flag_set(cpu, Z, cpu->x == 0);
  cpu_flag_set(cpu, N, cpu->x & 0x80);
  return 0;
}

uint8_t handle_TAY(CPU* cpu)
{
  cpu->y = cpu->a;
  cpu_flag_set(cpu, Z, cpu->y == 0);
  cpu_flag_set(cpu, N, cpu->y & 0x80);
  return 0;
}

uint8_t handle_TSX(CPU* cpu)
{
  cpu->x = cpu->sp;
  cpu_flag_set(cpu, Z, cpu->x == 0);
  cpu_flag_set(cpu, N, cpu->x & 0x80);
  return 0;
}

uint8_t handle_TXA(CPU* cpu)
{
  cpu->a = cpu->x;
  cpu_flag_set(cpu, Z, cpu->a == 0);
  cpu_flag_set(cpu, N, cpu->a & 0x80);
  return 0;
}

uint8_t handle_TXS(CPU* cpu)
{
  cpu->sp = cpu->x;
  return 0;
}

uint8_t handle_TYA(CPU* cpu)
{
  cpu->a = cpu->y;
  cpu_flag_set(cpu, Z, cpu->a == 0);
  cpu_flag_set(cpu, N, cpu->a & 0x80);
  return 0;
}

uint8_t handle_XXX(CPU* cpu)
{
  return 0;
}
