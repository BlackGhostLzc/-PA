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
#include <memory/paddr.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

bool check_parentheses(int p, int q, int *position);
int prio(char type);

enum
{
  TK_NOTYPE = 0x41,
  TK_EQ,
  NUM,
  HEX,
  TK_UEQ,
  REG,
  DEREF,
  MINUS
};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {                // 这里面不要有字符的type，因为标识从A开始
    {"0x[0-9A-Fa-f]+", HEX}, // 16进制数字
    {"\\$[0-9a-z]+", REG},   // 寄存器
    {"[0-9]+", NUM},         // 数字
    {"\\(", '('},            // 左括号
    {"\\)", ')'},            // 右括号
    {"\\+", '+'},            // plus
    {"\\-", '-'},            // sub
    {"\\*", '*'},            // mul
    {"\\/", '/'},            // divide
    {" +", TK_NOTYPE},       // spaces
    {"==", TK_EQ},           // equal
    {"!=", TK_UEQ},
    {"&&", '&'},
    {"\\|\\|", '|'}};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        if (substr_len > 32)
        {
          assert(0);
        }

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type)
        {
        case '*':
        case '-':
          // 可能是乘/减 或者 解引用/负数
          if (nr_token == 0 || tokens[nr_token - 1].type == '(' || prio(tokens[nr_token - 1].type) > 0)
          {
            // 这种是后面那种情况
            switch (rules[i].token_type)
            {
            case '*':
              tokens[nr_token].type = DEREF;
              break;
            case '-':
              tokens[nr_token].type = MINUS;
              break;
            }
          }
          else if (tokens[nr_token - 1].type == ')' || tokens[nr_token - 1].type == NUM || tokens[nr_token - 1].type == HEX || tokens[nr_token - 1].type == REG)
          {
            tokens[nr_token].type = rules[i].token_type;
          }
          else
          {
            assert(0);
          }
          nr_token++;
          break;

        case TK_NOTYPE:
          break;
        case NUM:
        case HEX:
        case REG:
          memcpy(tokens[nr_token].str, e + position - substr_len, (substr_len) * sizeof(char));
          tokens[nr_token].str[substr_len] = '\0';

        default:
          tokens[nr_token].type = rules[i].token_type;
          nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

word_t eval(int p, int q, bool *success, int *position);

word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }
  *success = true;
  int position = 0;
  int ans = eval(0, nr_token - 1, success, &position);
  /* TODO: Insert codes to evaluate the expression. */

  if (!*success)
  {
    printf("Some problems happens at position %d\n%s\n%*.s^\n", position, e, position, "");
  }

  return ans;
}

u_int32_t eval(int p, int q, bool *success, int *position)
{
  if (p > q)
  {
    *success = false;
    return 0;
  }
  else if (p == q)
  {
    u_int32_t buffer = 0;
    switch (tokens[p].type)
    {
    case HEX:
      sscanf(tokens[p].str, "%x", &buffer);
      break;

    case NUM:
      sscanf(tokens[p].str, "%u", &buffer);
      break;

    case REG:
      if (strcmp(tokens[p].str, "$pc") == 0)
      {
        buffer = cpu.pc;
        *success = true;
      }
      else
      {
        buffer = isa_reg_str2val(tokens[p].str, success);
      }
      if (!*success)
      {
        *position = p;
        return 0;
      }
      break;

    default:
      assert(0);
    }
    return buffer;
  }
  else if (q - p == 1 || check_parentheses(p + 1, q, position) == true)
  {
    switch (tokens[p].type)
    {
    case DEREF:
      return *((uint32_t *)guest_to_host(eval(p + 1, q, success, position)));
      break;

    case MINUS:
      return -eval(p + 1, q, success, position);

    default:
      assert(0);
    }
  }
  else if (check_parentheses(p, q, position) == true)
  {
    return eval(p + 1, q - 1, success, position);
  }
  else
  {
    if (*position != -1)
    {
      *success = false;
      return 0;
    }
    int op = -1, level = -1;
    for (int i = p; i <= q; i++)
    {
      if (tokens[i].type == '(')
      {
        level++;
      }
      else if (tokens[i].type == ')')
      {
        level--;
      }
      else if (level == -1 && prio(tokens[i].type) >= 0)
      {
        if (op == -1 || prio(tokens[i].type) <= prio(tokens[op].type))
        {
          op = i;
        }
      }
    }
    if (op == -1)
    {
      *success = false;
      *position = 0;
      return 0;
    }
    u_int32_t val1 = eval(p, op - 1, success, position);
    u_int32_t val2 = eval(op + 1, q, success, position);

    switch (tokens[op].type)
    {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2;
    case '*':
      return val1 * val2;
    case '/':
      return val1 / val2;
    case TK_EQ:
      return val1 == val2;
    case TK_UEQ:
      return val1 != val2;
    case '|':
      return val1 || val2;
    case '&':
      return val1 && val2;
    default:
      assert(0);
    }
  }
  return 0;
}

#define STACK_SIZE 1024
bool check_parentheses(int p, int q, int *position)
{
  char stack[STACK_SIZE];
  *position = -1;
  int top = -1, index = p;
  bool is_parentheses = tokens[p].type == '(';
  while (index <= q)
  {
    if (tokens[index].type == '(')
    {
      stack[++top] = '(';
    }
    else if (tokens[index].type == ')')
    {
      if (top < 0 || stack[top] != '(')
      {
        *position = p;
        return false;
      }
      else
      {
        top--;
      }
    }
    if (index < q)
      is_parentheses = (top >= 0) && is_parentheses;
    index++;
  }
  if (top != -1)
  {
    *position = p;
    return false;
  }
  return is_parentheses;
}

#define PRIOROTY_BASE 16

int prio(char type)
{
  switch (type)
  {
  case '|':
    return PRIOROTY_BASE + 0;

  case '&':
    return PRIOROTY_BASE + 1;

  case TK_EQ:
  case TK_UEQ:
    return PRIOROTY_BASE + 2;

  case '+':
  case '-':
    return PRIOROTY_BASE + 3;

  case '*':
  case '/':
    return PRIOROTY_BASE + 4;

  default:
    return -1;
  }
}