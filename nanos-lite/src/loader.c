#include <proc.h>
#include <elf.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);

int fs_open(const char *pathname, int flags, int mode);

static uintptr_t loader(PCB *pcb, const char *filename)
{
  // 这部分只在 PA3 第二阶段需要，那个阶段的ramdisk.img被替换为hello或是dummy程序的二进制代码，
  // 实际上ramdisk.img文件会自动生成 #include"files.h"中被生成
  /*
  Elf_Ehdr ehdr;
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  // check valid elf
  assert((*(uint32_t *)ehdr.e_ident == 0x464c457f));

  Elf_Phdr phdr[ehdr.e_phnum];
  ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr) * ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    if (phdr[i].p_type == PT_LOAD)
    {
      ramdisk_read((void *)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
      // set .bss with zeros
      memset((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;
  */
  enum
  {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
  };

  int fd = fs_open(filename, 0, 0);
  assert(fd >= 0);

  Elf_Ehdr ehdr;
  size_t len = fs_read(fd, (void *)&ehdr, sizeof(ehdr));
  assert(len == sizeof(ehdr));
  assert((*(uint32_t *)ehdr.e_ident == 0x464c457f));

  Elf_Phdr phdr[ehdr.e_phnum];
  fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  fs_read(fd, (void *)&phdr, sizeof(Elf_Phdr) * ehdr.e_phnum);

  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    if (phdr[i].p_type == PT_LOAD)
    {
      // Elf_Phdr:程序头表包含了可执行文件在内存中的各个段的信息
      fs_lseek(fd, phdr[i].p_offset, SEEK_SET);
      fs_read(fd, (void *)phdr[i].p_vaddr, phdr[i].p_memsz);
      // .bss 要填 0
      memset((void *)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  // 这是一个批处理程序，没有实现并发  文件在 navy-apps/fsimg/bin中
  uintptr_t entry = loader(pcb, "/bin/file-test");
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
