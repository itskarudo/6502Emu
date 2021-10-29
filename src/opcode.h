#ifndef __OPCODE_H__
#define __OPCODE_H__

#include <stdint.h>

typedef struct CPU CPU;

typedef struct Opcode {
  char* name;
  uint8_t (*operate)(CPU* cpu);
  uint8_t (*addrmode)(CPU* cpu);
  uint8_t cycles;
} Opcode;

uint8_t addr_abs(CPU* cpu);
uint8_t addr_abx(CPU* cpu);
uint8_t addr_aby(CPU* cpu);
uint8_t addr_imm(CPU* cpu);
uint8_t addr_imp(CPU* cpu);
uint8_t addr_ind(CPU* cpu);
uint8_t addr_izx(CPU* cpu);
uint8_t addr_izy(CPU* cpu);
uint8_t addr_rel(CPU* cpu);
uint8_t addr_zp0(CPU* cpu);
uint8_t addr_zpx(CPU* cpu);
uint8_t addr_zpy(CPU* cpu);

uint8_t handle_ADC(CPU* cpu);
uint8_t handle_AND(CPU* cpu);
uint8_t handle_ASL(CPU* cpu);
uint8_t handle_BCC(CPU* cpu);
uint8_t handle_BCS(CPU* cpu);
uint8_t handle_BEQ(CPU* cpu);
uint8_t handle_BIT(CPU* cpu);
uint8_t handle_BMI(CPU* cpu);
uint8_t handle_BNE(CPU* cpu);
uint8_t handle_BPL(CPU* cpu);
uint8_t handle_BRK(CPU* cpu);
uint8_t handle_BVC(CPU* cpu);
uint8_t handle_BVS(CPU* cpu);
uint8_t handle_CLC(CPU* cpu);
uint8_t handle_CLD(CPU* cpu);
uint8_t handle_CLI(CPU* cpu);
uint8_t handle_CLV(CPU* cpu);
uint8_t handle_CMP(CPU* cpu);
uint8_t handle_CPX(CPU* cpu);
uint8_t handle_CPY(CPU* cpu);
uint8_t handle_DEC(CPU* cpu);
uint8_t handle_DEX(CPU* cpu);
uint8_t handle_DEY(CPU* cpu);
uint8_t handle_EOR(CPU* cpu);
uint8_t handle_INC(CPU* cpu);
uint8_t handle_INY(CPU* cpu);
uint8_t handle_INX(CPU* cpu);
uint8_t handle_JMP(CPU* cpu);
uint8_t handle_JSR(CPU* cpu);
uint8_t handle_LDA(CPU* cpu);
uint8_t handle_LDX(CPU* cpu);
uint8_t handle_LDY(CPU* cpu);
uint8_t handle_LSR(CPU* cpu);
uint8_t handle_NOP(CPU* cpu);
uint8_t handle_ORA(CPU* cpu);
uint8_t handle_PHA(CPU* cpu);
uint8_t handle_PHP(CPU* cpu);
uint8_t handle_PLA(CPU* cpu);
uint8_t handle_PLP(CPU* cpu);
uint8_t handle_ROL(CPU* cpu);
uint8_t handle_ROR(CPU* cpu);
uint8_t handle_RTI(CPU* cpu);
uint8_t handle_RTS(CPU* cpu);
uint8_t handle_SBC(CPU* cpu);
uint8_t handle_SEC(CPU* cpu);
uint8_t handle_SED(CPU* cpu);
uint8_t handle_SEI(CPU* cpu);
uint8_t handle_STA(CPU* cpu);
uint8_t handle_STX(CPU* cpu);
uint8_t handle_STY(CPU* cpu);
uint8_t handle_TAX(CPU* cpu);
uint8_t handle_TAY(CPU* cpu);
uint8_t handle_TSX(CPU* cpu);
uint8_t handle_TXA(CPU* cpu);
uint8_t handle_TXS(CPU* cpu);
uint8_t handle_TYA(CPU* cpu);
uint8_t handle_XXX(CPU* cpu);


#endif /* __OPCODE_H__ */
