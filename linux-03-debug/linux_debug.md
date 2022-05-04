[TOC]



# linux debug

## gdb

​	gcc 在生成时生成调试信息：

```shell
gcc -Wall -g3 -o test test.c
```

​	gcc 生成调试信息的一些选项：

- -g：该选项可以利用操作系统“原生格式”生成调试信息。gdb 可以直接利用这个信息，其他调试器也可以使用这个调试信息；
- -ggdb：使 gcc 为 gdb 生成专用的更为丰富的调试信息，但不能用其他调试器调试。