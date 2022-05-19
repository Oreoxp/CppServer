[TOC]



# linux debug

## gdb调试

​	gcc 在生成时生成调试信息：

```shell
gcc -Wall -g3 -o test test.c
```

​	gcc 生成调试信息的一些选项：

- -g：该选项可以利用操作系统“原生格式”生成调试信息。gdb 可以直接利用这个信息，其他调试器也可以使用这个调试信息；

- -ggdb：使 gcc 为 gdb 生成专用的更为丰富的调试信息，但不能用其他调试器调试。

  

-g 和 -ggdb 也是分级别的

* -g2：这是默认的级别，此时产生的调试信息包括扩展的符号表、行号、局部或外部变量信息。
* -g3：包含级别2中的所有调试信息，以及源代码中定义的宏，以及C++中的内联函数（inline）。
* -g1：级别1（-g1）不包含局部变量和与行号有关的调试信息，因此只能够用于回溯跟踪和堆栈转储之用。回溯跟踪指的是监视程序在运行过程中的函数调用历史，堆栈转储则是一种以原始的十六进制格式保存程序执行环境的方法，两者都是经常用到的调试手段。

### 启动 gdb

调试可执行文件:

```
$gdb <program>
```

program也就是你的执行文件，一般在当前目录下。

调试core文件(core是程序非法执行后core dump后产生的文件):

```
$gdb <program> <core dump file>
$gdb program core.11127
```

调试服务程序:

```
$gdb <program> <PID>
$gdb hello 11127
```

如果你的程序是一个服务程序，那么你可以指定这个服务程序运行时的进程ID。gdb会自动attach上去，并调试他。program应该在PATH环境变量中搜索得到。

### gdb命令

启动gdb后，进入到交互模式，通过以下命令完成对程序的调试；注意高频使用的命令一般都会有缩写，熟练使用这些缩写命令能提高调试的效率；

#### 运行相关命令

* run：简记为 r ，其作用是运行程序，当遇到断点后，程序会在断点处停止运行，等待用户输入下一步的命令，run后可以带一些命令，比如我们要调试brks程序，我们第一步是gdb brks，进入gdb后，我们输入run log.conf去运行程序。  
* continue （简写c ）：继续执行，到下一个断点处（或运行结束）  
* next：（简写 n），单步跟踪程序，当遇到函数调用时，也不进入此函数体；此命令同 step 的主要区别是，step 遇到用户自定义的函数，将步进到函数中去运行，而 next 则直接调用函数，不会进入到函数体内。  
* step （简写s）：单步调试如果有函数调用，则进入函数；与命令n不同，n是不进入调用的函数的。  
* until：当你厌倦了在一个循环体内单步跟踪时，这个命令可以运行程序直到退出循环体。  
* until+行号： 运行至某行，不仅仅用来跳出循环。  
* finish： 运行程序，直到当前函数完成返回，并打印函数返回时的堆栈地址和返回值及参数值等信息。  
* call 函数(参数)：调用程序中可见的函数，并传递“参数”，如：call gdb_test(55)  
* quit：简记为 q ，退出gdb

#### 设置断点

* break n （简写b n）:在第n行处设置断点
  （可以带上代码路径和行号： b OAGUPDATE.cpp:578）  
* b fn1 if a＞b：条件断点设置  
* break func（break缩写为b）：在函数func()的入口处设置断点，如：break cb_button  
* delete 断点号n：删除第n个断点  
* disable 断点号n：暂停第n个断点  
* enable 断点号n：开启第n个断点  
* clear 行号n：清除第n行的断点  
* info b （info breakpoints） ：显示当前程序的断点设置情况  
* delete breakpoints：清除所有断点：

#### 查看源代码

* list ：简记为 l ，其作用就是列出程序的源代码，默认每次显示10行。
* list 行号：将显示当前文件以“行号”为中心的前后10行代码，如：list 12
* list 函数名：将显示“函数名”所在函数的源代码，如：list main
* list ：不带参数，将接着上一次 list 命令的，输出下边的内容。

#### 打印表达式

