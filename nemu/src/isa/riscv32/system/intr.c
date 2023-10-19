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

word_t isa_raise_intr(word_t NO, vaddr_t epc)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  // NO 是
  // epc 是当前 pc 的值
  // printf("open\n");
  cpu.csr.mepc = epc;
  cpu.csr.mcause = NO;
  // printf("mcause is %d\n", cpu.csr.mcause);

  mstatus_t mstatus;
  mstatus.val = cpu.csr.mstatus;
  mstatus.MPIE = mstatus.MIE;
  mstatus.MIE = 0;
  cpu.csr.mstatus = mstatus.val;

  return cpu.csr.mtvec;
}

word_t isa_query_intr()
{
  return INTR_EMPTY;
}
