#include <common.h>
#include "syscall.h"
/*
static char *syscall_names[] = {
    "SYS_exit", "SYS_yield", "SYS_open", "SYS_read", "SYS_write", "SYS_kill", "SYS_getpid", "SYS_close",
    "SYS_lseek", "SYS_brk", "SYS_fstat", "SYS_time", "SYS_signal", "SYS_execve", "SYS_fork", "SYS_link",
    "SYS_unlink", "SYS_wait", "SYS_times", "SYS_gettimeofday"};
*/

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

/*
void sys_write(Context *c)
{
  int ret = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1; // a7
  a[1] = c->GPR2; // a0
  a[2] = c->GPR3; // a1
  a[3] = c->GPR4;

  // #ifdef CONFIG_SYSCALLTRACE
  printf("SYSCALLTRACE: [#%3ld]( %ld, %ld, %ld )\n", a[0], a[1], a[2], a[3]);
  // #endif

  switch (a[0])
  {
  case SYS_exit:
    // naive_uload(NULL, "/bin/menu");
    halt(a[1]);
    // c->GPR4 = a[1];
    // c->GPRx = a[1];
    break;
  case SYS_yield:
    // yield();
    c->GPRx = 0;
    break;
  case SYS_write:
    sys_write(c);
    break;


    if (a[1] == 1 || a[1] == 2)
    {
      for (int i = 0; i < a[3]; i++)
      {
        putch(*((char *)(a[2] + i)));
      }
    }
    c->GPRx = a[3];
    break;
    */
/*
  case SYS_brk:
    c->GPRx = 0;
    break;

  case SYS_read:
    c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
    break;

  case SYS_open:
    // printf("hahaha怎么回事呢\n");
    int ret = fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
    c->GPRx = ret;
    // printf("open\n\n\n\n");
    break;

  case SYS_lseek:
    c->GPRx = fs_lseek(a[1], a[2], a[3]);
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
*/
// #ifdef CONFIG_SYSCALLTRACE
//  printf("SYSCALL:       [%s] -> %ld\n", syscall_names[a[0]], c->GPRx);
// #endif
// }

void sys_yield(Context *c)
{
  // yield();
  c->GPRx = 0;
}

void sys_exit(Context *c)
{
  c->GPRx = c->GPR2;
}
void sys_write(Context *c)
{
  int ret = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_read(Context *c)
{
  int ret = fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_brk(Context *c)
{
  c->GPRx = 0;
}

void sys_open(Context *c)
{
  int ret = fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_close(Context *c)
{
  int ret = fs_close(c->GPR2);
  c->GPRx = ret;
}

void sys_lseek(Context *c)
{
  int ret = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = ret;
}

// struct __temp{
//  #if __SIZEOF_POINTER__ == 8
//   __uint64_t sec;
//   __uint64_t usec;
//  #elif __SIZEOF_POINTER__ == 4
//   __uint32_t sec;
//   __uint32_t usec;
// #endif
// };

void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1; // a7
  a[1] = c->GPR2; // a0
  a[2] = c->GPR3; // a1
  a[3] = c->GPR4; // a2
  printf("SYSCALLTRACE: [#%3ld]( %ld, %ld, %ld )\n", a[0], a[1], a[2], a[3]);
  switch (a[0])
  {
  case SYS_yield:
    sys_yield(c);
    break;

  case SYS_exit:
    sys_exit(c);
    break;

  case SYS_write:
    sys_write(c);
    break;

  case SYS_brk:
    sys_brk(c);
    break;

  case SYS_open:
    sys_open(c);
    break;

  case SYS_close:
    sys_close(c);
    break;

  case SYS_read:
    sys_read(c);
    break;

  case SYS_lseek:
    sys_lseek(c);
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