* print 表达式：简记为 p ，其中“表达式”可以是任何当前正在被测试程序的有效表达式，比如当前正在调试C语言的程序，那么“表达式”可以是任何C语言的有效表达式，包括数字，变量甚至是函数调用。  
* print a：将显示整数 a 的值  
* print ++a：将把 a 中的值加1,并显示出来  
* print name：将显示字符串 name 的值  
* print gdb_test(22)：将以整数22作为参数调用 gdb_test() 函数  
* print gdb_test(a)：将以变量 a 作为参数调用 gdb_test() 函数  
* display 表达式：在单步运行时将非常有用，使用display命令设置一个表达式后，它将在每次单步进行指令后，紧接着输出被设置的表达式及值。如： display a  
* watch 表达式：设置一个监视点，一旦被监视的“表达式”的值改变，gdb将强行终止正在被调试的程序。如： watch a  
* whatis ：查询变量或函数  
* info function： 查询函数  
* 扩展info locals： 显示当前堆栈页的所有变量

#### 查询运行信息

* where/bt ：当前运行的堆栈列表；
* bt backtrace 显示当前调用堆栈
* up/down 改变堆栈显示的深度
* set args 参数:指定运行时的参数
* show args：查看设置好的参数
* info program： 来查看程序的是否在运行，进程号，被暂停的原因。

#### 分割窗口

* layout：用于分割窗口，可以一边查看代码，一边测试：
* layout src：显示源代码窗口
* layout asm：显示反汇编窗口
* layout regs：显示源代码/反汇编和CPU寄存器窗口
* layout split：显示源代码和反汇编窗口
* Ctrl + L：刷新窗口



## core dump

&emsp;程序异常退出时，会产生一个core文件，该文件记录了程序运行时的内存，寄存器状态，堆栈指针，内存管理信息还有各种函数调用堆栈信息等，我们可以理解为是程序工作当前状态存储生成的一个文件，通过工具分析这个文件，我们可以定位到程序异常退出的时候对应的堆栈调用等信息，找出问题所在并进行及时解决。

&emsp;&emsp;上面说当程序运行过程中异常终止或崩溃时会发生 core dump，但还没说到什么具体的情景程序会发生异常终止或崩溃，例如我们使用 kill -9 命令杀死一个进程会发生 core dump 吗？实验证明是不能的，那么什么情况会产生呢？  
&emsp;&emsp;Linux 中信号是一种异步事件处理的机制，每种信号对应有其默认的操作，你可以在 这里 查看 Linux 系统提供的信号以及默认处理。默认操作主要包括忽略该信号（Ingore）、暂停进程（Stop）、终止进程（Terminate）、终止并发生core dump（core）等。如果我们信号均是采用默认操作，那么，以下列出几种信号，它们在发生时会产生 core dump，这个可以通过`man 7 signal`:![9_1](.\markdownimage\9_1.png)



​		当然不仅限于上面的几种信号。这就是为什么我们使用 Ctrl+z 来挂起一个进程或者 Ctrl+C 结束一个进程均不会产生 core dump，因为前者会向进程发出 SIGTSTP 信号，该信号的默认操作为暂停进程（Stop Process）；后者会向进程发出SIGINT 信号，该信号默认操作为终止进程（Terminate Process）。同样上面提到的 kill -9 命令会发出 SIGKILL 命令，该命令默认为终止进程。而如果我们使用 Ctrl+\ 来终止一个进程，会向进程发出 SIGQUIT 信号，默认是会产生 core dump 的。还有其它情景会产生 core dump， 如：程序调用 abort() 函数、访存错误、非法指令等等。



### 前期设置

1. 设置 core 文件生成的目录，其中 %e 表示程序文件名，%p 表示进程 ID，否则会在程序的当前目录生成dore文件；

```
	 echo /data/coredump/core.%e.%p >/proc/sys/kernel/core_pattern   
```

2. 当前执行程序的用户对 core 目录有写权限且有足够的空间存储 core 文件；

3. 生成不受限制的 core 文件，`ulimit  -c unlimited`，这个只会对当前的终端有效，如果想永久有效，需要修改文件：/etc/security/limits.conf，增加一行，如下红圈所示：  
   ![设置core文件](F:\Github\server_develop_guide_book\chapter9\9_2.png)



### 使用 gdb 调试 coredump 文件

