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

#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct{
  word_t mcause;
  vaddr_t mepc;
  word_t mstatus;
  word_t mtvec;
}riscv32_CSRs;

typedef union mstatus_t {
  struct{
    uint32_t MIE : 1;     // 中断使能位
    uint32_t MPIE : 1;    // 先前中断使能位
    uint32_t MPP : 2;     // 先前特权级别
    uint32_t MPRV : 1;    // 加载和存储特权模式位
    uint32_t MXR : 1;     // 执行权限位
    uint32_t SUM : 1;     // 用户态访问控制位
    uint32_t TVM : 1;     // 虚拟化模式位
    uint32_t TW1 : 1;      // 态度控制位
    uint32_t TSR : 1;     // 时钟模式位
    uint32_t TW2 : 1;      // 写时钟模式位
    uint32_t UXL : 3;     // 用户模式扩展位
  };
  word_t val;
} mstatus_t;

typedef struct
{
  word_t gpr[32];
  vaddr_t pc;
  riscv32_CSRs csr;
} riscv32_CPU_state;

// decode
typedef struct
{
  union
  {
    uint32_t val;
  } inst;
} riscv32_ISADecodeInfo;

#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif
