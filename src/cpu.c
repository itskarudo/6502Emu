#include "cpu.h"
#include "bus.h"
#include "opcode.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define OPCODES_NUM 0x100

CPU* cpu_create(void)
{
  CPU* cpu = malloc(sizeof(CPU));
  if (cpu == NULL)
  {
    fprintf(stderr, "Failed to allocate memory, abort.\n");
    exit(-1);
  }

  cpu->lookup = malloc(sizeof(Opcode) * OPCODES_NUM);
  if (cpu->lookup == NULL)
  {
    fprintf(stderr, "Failed to allocate memory, abort.\n");
    exit(-1);
  }

  cpu->bus = NULL;

  cpu->a   = 0x00;
  cpu->x   = 0x00;
  cpu->y   = 0x00;
  cpu->sp  = 0xFD;
  cpu->pc  = 0xFFFC;
  cpu->st  = 0x00;

  cpu->cycles = 0;

  cpu->opcode = 0;

  cpu->lookup[0] = (Opcode){ "BRK", &handle_BRK, &addr_imm, 7 };
  cpu->lookup[1] = (Opcode){ "ORA", &handle_ORA, &addr_izx, 6 };
  cpu->lookup[2] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[3] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[4] = (Opcode){ "???", &handle_XXX, &addr_imp, 3 };
  cpu->lookup[5] = (Opcode){ "ORA", &handle_ORA, &addr_zp0, 3 };
  cpu->lookup[6] = (Opcode){ "ASL", &handle_ASL, &addr_zp0, 5 };
  cpu->lookup[7] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[8] = (Opcode){ "PHP", &handle_PHP, &addr_imp, 3 };
  cpu->lookup[9] = (Opcode){ "ORA", &handle_ORA, &addr_imm, 2 };
  cpu->lookup[10] = (Opcode){ "ASL", &handle_ASL, &addr_imp, 2 };
  cpu->lookup[11] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[12] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[13] = (Opcode){ "ORA", &handle_ORA, &addr_abs, 4 };
  cpu->lookup[14] = (Opcode){ "ASL", &handle_ASL, &addr_abs, 6 };
  cpu->lookup[15] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[16] = (Opcode){ "BPL", &handle_BPL, &addr_rel, 7 };
  cpu->lookup[17] = (Opcode){ "ORA", &handle_ORA, &addr_izy, 5 };
  cpu->lookup[18] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[19] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[20] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[21] = (Opcode){ "ORA", &handle_ORA, &addr_zpx, 4 };
  cpu->lookup[22] = (Opcode){ "ASL", &handle_ASL, &addr_zpx, 6 };
  cpu->lookup[23] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[24] = (Opcode){ "CLC", &handle_CLC, &addr_imp, 2 };
  cpu->lookup[25] = (Opcode){ "ORA", &handle_ORA, &addr_aby, 4 };
  cpu->lookup[26] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[27] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[28] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[29] = (Opcode){ "ORA", &handle_ORA, &addr_abx, 4 };
  cpu->lookup[30] = (Opcode){ "ASL", &handle_ASL, &addr_abx, 7 };
  cpu->lookup[31] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[32] = (Opcode){ "JSR", &handle_JSR, &addr_abs, 7 };
  cpu->lookup[33] = (Opcode){ "AND", &handle_AND, &addr_izx, 6 };
  cpu->lookup[34] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[35] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[36] = (Opcode){ "BIT", &handle_BIT, &addr_zp0, 3 };
  cpu->lookup[37] = (Opcode){ "AND", &handle_AND, &addr_zp0, 3 };
  cpu->lookup[38] = (Opcode){ "ROL", &handle_ROL, &addr_zp0, 5 };
  cpu->lookup[39] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[40] = (Opcode){ "PLP", &handle_PLP, &addr_imp, 4 };
  cpu->lookup[41] = (Opcode){ "AND", &handle_AND, &addr_imm, 2 };
  cpu->lookup[42] = (Opcode){ "ROL", &handle_ROL, &addr_imp, 2 };
  cpu->lookup[43] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[44] = (Opcode){ "BIT", &handle_BIT, &addr_abs, 4 };
  cpu->lookup[45] = (Opcode){ "AND", &handle_AND, &addr_abs, 4 };
  cpu->lookup[46] = (Opcode){ "ROL", &handle_ROL, &addr_abs, 6 };
  cpu->lookup[47] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[48] = (Opcode){ "BMI", &handle_BMI, &addr_rel, 7 };
  cpu->lookup[49] = (Opcode){ "AND", &handle_AND, &addr_izy, 5 };
  cpu->lookup[50] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[51] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[52] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[53] = (Opcode){ "AND", &handle_AND, &addr_zpx, 4 };
  cpu->lookup[54] = (Opcode){ "ROL", &handle_ROL, &addr_zpx, 6 };
  cpu->lookup[55] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[56] = (Opcode){ "SEC", &handle_SEC, &addr_imp, 2 };
  cpu->lookup[57] = (Opcode){ "AND", &handle_AND, &addr_aby, 4 };
  cpu->lookup[58] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[59] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[60] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[61] = (Opcode){ "AND", &handle_AND, &addr_abx, 4 };
  cpu->lookup[62] = (Opcode){ "ROL", &handle_ROL, &addr_abx, 7 };
  cpu->lookup[63] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[64] = (Opcode){ "RTI", &handle_RTI, &addr_imp, 7 };
  cpu->lookup[65] = (Opcode){ "EOR", &handle_EOR, &addr_izx, 6 };
  cpu->lookup[66] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[67] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[68] = (Opcode){ "???", &handle_XXX, &addr_imp, 3 };
  cpu->lookup[69] = (Opcode){ "EOR", &handle_EOR, &addr_zp0, 3 };
  cpu->lookup[70] = (Opcode){ "LSR", &handle_LSR, &addr_zp0, 5 };
  cpu->lookup[71] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[72] = (Opcode){ "PHA", &handle_PHA, &addr_imp, 3 };
  cpu->lookup[73] = (Opcode){ "EOR", &handle_EOR, &addr_imm, 2 };
  cpu->lookup[74] = (Opcode){ "LSR", &handle_LSR, &addr_imp, 2 };
  cpu->lookup[75] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[76] = (Opcode){ "JMP", &handle_JMP, &addr_abs, 3 };
  cpu->lookup[77] = (Opcode){ "EOR", &handle_EOR, &addr_abs, 4 };
  cpu->lookup[78] = (Opcode){ "LSR", &handle_LSR, &addr_abs, 6 };
  cpu->lookup[79] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[80] = (Opcode){ "BVC", &handle_BVC, &addr_rel, 7 };
  cpu->lookup[81] = (Opcode){ "EOR", &handle_EOR, &addr_izy, 5 };
  cpu->lookup[82] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[83] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[84] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[85] = (Opcode){ "EOR", &handle_EOR, &addr_zpx, 4 };
  cpu->lookup[86] = (Opcode){ "LSR", &handle_LSR, &addr_zpx, 6 };
  cpu->lookup[87] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[88] = (Opcode){ "CLI", &handle_CLI, &addr_imp, 2 };
  cpu->lookup[89] = (Opcode){ "EOR", &handle_EOR, &addr_aby, 4 };
  cpu->lookup[90] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[91] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[92] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[93] = (Opcode){ "EOR", &handle_EOR, &addr_abx, 4 };
  cpu->lookup[94] = (Opcode){ "LSR", &handle_LSR, &addr_abx, 7 };
  cpu->lookup[95] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[96] = (Opcode){ "RTS", &handle_RTS, &addr_imp, 7 };
  cpu->lookup[97] = (Opcode){ "ADC", &handle_ADC, &addr_izx, 6 };
  cpu->lookup[98] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[99] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[100] = (Opcode){ "???", &handle_XXX, &addr_imp, 3 };
  cpu->lookup[101] = (Opcode){ "ADC", &handle_ADC, &addr_zp0, 3 };
  cpu->lookup[102] = (Opcode){ "ROR", &handle_ROR, &addr_zp0, 5 };
  cpu->lookup[103] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[104] = (Opcode){ "PLA", &handle_PLA, &addr_imp, 4 };
  cpu->lookup[105] = (Opcode){ "ADC", &handle_ADC, &addr_imm, 2 };
  cpu->lookup[106] = (Opcode){ "ROR", &handle_ROR, &addr_imp, 2 };
  cpu->lookup[107] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[108] = (Opcode){ "JMP", &handle_JMP, &addr_ind, 5 };
  cpu->lookup[109] = (Opcode){ "ADC", &handle_ADC, &addr_abs, 4 };
  cpu->lookup[110] = (Opcode){ "ROR", &handle_ROR, &addr_abs, 6 };
  cpu->lookup[111] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[112] = (Opcode){ "BVS", &handle_BVS, &addr_rel, 7 };
  cpu->lookup[113] = (Opcode){ "ADC", &handle_ADC, &addr_izy, 5 };
  cpu->lookup[114] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[115] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[116] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[117] = (Opcode){ "ADC", &handle_ADC, &addr_zpx, 4 };
  cpu->lookup[118] = (Opcode){ "ROR", &handle_ROR, &addr_zpx, 6 };
  cpu->lookup[119] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[120] = (Opcode){ "SEI", &handle_SEI, &addr_imp, 2 };
  cpu->lookup[121] = (Opcode){ "ADC", &handle_ADC, &addr_aby, 4 };
  cpu->lookup[122] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[123] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[124] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[125] = (Opcode){ "ADC", &handle_ADC, &addr_abx, 4 };
  cpu->lookup[126] = (Opcode){ "ROR", &handle_ROR, &addr_abx, 7 };
  cpu->lookup[127] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[128] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[129] = (Opcode){ "STA", &handle_STA, &addr_izx, 6 };
  cpu->lookup[130] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[131] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[132] = (Opcode){ "STY", &handle_STY, &addr_zp0, 3 };
  cpu->lookup[133] = (Opcode){ "STA", &handle_STA, &addr_zp0, 3 };
  cpu->lookup[134] = (Opcode){ "STX", &handle_STX, &addr_zp0, 3 };
  cpu->lookup[135] = (Opcode){ "???", &handle_XXX, &addr_imp, 3 };
  cpu->lookup[136] = (Opcode){ "DEY", &handle_DEY, &addr_imp, 2 };
  cpu->lookup[137] = (Opcode){ "???", &handle_XXX, &addr_imm, 2 };
  cpu->lookup[138] = (Opcode){ "TXA", &handle_TXA, &addr_imp, 2 };
  cpu->lookup[139] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[140] = (Opcode){ "STY", &handle_STY, &addr_abs, 4 };
  cpu->lookup[141] = (Opcode){ "STA", &handle_STA, &addr_abs, 4 };
  cpu->lookup[142] = (Opcode){ "STX", &handle_STX, &addr_abs, 4 };
  cpu->lookup[143] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[144] = (Opcode){ "BCC", &handle_BCC, &addr_rel, 7 };
  cpu->lookup[145] = (Opcode){ "STA", &handle_STA, &addr_izy, 6 };
  cpu->lookup[146] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[147] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[148] = (Opcode){ "STY", &handle_STY, &addr_zpx, 4 };
  cpu->lookup[149] = (Opcode){ "STA", &handle_STA, &addr_zpx, 4 };
  cpu->lookup[150] = (Opcode){ "STX", &handle_STX, &addr_zpy, 4 };
  cpu->lookup[151] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[152] = (Opcode){ "TYA", &handle_TYA, &addr_imp, 2 };
  cpu->lookup[153] = (Opcode){ "STA", &handle_STA, &addr_aby, 5 };
  cpu->lookup[154] = (Opcode){ "TXS", &handle_TXS, &addr_imp, 2 };
  cpu->lookup[155] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[156] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[157] = (Opcode){ "STA", &handle_STA, &addr_abx, 5 };
  cpu->lookup[158] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[159] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[160] = (Opcode){ "LDY", &handle_LDY, &addr_imm, 7 };
  cpu->lookup[161] = (Opcode){ "LDA", &handle_LDA, &addr_izx, 5 };
  cpu->lookup[162] = (Opcode){ "LDX", &handle_LDX, &addr_imm, 2 };
  cpu->lookup[163] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[164] = (Opcode){ "LDY", &handle_LDY, &addr_zp0, 3 };
  cpu->lookup[165] = (Opcode){ "LDA", &handle_LDA, &addr_zp0, 3 };
  cpu->lookup[166] = (Opcode){ "LDX", &handle_LDX, &addr_zp0, 3 };
  cpu->lookup[167] = (Opcode){ "???", &handle_XXX, &addr_imp, 3 };
  cpu->lookup[168] = (Opcode){ "TAY", &handle_TAY, &addr_imp, 2 };
  cpu->lookup[169] = (Opcode){ "LDA", &handle_LDA, &addr_imm, 2 };
  cpu->lookup[170] = (Opcode){ "TAX", &handle_TAX, &addr_imp, 2 };
  cpu->lookup[171] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[172] = (Opcode){ "LDY", &handle_LDY, &addr_abs, 4 };
  cpu->lookup[173] = (Opcode){ "LDA", &handle_LDA, &addr_abs, 4 };
  cpu->lookup[174] = (Opcode){ "LDX", &handle_LDX, &addr_abs, 4 };
  cpu->lookup[175] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[176] = (Opcode){ "BCS", &handle_BCS, &addr_rel, 7 };
  cpu->lookup[177] = (Opcode){ "LDA", &handle_LDA, &addr_izy, 6 };
  cpu->lookup[178] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[179] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[180] = (Opcode){ "LDY", &handle_LDY, &addr_zpx, 4 };
  cpu->lookup[181] = (Opcode){ "LDA", &handle_LDA, &addr_zpx, 4 };
  cpu->lookup[182] = (Opcode){ "LDX", &handle_LDX, &addr_zpy, 4 };
  cpu->lookup[183] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[184] = (Opcode){ "CLV", &handle_CLV, &addr_imp, 2 };
  cpu->lookup[185] = (Opcode){ "LDA", &handle_LDA, &addr_aby, 4 };
  cpu->lookup[186] = (Opcode){ "TSX", &handle_TSX, &addr_imp, 2 };
  cpu->lookup[187] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[188] = (Opcode){ "LDY", &handle_LDY, &addr_abx, 4 };
  cpu->lookup[189] = (Opcode){ "LDA", &handle_LDA, &addr_abx, 4 };
  cpu->lookup[190] = (Opcode){ "LDX", &handle_LDX, &addr_aby, 4 };
  cpu->lookup[191] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[192] = (Opcode){ "CPY", &handle_CPY, &addr_imm, 7 };
  cpu->lookup[193] = (Opcode){ "CMP", &handle_CMP, &addr_izx, 5 };
  cpu->lookup[194] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[195] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[196] = (Opcode){ "CPY", &handle_CPY, &addr_zp0, 3 };
  cpu->lookup[197] = (Opcode){ "CMP", &handle_CMP, &addr_zp0, 3 };
  cpu->lookup[198] = (Opcode){ "DEC", &handle_DEC, &addr_zp0, 5 };
  cpu->lookup[199] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[200] = (Opcode){ "INY", &handle_INY, &addr_imp, 2 };
  cpu->lookup[201] = (Opcode){ "CMP", &handle_CMP, &addr_imm, 2 };
  cpu->lookup[202] = (Opcode){ "DEX", &handle_DEX, &addr_imp, 2 };
  cpu->lookup[203] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[204] = (Opcode){ "CPY", &handle_CPY, &addr_abs, 4 };
  cpu->lookup[205] = (Opcode){ "CMP", &handle_CMP, &addr_abs, 4 };
  cpu->lookup[206] = (Opcode){ "DEC", &handle_DEC, &addr_abs, 6 };
  cpu->lookup[207] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[208] = (Opcode){ "BNE", &handle_BNE, &addr_rel, 7 };
  cpu->lookup[209] = (Opcode){ "CMP", &handle_CMP, &addr_izy, 6 };
  cpu->lookup[210] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[211] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[212] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[213] = (Opcode){ "CMP", &handle_CMP, &addr_zpx, 4 };
  cpu->lookup[214] = (Opcode){ "DEC", &handle_DEC, &addr_zpx, 6 };
  cpu->lookup[215] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[216] = (Opcode){ "CLD", &handle_CLD, &addr_imp, 2 };
  cpu->lookup[217] = (Opcode){ "CMP", &handle_CMP, &addr_aby, 4 };
  cpu->lookup[218] = (Opcode){ "NOP", &handle_NOP, &addr_imp, 2 };
  cpu->lookup[219] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[220] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[221] = (Opcode){ "CMP", &handle_CMP, &addr_abx, 4 };
  cpu->lookup[222] = (Opcode){ "DEC", &handle_DEC, &addr_abx, 7 };
  cpu->lookup[223] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[224] = (Opcode){ "CPX", &handle_CPX, &addr_imm, 7 };
  cpu->lookup[225] = (Opcode){ "SBC", &handle_SBC, &addr_izx, 5 };
  cpu->lookup[226] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[227] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[228] = (Opcode){ "CPX", &handle_CPX, &addr_zp0, 3 };
  cpu->lookup[229] = (Opcode){ "SBC", &handle_SBC, &addr_zp0, 3 };
  cpu->lookup[230] = (Opcode){ "INC", &handle_INC, &addr_zp0, 5 };
  cpu->lookup[231] = (Opcode){ "???", &handle_XXX, &addr_imp, 5 };
  cpu->lookup[232] = (Opcode){ "INX", &handle_INX, &addr_imp, 2 };
  cpu->lookup[233] = (Opcode){ "SBC", &handle_SBC, &addr_imm, 2 };
  cpu->lookup[234] = (Opcode){ "NOP", &handle_NOP, &addr_imp, 2 };
  cpu->lookup[235] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[236] = (Opcode){ "CPX", &handle_CPX, &addr_abs, 4 };
  cpu->lookup[237] = (Opcode){ "SBC", &handle_SBC, &addr_abs, 4 };
  cpu->lookup[238] = (Opcode){ "INC", &handle_INC, &addr_abs, 6 };
  cpu->lookup[239] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[240] = (Opcode){ "BEQ", &handle_BEQ, &addr_rel, 7 };
  cpu->lookup[241] = (Opcode){ "SBC", &handle_SBC, &addr_izy, 6 };
  cpu->lookup[242] = (Opcode){ "???", &handle_XXX, &addr_imp, 2 };
  cpu->lookup[243] = (Opcode){ "???", &handle_XXX, &addr_imp, 8 };
  cpu->lookup[244] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[245] = (Opcode){ "SBC", &handle_SBC, &addr_zpx, 4 };
  cpu->lookup[246] = (Opcode){ "INC", &handle_INC, &addr_zpx, 6 };
  cpu->lookup[247] = (Opcode){ "???", &handle_XXX, &addr_imp, 6 };
  cpu->lookup[248] = (Opcode){ "SED", &handle_SED, &addr_imp, 2 };
  cpu->lookup[249] = (Opcode){ "SBC", &handle_SBC, &addr_aby, 4 };
  cpu->lookup[250] = (Opcode){ "NOP", &handle_NOP, &addr_imp, 2 };
  cpu->lookup[251] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };
  cpu->lookup[252] = (Opcode){ "???", &handle_XXX, &addr_imp, 4 };
  cpu->lookup[253] = (Opcode){ "SBC", &handle_SBC, &addr_abx, 4 };
  cpu->lookup[254] = (Opcode){ "INC", &handle_INC, &addr_abx, 7 };
  cpu->lookup[255] = (Opcode){ "???", &handle_XXX, &addr_imp, 7 };

  return cpu;
}

