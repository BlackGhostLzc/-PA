#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  // 需要添加 open_offset 表示当前文件的偏移
  // 为了方便，初始化就为 disk_offset
  size_t open_offset;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    // file_table[fd] 文件描述符代表在 file_table 中的偏移
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
#include "files.h"
};

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

int fs_open(const char *pathname, int flags, int mode)
{
  // 遍历 file_table 数组，找到文件名，返回下标作为文件描述符
  for (int i = 0; i < LENGTH(file_table); i++)
  {
    if (file_table[i].name && strcmp(pathname, file_table[i].name) == 0)
    {
      // 设置 open_offset
      file_table[i].open_offset = file_table[i].disk_offset;
      return i;
    }
  }
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{

  if (file_table[fd].read)
  {
    len = file_table[fd].read(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }
  if (file_table[fd].open_offset - file_table[fd].disk_offset + len > file_table[fd].size)
  {
    len = file_table[fd].size + file_table[fd].disk_offset - file_table[fd].open_offset;
  }

  ramdisk_read(buf, file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  assert(fd >= 0 && fd <= LENGTH(file_table));

  if (file_table[fd].write)
  {
    // 如果 fd文件有个write函数，比如说说终端的write是serial_write函数，但offset和size都是0
    len = file_table[fd].write(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }
  else
  {
    // 否则当成是一个存储在 ramdisk 中的文件
    // 首先判断是否超出文件范围
    if (file_table[fd].open_offset + len > file_table[fd].disk_offset + file_table[fd].size)
    {
      len = file_table[fd].disk_offset + file_table[fd].size - file_table[fd].open_offset;
    }
    ramdisk_write(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
    return len;
  }
  assert(0);
  return -1;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  /*
  size_t new_offset;
  switch (whence)
  {
  case SEEK_SET:
    new_offset = file_table[fd].disk_offset + offset;
    break;

  case SEEK_CUR:
    new_offset = file_table[fd].open_offset + offset;
    break;

  case SEEK_END:
    new_offset = file_table[fd].disk_offset + file_table[fd].size;
    break;

  default:
    new_offset = -1;
    assert(0);
  }
  if (new_offset >= file_table[fd].disk_offset && new_offset <= file_table[fd].disk_offset + file_table[fd].size)
  {
    file_table[fd].open_offset = new_offset;
    return new_offset - file_table[fd].disk_offset;
  }
  return -1;
  */
  Finfo *info = &file_table[fd];

  switch (whence)
  {
  case SEEK_CUR:
    assert(info->open_offset + offset <= info->size);
    info->open_offset += offset;
    break;

  case SEEK_SET:
    assert(offset <= info->size);
    info->open_offset = offset;
    break;

  case SEEK_END:
    assert(offset <= info->size);
    info->open_offset = info->size + offset;
    break;

  default:
    assert(0);
  }

  return info->open_offset;
}

int fs_close(int fd)
{
  return 0;
}

void init_fs()
{
  // TODO: initialize the size of /dev/fb
}