&emsp;&emsp;产生了 core 文件，我们该如何使用该 Core 文件进行调试呢？Linux 中可以使用 GDB 来调试 core 文件，命令如下：

```
gdb program core
```

​		program 是程序名称，core 是 coredump 文件。

​		比如：
![coredump](F:\Github\server_develop_guide_book\chapter9\9_3.png)
那都有哪些操作命令可以调试这个coredump文件呢？

* bt 或者 where 查看调用栈信息
      如果你要查看某一层的信息，你需要切换当前的栈，一般来说，程序停止时，最顶层的栈就是当前栈，如果你要查看栈下面层的详细信息，首先要做的是切换当前栈。  
  * frame \<n>和 f \<n>   
    	n是 一个从 0 开始的整数，是栈中的层编号。比如：frame 0，表示栈顶，frame 1，表示栈的第二层。
* up <n>  
  &emsp;&emsp;表示向栈的上面移动 n 层，可以不打 n，表示向上移动一层。 
* down <n> 
      表示向栈的下面移动 n 层，可以不打 n，表示向下移动一层。   
  	

&emsp;&emsp;上面的命令，都会打印出移动到的栈层的信息。如果你不想让其打出信息。你可以使用这三个命令：

```
            select-frame   <n>  对应于 frame 命令。
            up-silently    <n>  对应于 up 命令。
            down-silently  <n>  对应于 down 命令。
```

&emsp;&emsp;查看当前栈层的信息，你可以用以下GDB命令：

* frame 或 f 
  &emsp;&emsp;会打印出这些信息：栈的层编号，当前的函数名，函数参数值，函数所在文件及行号，函数执行到的语句。
* info frame 和 info f 
  &emsp;&emsp;这个命令会打印出更为详细的当前栈层的信息，只不过，大多数都是运行时的内内地址。比如：函数地址，调用函数的地址，被调用函数的地址，目前的函数是由什么样的程序语言写成的、函数参数地址及值、局部变量的地址等等。

### 奔溃日志

​		生成 coredump 去用 gdb 调试还是依赖我们是否生成调试信息，如果我们的没有添加 -g 选项就麻烦了，此时我们可以怎么办呢？  
&emsp;&emsp;上文说过程序崩溃（core）了是由一些信号触发的，这些信号也许是因为用户操作，也许是我们程序有一些错误的指令（比如对空指针的赋值，除数为零）触发的。那么我们程序实际上是可以捕获这些信号的，然后去打印堆栈的信息，how to do？  
&emsp;&emsp;signal是一个截取信号的函数，它的申明如下：

```
#include <signal.h>

typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);
```

​		signal 截取信号，然后调用回调函数 handler 处理信号，那么其实我们就可以在 handler 函数里打印堆栈信息，backtrace 获取堆栈的大小，并且把堆栈的信息放在 array 指向的内存中，然后调用backtrace_symbols 把堆栈中的函数信息转换成函数名。例如：

```
int main(int argc, char** argv)
{
    signal(SIGSEGV, handle_segv); // SIGSEGV    11  Core Invalid memory reference
    signal(SIGABRT, handle_segv); // SIGABRT    6       Core Abort signal from
	signal(SIGINT, handle_segv);
	signal(SIGTSTP, handle_segv);
	signal(SIGTERM, handle_segv);

    //...
	
	return 0;
}
void handle_segv(int signum)
{
    void *array[100];
    size_t size;
    char **strings;
    size_t i;
	
    signal(signum, SIG_DFL); /* 还原默认的信号处理handler */

    size = backtrace (array, 100);
    strings = (char **)backtrace_symbols (array, size);
	
	// 这里是打印到日志中，其实也可以打印到某个文件中
    LOG_INFO("Launcher received SIG: %d Stack trace:\n", signum);
    for (i = 0; i < size; i++)
    {
        LOG_ERROR("%d %s \n",i,strings[i]);
    }

    free (strings);
	//g_launcher->stop();
}

```

&emsp;&emsp;从上文，我们也可以看出我们在程序崩溃前可以做一些善后处理。





## 内存泄漏检查

### 怎么就知道内存泄漏了