void cpu_destroy(CPU* cpu)
{
  if (cpu->bus)
    bus_destroy(cpu->bus);

  free(cpu->lookup);
  free(cpu);
}

void cpu_connect_bus(CPU* cpu, Bus* bus)
{
  cpu->bus = bus;
}

void cpu_reset(CPU* cpu)
{
  cpu->addr_abs = 0xFFFC;

  uint8_t lo = cpu_read(cpu, cpu->addr_abs);
  uint8_t hi = cpu_read(cpu, cpu->addr_abs + 1);

  cpu->pc = (hi << 8) | lo;

  cpu->a = 0x00;
  cpu->x = 0x00;
  cpu->y = 0x00;

  cpu->st = 0x00 | U;

  cpu->addr_abs = 0x00;
  cpu->addr_rel = 0x00;
  cpu->fetched = 0x00;

  cpu->cycles = 8;
}

uint8_t cpu_read(CPU* cpu, uint16_t address)
{
  return bus_read(cpu->bus, address);
}

void cpu_write(CPU* cpu, uint16_t address, uint8_t value)
{
  bus_write(cpu->bus, address, value);
}

bool cpu_flag_get(CPU* cpu, CPUFlags flag)
{
  return (cpu->st & flag) > 0 ? true : false;
}

void cpu_flag_set(CPU* cpu, CPUFlags flag, bool value)
{
  if (value)
    cpu->st |= flag;
  else
    cpu->st &= ~flag;
}

