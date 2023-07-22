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

#include "sdb.h"

#define NR_WP 32

static int number = 1;

typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char condation[32];
  word_t old_val;

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

static void insert_free(WP *wp)
{
  wp->next = free_->next;
  free_->next = wp;
}

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  // head是使用中的监视点结构
  head = NULL;
  // free_是空闲的监视点结构
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp(char *condation, bool *success)
{
  if (free_->next == NULL)
  {
    assert(0);
  }

  WP *ret = free_->next;
  ret->NO = number++;
  free_->next = ret->next;
  ret->next = NULL;

  word_t ans = expr(condation, success);
  strcpy(ret->condation, condation);
  ret->old_val = ans;

  if (head == NULL)
  {
    head = ret;
  }
  else
  {
    ret->next = head->next;
    head->next = ret;
  }

  return ret;
}

void free_wp(int NO)
{
  if (head->NO == NO)
  {
    WP *buffer = head->next;
    insert_free(head);
    head = buffer;
    return;
  }

  WP *prev = head;
  while (prev->next)
  {
    if (prev->next->NO == NO)
    {
      WP *buffer = prev->next->next;
      insert_free(prev->next);
      prev->next = buffer;
      return;
    }
    prev = prev->next;
  }

  printf("未找到 \e[1;36mWatchPoint(NO.%d)\e[0m\n", NO);
}

/*
void watchpoint_display()
{
  printf("NO. \t Condation\n");
  WP *cur = head;
  while (cur)
  {
    printf("\e[1;36m%d\e[0m\t\e[0;32m%s\e[0m\n", cur->NO, cur->condation);
    cur = cur->next;
  }
}
*/

bool check_watchpoint(WP **point)
{
  WP *cur = head;
  bool success = true;
  while (cur)
  {
    word_t new_val = expr(cur->condation, &success);
    if (new_val != cur->old_val)
    {
      *point = cur;
      return true;
    }
    cur = cur->next;
  }
  return false;
}
