#!/bin/bash

# 设置变量
interpreter="./build/riscv32-nemu-interpreter"
log_file="/home/blackghost/Desktop/ics-pa-2022/nemu/build/nemu-log.txt"
diff_tool="/home/blackghost/Desktop/ics-pa-2022/nemu/tools/spike-diff/build/riscv32-spike-so"
elf_dir="../am-kernels/tests/cpu-tests/build"
elf_extension=".elf"
bin_extension=".bin"
input_file="/tmp/input.txt"

echo "c" > "$input_file"
echo "q" >> "$input_file"

# 遍历目录中的elf文件
for elf_file in "$elf_dir"/*"$elf_extension"; do
    # 提取elf文件名（不包含路径和后缀）
    base_name=$(basename "$elf_file" "$elf_extension")
    
    # 构建对应的bin文件路径
    bin_file="$elf_dir/$base_name$bin_extension"
    
    # 检查bin文件是否存在
    if [ -f "$bin_file" ]; then
        # 执行命令
        "$interpreter" --log="$log_file" --diff="$diff_tool" --elf="$elf_file" "$bin_file" < "$input_file"
    else
        echo "对应的bin文件不存在: $bin_file"
    fi
done