/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "tp", "sp", "a0", "a1", "a2", "a3",
  "a4", "a5", "a6", "a7", "t0", "t1", "t2", "t3",
  "t4", "t5", "t6", "t7", "t8", "rs", "fp", "s0",
  "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8"
};

void isa_reg_display() {
  int length = ARRLEN(regs);
  int i = 0;
  printf("=========寄存器信息=========\n");
  for (i = 0; i < length; i+= REGISTERS_PER_LINE){
    for (int j = i; j < MIN(length, i + REGISTERS_PER_LINE); ++j){
      printf("\e[1;36m%3s:\e[0m %#12x | ", regs[j], cpu.gpr[j]._32);
    }
    printf("\n");
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  *success = true;
  if (strcmp(s, regs[0]) == 0){
    return cpu.gpr[0]._32;
  }

  for (int i = 1; i < ARRLEN(regs); ++i){
    if (strcmp(regs[i], s+1) == 0){//跳过$
      *success = true;
      return cpu.gpr[i]._32;
    }
  }

  *success = false;
  return -1;
}