&emsp;&emsp;我们如果不去特意查内存泄漏，实际上是很难发现的，因为这个程序即使发生内存泄漏，他也不会有任何特征（泄漏速度不是很快），知道某一天，我们发现系统运行变慢了，或者出现了其它莫名其妙的问题，等到我们把系统重启，发现整个程序运行又正常了，这大致就是内存泄漏遇见我们时，玩的捉迷藏游戏。因为程序是一个动态运行的过程，所以我们很难发现一定就内存泄漏了，那可以先用top命令跟踪下某个进程，哦先来了解top命令：

&emsp;&emsp;在终端上敲入top命令，显示结果如下：
![top命令](F:\Github\server_develop_guide_book\chapter9\9_4.png)
&emsp;&emsp;top命令是Linux下常用的性能分析工具，能够实时显示系统中各个进程的资源占用状况，类似于Windows的任务管理器。top是一个动态显示过程,即可以通过用户按键来不断刷新当前状态.如果在前台执行该命令,它将独占前台,直到用户终止该程序为止.比较准确的说,top命令提供了实时的对系统处理器的状态监视.它将显示系统中CPU最“敏感”的任务列表.该命令可以按CPU使用.内存使用和执行时间对任务进行排序；而且该命令的很多特性都可以通过交互式命令或者在个人定制文件中进行设定。

* 第一行
  * 16:19:51 ：系统当前时间
  * up 9 days, 22:22 ：系统开机到现在经过了多少时间
  * 2 users ：当前两个用户在线
  * 1.14, 0.73, 0.32 ：系统1分钟、5分钟、15分钟的CPU负载信息
* 第二行
  * Tasks ：任务
  * 105 total ：很好理解，就是当前有87个任务，也就是87个进程。
  * 2 running ：两个进程正在运行
  * 73 sleeping ：73个进程正在休眠
  * 0 stopped：停止的进程数
  * 0 zombie：僵尸进程数
 * 第三行
   * %Cpu(s): 表示这一行显示CPU总体信息
   * 100.0 us：用户态进程占用CPU时间百分比，不包含renice值为负的任务占用的CPU的时间。
   * 0.0 sy：内核占用CPU时间百分比
   * 0.0 ni：改变过优先级的进程占用CPU的百分比
   * 0.0 id：空闲CPU时间百分比
   * 0.0 wa：等待I/O的CPU时间百分比
   * 0.0 hi：CPU硬中断时间百分比
   * 0.0 si：CPU软中断时间百分比
   * 0.0 st： 
     注：这里显示数据是所有cpu的平均值，如果想看每一个cpu的处理情况，按1即可；折叠，再次按1；
* 第四行
  * Mem : 内存的信息
  * 2041248 total：物理内存总量
  * 71508 free：空闲的物理内存量
  * 706876 used：使用的物理内存量
  * 1262864 buff/cache ：用作内核缓存的物理内存量
 * 第五行
   * Swap: 虚拟内存使用（交换空间）
   * 0 total：交换区总量
   * 0 free：空闲
   * 0 used：使用
   * 1140696 avail Mem ：缓存交换空间
     以下就是是以列显示的进程。不同的列代表下面要解释的不同属性。默认上，top显示这些关于进程的属性：
* PID：进程ID，进程的唯一标识符
* USER：进程所有者的实际用户名。
* PR：进程的调度优先级。这个字段的一些值是'rt'。这意味这这些进程运行在实时态。
* NI：进程的nice值（优先级）。越小的值意味着越高的优先级。
* VIRT：进程使用的虚拟内存。
* RES：驻留内存大小。驻留内存是任务使用的非交换物理内存大小。
* SHR：SHR是进程使用的共享内存。
* S ：这个是进程的状态。它有以下不同的值:
  * D - 不可中断的睡眠态。
  * R – 运行态
  * S – 睡眠态
  * T – 被跟踪或已停止
  * Z – 僵尸态
* %CPU：自从上一次更新时到现在任务所使用的CPU时间百分比。
* %MEM：进程使用的可用物理内存百分比。
* TIME+：任务启动后到现在所使用的全部CPU时间，精确到百分之一秒。
* COMMAND：运行进程所使用的命令。 

#### 下面列出一些常用的 top命令操作指令