void cpu_clock(CPU* cpu)
{
  if (cpu->cycles == 0)
  {
    cpu->opcode = cpu_read(cpu, cpu->pc);
    cpu->pc++;

    cpu->cycles = cpu->lookup[cpu->opcode].cycles;

    uint8_t ac1 = (cpu->lookup[cpu->opcode].addrmode)(cpu);
    uint8_t ac2 = (cpu->lookup[cpu->opcode].operate)(cpu);

    cpu->cycles += (ac1 & ac2);
  }

  cpu->cycles--;

}

uint8_t cpu_fetch(CPU* cpu)
{
  if (cpu->lookup[cpu->opcode].addrmode != &addr_imp)
    cpu->fetched = cpu_read(cpu, cpu->addr_abs);

  return cpu->fetched;
}

void cpu_nmi(CPU* cpu)
{

  cpu_write(cpu, 0x100 + cpu->sp, (cpu->pc >> 8) & 0xFF);
  cpu->sp--;

  cpu_write(cpu, 0x100 + cpu->sp, cpu->pc & 0xFF);
  cpu->sp--;

  cpu_flag_set(cpu, B, false);
  cpu_flag_set(cpu, U, true);
  cpu_flag_set(cpu, I, 1);

  cpu_write(cpu, 0x100 + cpu->sp, cpu->st);
  cpu->sp--;

  cpu->addr_abs = 0xFFFA;
  
  uint8_t lo = cpu_read(cpu, cpu->addr_abs);
  uint8_t hi = cpu_read(cpu, cpu->addr_abs + 1);

  cpu->pc = (hi << 8) | lo;

  cpu->cycles = 8;

}

