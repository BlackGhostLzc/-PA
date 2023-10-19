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

void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1; // a7
  a[1] = c->GPR2; // a0
  a[2] = c->GPR3; // a1
  a[3] = c->GPR4;

#ifdef CONFIG_STRACE
  printf("STRACE: [#%3ld]( %ld, %ld, %ld )\n", a[0], a[1], a[2], a[3]);
#endif

  switch (a[0])
  {
  case SYS_exit:
    halt(a[1]);
    c->GPR4 = a[1];
    break;
  case SYS_yield:
    yield();
    c->GPRx = 0;
    break;
  case SYS_write:
    c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
    break;

    /*
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

  case SYS_brk:
    c->GPRx = 0;
    break;

  case SYS_read:
    c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
    break;

  case SYS_open:
    printf("hahaha怎么回事呢\n");
    int ret = fs_open((char *)c->GPR1, c->GPR2, c->GPR3);
    c->GPRx = ret;
    printf("open\n\n\n\n");
    break;

  case SYS_lseek:
    c->GPRx = fs_lseek(a[1], a[2], a[3]);
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }

#ifdef CONFIG_SYSCALLTRACE
  printf("SYSCALL:       [%s] -> %ld\n", syscall_names[a[0]], c->GPRx);
#endif
}