* q：退出top命令
* \<Space>：立即刷新
* s：设置刷新时间间隔
* c：显示命令完全模式
* t:：显示或隐藏进程和CPU状态信息
* m：显示或隐藏内存状态信息
* l：显示或隐藏uptime信息
* f：增加或减少进程显示标志
* S：累计模式，会把已完成或退出的子进程占用的CPU时间累计到父进程的MITE+
* P：按%CPU使用率排行
* T：按MITE+排行
* M：按%MEM排行
* u：指定显示用户进程
* r：修改进程renice值
* kkill：进程
* i：只显示正在运行的进程
* W：保存对top的设置到文件^/.toprc，下次启动将自动调用toprc文件的设置。
* h：帮助命令。
* q：退出
* 注：强调一下，使用频率最高的是P、T、M，因为通常使用top，我们就想看看是哪些进程最耗cpu资源、占用的内存最多； 注：通过”shift + >”或”shift + \<”可以向右或左改变排序列 如果只需要查看内存：可用free命令。只查看uptime信息（第一行），可用uptime命令；

&emsp;&emsp;从以上的介绍我们可以清楚的指导top命令其实可以干很多的事情，比如CPU信息，比如中断信息等等，当然我们也可以使用free和vmstat命令查询可用内存，那么一般我们持续观察某个进程或者相关的进程的内存使用情况，如果内存使用量持续或者螺旋震荡向上增长，那么很有可能发生了内存泄漏。调试和观察手段除了这些实际上还有很多，比如pstack、strace、ipcs、iostat、sar和netstat等等。  
&emsp;&emsp;那么

### 如何定位内存泄漏

&emsp;&emsp;Valgrind是运行在Linux上一套基于仿真技术的程序调试和分析工具，作者是获得过Google-O'Reilly开源大奖的Julian Seward，它包含一个内核──一个软件合成的CPU，和一系列的小工具，每个工具都可以完成一项任务──调试，分析，或测试等，内存检测，我们可以使用它的工具：Memcheck。

**Valgrind 安装**

* 方法 1.  valgrind官网：http://valgrind.org下载

* 方法 2.  Ubuntu  sudo apt-get install valgrind

**Memcheck检测范围**
   用来检测c/c++程序中出现的内存问题，所有对内存的读写都会被检测到，一切对malloc()/free()/new/delete的调用都会被捕获。所以，它能检测以下问题： 

1. 对未初始化内存的使用； 

2. 读/写释放后的内存块；

3. 读/写超出malloc等分配的动态内存范围； 

4. 读/写不适当的栈中内存块； 

5. 内存泄漏，指向一块内存的指针永远丢失； 

6. 不正确的malloc/free或new/delete匹配； 

7. memcpy()相关函数中的dst和src指针重叠问题。 


**Memcheck 检查步骤及注意事项**
1.在编译程序的时候打开调试模式（gcc编译器的-g选项），以便显示行号，编译时去掉-O1 -O2等优化选项;检查的是C++程序的时候，考虑加上选项： -fno-inline ,这样它函数调用链会很清晰

2. 执行：valgrind --tool=memcheck --leak-check=full --log-file=./log.txt  ./YourProgram

3.程序运行结束，查看 log.txt 中的结果。

**结果分析**
Valgrind(memcheck)包含这7类错误：

1.illegal read/illegal write errors    非法读取/非法写入错误

2.use of uninitialised values        使用未初始化的区域

3.use of uninitialised or unaddressable values in system calls 系统调用时使用了未初始化或不可寻址的地址

4.illegal frees   非法的释放

5.when a heap block is freed with an inappropriate deallocation function 分配和释放函数不匹配

6.overlapping source and destination blocks   源和目的内存块重叠

7.memory leak detection    内存泄漏检测
	

* 7.1 Still reachable 
  内存指针还在还有机会使用或者释放，指针指向的动态内存还没有被释放就退出了
* 7.2 Definitely lost 
  确定的内存泄露，已经不能够访问这块内存
* 7.3 Indirectly lost 
  指向该内存的指针都位于内存泄露处
* 7.4 Possibly lost 
  可能的内存泄露，仍然存在某个指针能够访问某块内存，但该指针指向的已经不是该内
  存首位置
* 7.5 Suppressed  某些库产生的错误不予以提示，这些错误会被统计到suppressed项目





https://zhuanlan.zhihu.com/p/458505244