void cpu_irq(CPU* cpu)
{
  if (cpu_flag_get(cpu, I) == 0)
  {
    cpu_write(cpu, 0x100 + cpu->sp, (cpu->pc >> 8) & 0xFF);
    cpu->sp--;

    cpu_write(cpu, 0x100 + cpu->sp, cpu->pc & 0xFF);
    cpu->sp--;

    cpu_flag_set(cpu, B, false);
    cpu_flag_set(cpu, U, true);
    cpu_flag_set(cpu, I, 1);

    cpu_write(cpu, 0x100 + cpu->sp, cpu->st);
    cpu->sp--;

    cpu->addr_abs = 0xFFFE;
    
    uint8_t lo = cpu_read(cpu, cpu->addr_abs);
    uint8_t hi = cpu_read(cpu, cpu->addr_abs + 1);

    cpu->pc = (hi << 8) | lo;

    cpu->cycles = 7;

  }
}

bool cpu_complete(CPU* cpu)
{
  return cpu->cycles == 0;
}

char* cpu_disassemble(CPU* cpu)
{

  char* instr = malloc(sizeof(char) * 16);
  if (instr == NULL)
  {
    fprintf(stderr, "Failed to allocate memory, abort.\n");
    exit(-1);
  }

  uint8_t opcode = cpu_read(cpu, cpu->pc);

  if (cpu->lookup[opcode].addrmode == &addr_imp)
  {
    sprintf(instr, "$%04x: %s, {IMP}", cpu->pc, cpu->lookup[opcode].name);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_imm)
  {
    uint8_t value = cpu_read(cpu, cpu->pc+1);
    sprintf(instr, "$%04x: %s $%02x, {IMM}", cpu->pc, cpu->lookup[opcode].name, value);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_abs)
  {
    uint8_t lo = cpu_read(cpu, cpu->pc+1);
    uint8_t hi = cpu_read(cpu, cpu->pc+2);

    sprintf(instr, "$%04x: %s $%04x, {ABS}", cpu->pc, cpu->lookup[opcode].name, (hi << 8) | lo);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_abx)
  {
    uint8_t lo = cpu_read(cpu, cpu->pc+1);
    uint8_t hi = cpu_read(cpu, cpu->pc+2);

    sprintf(instr, "$%04x: %s $%04x, {ABX}", cpu->pc, cpu->lookup[opcode].name, (hi << 8) | lo);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_aby)
  {
    uint8_t lo = cpu_read(cpu, cpu->pc+1);
    uint8_t hi = cpu_read(cpu, cpu->pc+2);

    sprintf(instr, "$%04x: %s $%04x, {ABY}", cpu->pc, cpu->lookup[opcode].name, (hi << 8) | lo);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_ind)
  {
    uint8_t lo = cpu_read(cpu, cpu->pc+1);
    uint8_t hi = cpu_read(cpu, cpu->pc+2);

    sprintf(instr, "$%04x: %s $%04x, {IND}", cpu->pc, cpu->lookup[opcode].name, (hi << 8) | lo);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_izx)
  {
    uint8_t lo = cpu_read(cpu, cpu->pc+1);
    uint8_t hi = cpu_read(cpu, cpu->pc+2);

    sprintf(instr, "$%04x: %s $%04x, {IZX}", cpu->pc, cpu->lookup[opcode].name, (hi << 8) | lo);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_izy)
  {
    uint8_t lo = cpu_read(cpu, cpu->pc+1);
    uint8_t hi = cpu_read(cpu, cpu->pc+2);

    sprintf(instr, "$%04x: %s $%04x, {IZY}", cpu->pc, cpu->lookup[opcode].name, (hi << 8) | lo);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_rel)
  {
    uint8_t value = cpu_read(cpu, cpu->pc+1);
    sprintf(instr, "$%04x: %s $%02x, {REL}", cpu->pc, cpu->lookup[opcode].name, value);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_zp0)
  {
    uint8_t value = cpu_read(cpu, cpu->pc+1);
    sprintf(instr, "$%04x: %s $00%x, {ZP0}", cpu->pc, cpu->lookup[opcode].name, value);
  }
  
  else if (cpu->lookup[opcode].addrmode == &addr_zpx)
  {
    uint8_t value = cpu_read(cpu, cpu->pc+1);
    sprintf(instr, "$%04x: %s $00%x, {ZPX}", cpu->pc, cpu->lookup[opcode].name, value);
  }

  else if (cpu->lookup[opcode].addrmode == &addr_zpy)
  {
    uint8_t value = cpu_read(cpu, cpu->pc+1);
    sprintf(instr, "$%04x: %s $00%x, {ZPY}", cpu->pc, cpu->lookup[opcode].name, value);
  }


  return instr;
}
