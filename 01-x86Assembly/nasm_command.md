mac brew 安装 nasm：

```command

```

编译：

```command
nasm -f macho64 -o exam.o exam.asm
```

链接动态库：

```command
ld -macosx_version_min 11.3 -o exam -e _MAIN exam.o -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib -lSystem
```

win:
nasm -f win64 -o exam.o exam.asm
 cl.exe exam.obj /link /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib" libcmt.lib /ENTRY:_MAIN /SUBSYSTEM:CONSOLE

linux：

```shell
nasm -f elf64 hello.asm
gcc -o hello hello.o
```


new：

nasm  -o  hello.bin   -l hello.lst  hello.asm

其中  hello.bin 文件为二进制文件   可写入虚拟磁盘  生成操作系统

hello.lst 为列表文件   可查看程序代码段地址

hello.asm 为待编译文件
