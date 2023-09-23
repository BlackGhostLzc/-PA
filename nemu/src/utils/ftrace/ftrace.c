
// #ifdef FTRACE

#include "ftrace.h"

static int elf_idx = 0;
static void append_elfunc(char *func_name, paddr_t start, size_t size)
{
    strncpy(elf_funcs[elf_idx].func_name, func_name, sizeof(func_name));
    elf_funcs[elf_idx].start = start;
    elf_funcs[elf_idx].size = size;
    elf_idx++;
}

FUNC_INFO *check_func(paddr_t addr)
{
    for (int i = 0; i < elf_idx; i++)
    {
        FUNC_INFO *ret = &elf_funcs[i];
        if (addr >= elf_funcs[i].start && addr < elf_funcs[i].start + elf_funcs[i].size)
        {
            return ret;
        }
    }
    return NULL;
}

static void read_from_file(FILE *elf, size_t offset, size_t size, void *dest)
{
    fseek(elf, offset, SEEK_SET);
    // size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
    // count 表示读取数据量的个数, 例如一个 Elf32_hdr 大小为 size, 我只要读取一个
    size_t count = fread(dest, size, 1, elf);
    assert(count == 1);
}

static void read_funcname_file(FILE *elf, size_t offset, size_t n, char *dest)
{
    fseek(elf, offset, SEEK_SET);
    fread(dest, sizeof(char), n, elf);
    // fgets(dest, n, elf);
}

void init_elf(const char *elf_file)
{
    // 只读，二进制
    FILE *file = fopen(elf_file, "rb");
    assert(file != NULL);

    // 首先读取 ELF 文件头
    Elf32_Ehdr elf_hdr;
    read_from_file(file, 0, sizeof(Elf32_Ehdr), &elf_hdr);

    // ELF 文件头中有一个段表，保存这些段基本属性的结构，每个段都是 Elf32_Shdr 这样一个结构，记录了偏移
    Elf32_Off section_headers_offset = elf_hdr.e_shoff;    // 段表在 ELF 文件中的偏移
    Elf32_Half section_headers_size = elf_hdr.e_shentsize; // 每个段表的大小
    Elf32_Half section_headers_num = elf_hdr.e_shnum;      // ELF 文件中段表的个数

    assert(section_headers_size == sizeof(Elf32_Shdr));

    Elf32_Shdr section_hdr;

    Elf32_Off symtab_offset;
    Elf32_Word symtab_total_size;
    Elf32_Word symtab_entry_size;

    Elf32_Word string_offset;

    for (int i = 0; i < section_headers_num; i++)
    {
        size_t offset = section_headers_offset + i * section_headers_size;
        read_from_file(file, offset, section_headers_size, &section_hdr);
        if (section_hdr.sh_type == SHT_SYMTAB)
        {
            // 该段表是符号表的信息
            symtab_offset = section_hdr.sh_offset;
            symtab_total_size = section_hdr.sh_size;
            symtab_entry_size = section_hdr.sh_entsize;
        }
        else if (section_hdr.sh_type == SHT_STRTAB)
        {
            // 字符串表的信息
            if (i == elf_hdr.e_shstrndx)
            {
                // 这个字符串表是段表字符串表
                // 注意看 Elf32_Shdr中也有 sh_name 这一个字段，表示段名也需要字符串来表示
                // DummyCode()
            }
            else
                string_offset = section_hdr.sh_offset;
        }
    }

    Elf32_Sym sym_entry;
    char function_name[64];

    for (int i = 0; i < symtab_total_size / symtab_entry_size; i++)
    {
        size_t offset = symtab_offset + i * symtab_entry_size;
        read_from_file(file, offset, symtab_entry_size, &sym_entry);
        if ((sym_entry.st_info & 0xf) == STT_FUNC)
        {
            // 首先要把函数名字读取出来
            read_funcname_file(file, string_offset + sym_entry.st_name, sizeof(function_name), function_name);
            append_elfunc(function_name, sym_entry.st_value, sym_entry.st_size);
        }
    }
}

void init_ftrace(const char *elf_file)
{
    init_elf(elf_file);

    stack_header.cur_info = NULL;
    stack_header.des_info = NULL;
    stack_header.addr = 0;
    stack_header.next = NULL;
}

void append_entry(paddr_t cur, paddr_t des, int type)
{
    FUNC_INFO *x = check_func(cur);
    FUNC_INFO *y = check_func(des);
    if (x == NULL || y == NULL)
    {
        return;
    }

    STACK_ENTRY *stack_node = malloc(sizeof(STACK_ENTRY));
    stack_cur->next = stack_node;

    stack_node->addr = cur;
    stack_node->cur_info = x;
    stack_node->des_info = y;

    stack_node->type = type;
    stack_node->next = NULL;

    stack_cur = stack_node;
}

void print_func_trace()
{
    int depth = 0;
    for (FUNC_INFO *node = stack_header.next; node != NULL; node = node->next)
    {

        if (node->type == FT_CALL)
        {
            printf("0x%08x ", node->addr);
            printf("%*s", depth, "");
            printf("call [%s@0x%08x] \n", node->des_info->func_name, node->des_info->addr);
            depth += 2;
        }
        else if (node->type == FT_RET)
        {
            printf("0x%08x ", node->addr);
            printf("%*s", depth, "");
            printf("ret [%s] \n", node->des_info->func_name);
            depth -= 2;
        }
    }
}

// #endif