---
title: GDB 基本用法，以及多进程、多线程的调试
---

该内容主要是把几个参考的博客内容进行整理和补充，并采用markdown语法对原博客内容重写

<!-- TOC -->

- [一、GDB 调试基本知识](#一gdb-调试基本知识)
- [二、GDB 基本命令清单查询](#二gdb-基本命令清单查询)
- [三、GDB 基本命令用法](#三gdb-基本命令用法)
    - [1. 运行和退出](#1-运行和退出)
    - [2. 查看信息](#2-查看信息)
    - [3. 暂停执行](#3-暂停执行)
        - [设置断点](#设置断点)
        - [断点的管理](#断点的管理)
        - [为断点设置命令列表](#为断点设置命令列表)
        - [监视点](#监视点)
        - [捕捉点](#捕捉点)
    - [4. 恢复执行](#4-恢复执行)
    - [5. 变量赋值](#5-变量赋值)
    - [6. 函数调用](#6-函数调用)
    - [7. 栈信息](#7-栈信息)
    - [8. 信号](#8-信号)
    - [9. 字段搜索](#9-字段搜索)
    - [10. UNIX接口](#10-unix接口)
    - [11. 工作目录](#11-工作目录)
    - [12. 机器语言工具](#12-机器语言工具)
    - [13. 命令的历史](#13-命令的历史)
    - [14. 查看机器码](#14-查看机器码)
- [四、多进程调试](#四多进程调试)
    - [1. 使用调试器选项 follow-fork-mode 和 detach-on-fork](#1-使用调试器选项-follow-fork-mode-和-detach-on-fork)
    - [2. 使用 attach 命令](#2-使用-attach-命令)
- [五、多线程调试](#五多线程调试)
- [参考资料：](#参考资料)

<!-- /TOC -->

# 一、GDB 调试基本知识

1. 调试器指示的是将要执行的代码行
2. 只有在编译时拥有调试符号(-g)的程序才能在调试时看到源码
3. 同一行上有多个断点时，gdb仅中断在断点号最小的那个断点上
4. 断点可以设置在同一程序的不同文件中
5. 在任何给定时间，gdb只有一个焦点，即当前“活动”的文件
6. 源文件改变后，断点发生移动，带式断点属性的行号不变

# 二、GDB 基本命令清单查询

在 gdb 提示符处键入help，将列出命令的分类，主要的分类有： 

* aliases：命令别名
* breakpoints：断点定义
* data：数据查看
* files：指定并查看文件
* internals：维护命令
* running：程序执行
* stack：调用栈查看
* statu：状态查看
* tracepoints：跟踪程序执行

键入 help 后跟命令的分类名（如help aliases），可获得该类命令的详细清单。

# 三、GDB 基本命令用法

## 1. 运行和退出

* **run（简写r）：执行程序**
    ```shell
    (gdb) run app [argv1] [argv2] ...
    ```
    run命令后可跟随发给该程序的任何参数，包括标准输入和标准输出说明符(<和>)和外壳通配符（*、？、[、]）在内。  
    如果使用不带参数的run命令，gdb就再次使用前一条run命令的参数。

* **set args：设定传递给程序的参数**
    ```shell
    (gdb) set args [argv1] [argv2] ...
    ```
* **show args：查看其缺省参数的列表**
    ```shell
    (gdb) show follow-fork-mode     # 显示参数 follow-fork-mode 选项的值
    ```
* **kill（简写k）：异常终止在gdb 控制下运行的程序**
    ```shell
    (gdb) kill
    ```
* **quit（简写q）：退出gdb**
    ```shell
    (gdb) quit
    ```

## 2. 查看信息

* **list（简写l）：查看源码**

    ```shell
    (gdb) list line_num
    ```
    或
    ```shell
    (gdb) l line_num
    ```
    其中line_num 为行号。

    查看指定文件源码：
     ```shell
    (gdb) list file_name:line_num
    ```

* **pirnt(简写p)：**

    print var：查看变量var的值。
    ```shell
    (gdb) print var
    ```
    可以通过添加参数来设置输出格式：
    > /x 按十六进制格式显示变量  
    > /d 按十进制格式显示变量  
    > /u 按十六进制格式显示无符号整型  
    > /o 按八进制格式显示变量  
    > /t 按二进制格式显示变量  
    > /a 按十六进制格式显示变量  
    > /c 按字符格式显示变量  
    > /f 按浮点数格式显示变量

    例如：print /x var

    print可以显示被调试的语言中任何有效的表达式。表达式除了包含程序中的变量外，还可以包含以下内容：  
    a). 对程序中函数的调用
    ```shell
    (gdb) print find_entry(1,0)
    ```
    b).数据结构和其他复杂对象

    ```shell
    (gdb) print *table_start

    >>> 8={e=reference=’\000’,location=0x0,next=0x0}
    ```

    c).值的历史成分
    ```shell
    (gdb) print $1  # $1为历史记录变量，在以后可以直接引用 $1 的值
    ```
    d).人为数组

    人为数组提供了一种去显示存储器块（数组节或动态分配的存储区）内容的方法。早期的调试程序没有很好的方法将任意的指针换成一个数组。就像对待参数一样，让我们查看内存中在变量h后面的10个整数，一个动态数组的语法如下所示：  

    base@length  

    因此，要想显示在h后面的10个元素，可以使用h@10：

    ```shell
    (gdb) print h@10

    >>> $13=(-1,345,23,-234,0,0,0,98,345,10)
    ```

    e).给变量赋值  

    print除了显示一个变量的值外，还可以用来赋值，如：
    ```shell
    (gdb) print var=1
    ```

* info（简写i）：

    info break：显示断点信息，下面断点部分详述。
    ```shell
    (gdb) info break
    ```
    info local：显示当前函数中的局部变量信息。
    ```shell
    (gdb) info local
    ```
    info var：显示所有的全局和静态变量名称。
    ```shell
    (gdb) info var
    ```
    info func：显示所有的函数名称。
    ```shell
    (gdb) info func
    ```
    info prog：显示被调试程序的执行状态。
    ```shell
    (gdb) info prog
    ```
    info files：显示被调试文件的详细信息。
    ```shell
    (gdb) info files
    ```
    info watchpoints: 显示所有观测点
    ```shell
    (gdb) info watchpoints
    ```

    whatis：显示变量的类型  
    如程序中定义struct timeval var；
    ```shell
    (gdb) whatis var

    >>> type = struct timeval
    ```
  
    ptype：比whatis的功能更强，它可以提供一个结构的定义
    ```shell
    (gdb) ptype var

    >>> type = struct timeval{
    __time_t tv_sec;
    __suseconds_t tv_usec;
    }
    ```

* examine: 查看内存中的值

    命令的格式为：
    ```shell
    (gdb) x[/p] addr
    ```
    其中：  
    > addr： 内存地址，  
    > p： 可选参数，可以为 n、f 和u，其中：
    >> n： 为正整数，表示需要显示 n 个地址的内容  
    >> f： 控制显示格式，包括:
    >>> s：以字符串形式显示  
    >>> i：以指令格式显示
    >>
    >> u： 表示从当前地址往后请求的字节数，默认为4自己，包括：
    >>> b： 单字节  
    >>> h： 双字节  
    >>> w： 四字节  
    >>> g： 八字节

## 3. 暂停执行

### 设置断点

* break（简写b）：在调试的程序中设置断点

    该命令有如下四种形式：
    ```shell
    (gdb) break line_number     # 使程序在执行给定行之前停止。
    (gdb) break function_name   # 使程序在进入指定的函数之前停止。
    (gdb) break line-or-function if condition    # 如果condition（条件）是真，程序到达指定行或函数时停止。
    (gdb) break routine-name    # 在指定例程的入口处设置断点
    (gdb) break *address        # 在程序的运行地址处停止
    (gdb) break                 # 没有参数，则在下一句停止
    ```
    多源文件中设置断点：
    ```shell
    (gdb) break filename:line-number
    (gdb) break filename:function-name
    ```

* break if：条件断点，满足特定条件后才会中断
    ```shell
    (gdb) break line-or-function if (condition)
    ```
    例如：
    ```shell
    (gdb) break 46 if testsize==100
    ```

* break thread：线程中断
    ```shell
    (gdb) break [line_number] thread [thread_number] [if ...]
    ```
    其中： line_number：断点设置所在代码行号  
    thread_number：线程的ID。由GDB分配，通过info threads查看运行中程序的线程信息。
    if ...：设置中断条件。

* condition（简写cond）：与break if 类似，只是condition只能用在已存在的断点上
    ```shell
    (gdb) condition <break_list> (condition)
    ```
    例如：
    ```shell
    (gdb) cond 3 i==3
    ```
    将在断点3上附加条件(i==3)  
    tbreak（简写tb）：临时断点，中断一次后自动删除断点，形式参照break

### 断点的管理

* 显示当前gdb的断点信息info(简写i)：
    ```shell
    (gdb) info break
    ```
    或
    ```shell
    (gdb) i b
    ```
    它会以如下的形式显示所有的断点信息：

    | Num   | Type       | Disp  | Enb   | Address    | What                           |
    | :---: | :--------: | :---: | :---: | :--------: | :----------------------------: |
    | 1     | breakpoint | keep  | y     | 0x000028bc | in init_random at qsort2.c:155 |
    | 2     | breakpoint | keep  | y     | 0x0000291c | in init_organ at qsort2.c:168  |

    其中：  
    > Num：断点号  
    > Type：断点类型（断点、监视点、捕获点）  
    > Disp：断点部署（该断点下次中断后该如何处理该断点）  
    > keep：保持不变  
    > del： 删除该断点  
    > dis： 禁用该断点  
    > Enb：断点当前启用状态  
    > What：断点位置  

* 删除断点 delete (简写 d )：
    ```shell
    (gdb) delete breakpoint 1
    ```
    删除编号为 1 的断点，如果不带编号参数，将删除所有的断点
    ```shell
    (gdb) delete breakpoint
    ```

* 禁止使用某个断点
    ```shell
    (gdb) disable breakpoint 1
    ```
    禁止断点 1，断点信息的 (Enb)域将变为 n

* 允许使用某个断点
    ```shell
    (gdb) enable breakpoint 1
    ```
    允许断点 1，断点信息的 (Enb)域将变为 y
    ```shell
    (gdb) enable once breakpoint 1
    ```
    断点1中断一次后自动禁用

* 删除刚才停止处的断点
    ```shell
    (gdb) clear
    ```

* 清除某一代码行上的所有断点
    ```shell
    (gdb) clear [filename:]line_number
    ```

* 清除某一函数的断点
    ```shell
    (gdb) clear [filename:]function_name
    ```

* 移动断点

    gdb中移动断点的唯一方法是删除断点后在新的位置再放置一个一模一样的断点，但ddd中只需要拖曳断点符号就可以移动新的断点，并且保留该断点的所有属性。

### 为断点设置命令列表

通常，设置一个断点并且在上面中断后，一般会查询一些变量或做一些其他动作。使用命令列表(commands)就能使程序到达断点后自动执行这些动作。

步骤如下：

1. 建立断点
2. 使用commands命令，用法(gdb)command break_list，如：
    ```shell
    (gdb) commands 1

    Type commands for when breakpoint 1 is hit, one per line.
    End with a line saying just "end".

    >silent
    >printf "n = %d\n", n
    >continue
    >end
    ```

### 监视点

* display: 监视某个变量，并且每次都停下都显示该变量值
    ```shell
    (gdb) display i
    ```
* watch expr：监视变量的变化达到条件时停止程序执行
    ```shell
    (gdb) watch i>99
    ```
* rwatch expr: 当表达式expr被读时，停止程序
    ```shell
    (gdb) rwatch i
    ```
* awatch expr: 当表达式的值被读或写时，停止程序
    ```shell
    (gdb) awatch i
    ```
    `注意：监视点的设定不依赖于断点的位置，但是与变量的作用域有关。也就是说，要设置监视点必须在程序运行时才可设置。`

    在循环中我们也可以使用 watch ，配合 ignore ，它是除了 until 命令之外又一个可以让我们跳出循环的方法，`不过 watch + ignore 更强大，可以任意跳转到第 i 次循环`。它们的意思就是观察一个变量，可以理解为断点，ignore 这个断点多少次，然后用 continue 就可以直接跳过了。
    ```shell
    (gdb) watch i
    >>> Hardware watchpoint 3: i
    (gdb) ignore 3 15   # 忽略第三个观测点15次
    >>> Will ignore next 15 crossings of breakpoint 3.
    ```

### 捕捉点

* catch event：当event发生时，停止执行程序
    event可为以下情况：
    > throw：一个C++抛出的异常（throw为关键字）  
    > catch：一个C++捕捉到的异常(catch为关键字)  
    > exec：调用系统调用exec时（exec为关键字，目前此功能只在HP-UX下有用）  
    > fork：调用系统调用fork时（fork为关键字，目前此功能只在HP-UX下有用）  
    > vfork：调用系统调用vfork时（vfork为关键字，目前此功能只在HP-UX下有用）  
    > load 或 load 载入共享库（动态链接库）时（load为关键字，目前此功能只在HP-UX下有用）  
    > unload 或 unload 卸载共享库（动态链接库）时（unload为关键字，目前此功能只在HP-UX下有用）

## 4. 恢复执行

* next（简写n）：不进入的单步执行，且执行n步
    ```shell
    (gdb) next [n]
    ```
* step（简写s）：进入的单步执行，且执行n步
    ```shell
    (gdb) step [n]
    ```
* continue（简写c）：从断点继续运行
    ```shell
    (gdb) continue [n]
    ```
    参数n使gdb忽略接下来的n个断点。

* finish：程序已经进入了某函数，执行finish退出该函数返回到它的调用函数中
    ```shell
    (gdb) finish
    ```
* until：无参的until命令恢复程序的运行，并执行完循环的其余部分

    until官方定义：执行程序，直到到达当前循环体外的下一行源代码  
    实际上，由于高级语言到机器语言在顺序上并不一定完全对应，until在到达内存地址比当前高的机器指令后就会停止  
    用法：
    ```shell
    until [filename:]line_number
    until [filename:]function
    ```
    以上两种用法在到达指定的行号或者函数后即停止。

## 5. 变量赋值

除了使用 print 给变量赋值，还可以使用 set variable 命令赋值。

* set variable：给变量赋值
    ```shell
    (gdb) set variable i=1
    ```
* set env environment Varname=value ： 设置环境变量
    ```shell
    (gdb) set env USER=benben
    ```

## 6. 函数调用

* call func_name：调用和执行一个函数
    ```shell
    (gdb) call gen_and_sork(1234, 1, 0)
    (gdb) call printf(“abcd\n”)

    >>> abcd
    >>> $1=4
    ```
* jump：在源程序中的另一点开始运行
    ```shell
    (gdb) jump line_number
    ```
    line_number 可以是 linenum，filename+linenum，+ offset 这几种形式
    ```shell
    (gdb) jump address  # 跳到代码行的地址
    ```

## 7. 栈信息

* backtrace（简写bt）：为堆栈提供向后跟踪功能
    ```shell
    (gdb) backtrace [-n] [n]  # 显示程序中的当前位置和表示如何到达当前位置的栈跟踪。
    ```
    其中：
    > -n：表示只打印栈底上n层的栈信息  
    > n：表示只打印栈顶上n层的栈信息

    不加参数，表示打印所有栈信息。
    ```shell
    (gdb) down  # 下移栈帧，使得另一个函数成为当前函数
    (gdb) up    # 上移栈帧，使另一函数成为当前函数
    ```

## 8. 信号

* handle signals [argu]：捕捉信号

    signals：是Linux/Unix定义的信号，如SIGINT表示中断字符信号，也就是Ctrl+C的信号，SIGBUS表示硬件故障的信号等等。

    argu：
    > nostop：接收到信号时，GDB不会停止程序的运行，但会打出消息通知收到这种信号。  
    > stop：接受到信号时停止程序的执行，允许程序调试；显示一条表示已接受到信号的消息（禁止使用消息除外）。  
    > print：接受到信号时显示一条消息。  
    > noprint：接受到信号时不要显示消息。  
    > pass or noignore：接收到信号时，GDB不处理信号。这表示，GDB会把这个信号交给被调试程序处理。  
    > nopass or ignore：接收到信号时，GDB不会让调试程序来处理这个信号。

    例如，假定截获SIGPIPE信号，以防止正在调试的程序接受到该信号，而且只要该信号一到达，就要求该程序停止，并通知你。要完成这一任务，可利用如下命令：
    ```shell
    (gdb) handle SIGPIPE stop print
    ```
    `UNIX的信号名总是采用大写字母`。可以用信号编号替代信号名。  

    如果程序要测试其信号处理程序，就需要一种能将信号发送给程序的简便方法：signal命令。  
    signal命令的参数是一个数字或者一个名字，如SIGINT。假定程序已将一个专用的SIGINT（键盘输入，或CTRL-C；信号2）信号处理程序设置成某个动作，要测试该信号处理程序，可设置一个断点并使用如下命令：
    ```shell
    (gdb) signal 2

    >>> continuing with signal SIGINT(2)
    ```
    该程序继续执行，但是立即传输该信号，而且处理程序开始运行。

## 9. 字段搜索

* search text：至上往下搜索，显示在当前文件中包含text的代码行

* reverse-search text：至下往上搜索，显示包含text的代码行

## 10. UNIX接口

* shell：使可不用离开gdb下启动unix外壳，即暂时退出gdb调试回到用户控制终端
    ```sehll
    (gdb) shell
    ```
* exit（快捷键ctrl+d）：退出unix外壳，返回到 gdb
    ```shell
    exit
    ```

## 11. 工作目录

* cd：改变工作目录
    ```shell
    (gdb) cd [path]
    ```
* pwd：显示工作目录
    ```shell
    (gdb) pwd
    ```

## 12. 机器语言工具

有一组专用的gdb变量可以用来检查和修改计算机的通用寄存器，gdb提供了目前每一台计算机中实际使用的4个寄存器的标准名字：

* $pc ： 程序计数器
* $fp ： 帧指针（当前堆栈帧）
* $sp ： 栈指针
* $ps ： 处理器状态

## 13. 命令的历史

为了允许使用历史命令，可使用 set history expansion on 命令

```shell
(gdb) set history expansion on
```

## 14. 查看机器码

* disassemble： 查看执行时源代码的机器码

    ```shell
    (gdb) disassemble b.func

    >>>
    0x00005555555549b8 <+0>:    push   %rbp
    0x00005555555549b9 <+1>:    mov    %rsp,%rbp
    0x00005555555549bc <+4>:    sub    $0x20,%rsp
    0x00005555555549c0 <+8>:    mov    %rdi,-0x18(%rbp)
    0x00005555555549c4 <+12>:   mov    %esi,-0x1c(%rbp)
    0x00005555555549c7 <+15>:   movl   $0x0,-0x4(%rbp)
    0x00005555555549ce <+22>:   mov    -0x4(%rbp),%eax
    0x00005555555549d1 <+25>:   cmp    -0x1c(%rbp),%eax
    ...
    ```

# 四、多进程调试

在进行多进程调试的时候，我们在 fork() 函数后的父进程与子进程代码段内分别设有断点并运行调试的时候会发现，父进程能够在断点处正常停止，而子进程会忽略断点继续运行，因此需要一定的手段调试多进程程序。

## 1. 使用调试器选项 follow-fork-mode 和 detach-on-fork

* follow-fork-mode 选项

    用于选择调试指定进程，但只能同时调试其中一个进程，另一个进程会忽略断点继续运行。命令格式为：  
    ```shell
    set follow-fork-mode mode
    ```
    其中 mode 的可选值是：
    > parent： 仅调试父进程，默认值  
    > child： 仅调试子进程

    ```shell
    (gdb) set follow-fork-mode child
    ```
    此时 gdb 将调试子进程，并忽略父进程的断点。若想同时调试父/子进程，则需要使用 detach-on-fork 选项。

* detach-on-fork 选项
  
    指示 GDB 在 fork 之后是否断开（detach）某个进程的调试，或者都交由 GDB 控制。命令格式为：
    ```shell
    (gdb) set detach-on-fork mode
    ```
    其中 mode 的可选值是：
    > off： 断开（可同时调试）  
    > on： 断开（不能同时调试），默认值  

    当 mode 为 off 时，其中某个进程将会`阻塞`在 fork 位置，具体如下表所示：

    | follow-fork-mode | detach-on-fork | 说明                                                 |
    | :--------------: | :------------: | :--------------------------------------------------: |
    | parent           | on             | 只调试父进程（GDB默认）                              |
    | child            | on             | 只调试子进程                                         |
    | parent           | off            | 同时调试两个进程，gdb 调试父进程，子进程 block 在 fork 位置 |
    | child            | off            | 同时调试两个进程，gdb 调试子进程，父进程 block 在 fork 位置 |

    即，若要同时调试两个进程，可以进行如下设置：
    ```shell
    (gdb) set follow-fork-mode child    # 调试子进程
    (gdb) set detach-on-fork off        # 关闭进程分离，同时调试父进程（父进程默认阻塞在 fork 函数）
    (gdb) b 18                          # 在子进程代码区设断点
    (gdb) r
    ```
    `注`：该方法并不适合所有 Linux， 因此需要采用其他方法。

## 2. 使用 attach 命令

众所周知，GDB 有附着（attach）到正在运行的进程的功能，即 attach \< pid \> 命令。因此我们可以利用该命令 attach 到子进程然后进行调试。

首先我们需要查看运行程序的进程ID，使用如下命令:

```shell
ps -ef | grep test
```

查找到你运行的程序的PID，并在 gdb 下使用 attach 命令可以附着在该进程下：

```shell
(gdb) attach PID  # 其中，PID是要调试的进程ID
```

被成功附着的进程将直接暂停执行，此时你可以正常来调试。

脱离进程使用如下命令：

```shell
(gdb) detach
```

`注`：部分 Linux 需要在`超级用户`下运行 gdb 才能 attach 成功。

一个新的问题是，**`子进程一直在运行，attach上去后都不知道运行到哪里了。有没有办法解决呢？`**

至于这段代码所采用的条件，看你的偏好了。比如我们可以在需要调试的进程（可以同时调试多个进程）的代码处添加个`无限循环`并配合 sleep 函数，然后配合使用 jump、next 命令或者其他修改变量的命令如 set 改变循环条件退出循环。以 jump 为例，其形式如下：

```cpp
while(true) {
    cout << "parent ID： " << getpid() << endl;
    sleep(1);
}
... // 其他代码，使用 jump 跳转至此
```

# 五、多线程调试

gdb调试一般有两种模式：all-stop模式和no-stop模式（gdb7.0之前不支持no-stop模式）。

* all-stop 模式

    在这种模式下，`当你的程序在gdb由于任何原因而停止，所有的线程都会停止`，而不仅仅是当前的线程。一般来说，gdb不能单步所有的线程。因为线程调度是gdb无法控制的。`无论什么时候当gdb停止你的程序，它都会自动切换到触发断点的那个线程`。

* no-stop模式（网络编程常用）

    当程序在gdb中停止，只有当前的线程会被停止，而其他线程将会继续运行。这时候step，next这些命令就只对当前线程起作用。

如果要打开 no-stop 模式，可以进行如下设置：

```shell
#Enable the async interface
(gdb) set target-async 1
#If using the CLI, pagination breaks non-stop
(gdb) set pagination off
#Finall, turn it on
(gdb) set non-stop on
```

gdb 支持的命里有两种类型：前台的（同步的）和后台（异步 ）的。区别很简单，同步的在输出提示符之前会等待程序 report 一些线程已经终止的信息，异步则是直接返回。所以我们需要 set target-async 1。set pagination off 可以不让线程结束的时候出现 Type < return > to continue 的提示信息 。最后一步是打开。

下面是常用命令：

* info threads

    显示所有线程。

* thread id

    切换到指定线程。

* break FileName.cpp:LinuNum thread all
  
    所有线程都在文件 FileName.cpp 的第 LineNum 行有断点。

* thread apply ID1 ID2 IDN command

    让线程编号是 ID1，ID2… 等等的线程都执行 command 命令。

* thread apply all command

    所有线程都执行 command 命令。

* set scheduler-locking off|on|step：

    在调式某一个线程时，其他线程是否执行。在使用 step 或 continue 命令调试当前被调试线程的时候，其他线程也是同时执行的，如果我们只想要被调试的线程执行，而其他线程停止等待，那就要锁定要调试的线程，只让他运行。其中：

    > off：不锁定任何线程，默认值。  
    > on：锁定其他线程，只有当前线程执行。  
    > step：在 step （单步）时，只有被调试线程运行。

* set non-stop on/off

    当调式一个线程时，其他线程是否运行。

* set pagination on/off

    在使用 backtrace 时，在分页时是否停止。

* set target-async on/off

    是否开启同步和异步。其中：
    > on：同步，gdb 在输出提示符之前等待程序报告一些线程已经终止的信息。  
    > off： 异步，线程结束后则是直接返回。

* show scheduler-locking

    查看当前锁定线程的模式。

# 参考资料：

* [GDB 基本命令(整合)](https://blog.csdn.net/water_cow/article/details/7214054 "GDB基本命令(整合)")
* [GDB 常用调试命令以及多进程多线程调试](https://blog.csdn.net/freeelinux/article/details/53700266 "GDB常用调试命令以及多进程多线程调试")
* [使用 GDB 调试多进程程序](https://www.ibm.com/developerworks/cn/linux/l-cn-gdbmp/ "使用 GDB 调试多进程程序")
* [gdb调试多进程与多线程](https://blog.csdn.net/snow_5288/article/details/72982594 "gdb调试多进程与多线程")