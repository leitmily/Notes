# linux下C/C++编程导入第三方库文件的几种方法

参考资料：

* [linux下添加动态链接库路径的方法](https://blog.csdn.net/zxh2075/article/details/54629318 'linux下添加动态链接库路径的方法')
* [linux添加头文件路径](https://blog.csdn.net/yanshaodan/article/details/71576853 'linux添加头文件路径')
* [pkg-config 详解](https://blog.csdn.net/newchenxf/article/details/51750239 'pkg-config 详解')

## 1. 添加编译选项

在编译的时候添加库路径以及头文件路径选项，如

* -I 头文件所在的路径
* -L 库文件所在的路径（包括静态库和动态库）
* -l 指定库文件

```shell
    g++ -c main.c -I./inc -L./lib
```

## 2. 修改系统环境变量

将头文件路径和库文件路径添加到指定的系统环境变量中去，具体如下：

* 使用 **gcc** 编译时将 `头文件` 路径添加到 **C_INCLUDE_PATH** 系统环境变量中；  
* 使用 **g++** 编译时将 `头文件` 路径添加到 **CPLUS_INCLUDE_PATH** 系统环境变量中；  
* 将 `动态连接库` 路径添加到 **LD_LIBRARY_PATH** 系统环境变量中；
* 将 `静态库` 路径添加到 **LIBRARY_PATH** 系统变量中。

改变系统变量主要有两种形式，一种是临时改变，另一种是永久改变。

临时改变系统变量只需要使用 **export** 命令，重启终端后将恢复至先前状态，如

```shell
    export C_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/myinclude
```

而永久改变又可分为仅改变当前用户和改变所有用户：

* 仅改变当前用户的系统环境时，只需将上述的 **export** 语句添加到 `~/.bashrc` 或者 `~/.bash_profile` 文件内容后面，重启终端即可。
* 要改变所有用户的环境变量时，需将上述的的 **export** 语句添加到 `/etc/profile` 文件内容，并需要重启计算机。

## 3. 使用 pkg-config 命令

部分第三方库在安装完成后会自动生成一个 \*.pc 的配置文件用来存放第三方库安装的路径，包括头文件路径以及库文件路径，可以使用 **pkg-config** 命令查看，如：

```shell
    pkg-config --cflags --libs protobuf

    >>> -pthread -I/usr/local/include -L/usr/local/lib -lprotobuf -pthread
```

其中：

* --cflags 返回头文件目录
* --libs 返回库所在的目录以及库参数
* protobuf 为第三方库名

在编译包含这些库的代码时，需要执行上述命令获取返回结果，比如：

```shell
    g++ -c main.c `pkg-config --cflags --libs protobuf`
```

其中，`符号为数字1键左边的那键。

对于某些三方库没有*.pc文件，我们也可以自己写一个配置文件。\*.pc文件的路径存放在 **PKG_CONFIG_PATH** 系统变量下，使用 **echo** 命令可查看其变量。

```shell
    echo $PKG_CONFIG_PATH

    >>> :/usr/local/lib/pkgconfig
```

进入该路径，我们可以看到 \*.pc 的配置文件，用文本编辑器打开其中一个如protobuf.pc，其内容如下：

```shell
prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: Protocol Buffers
Description: Google's Data Interchange Format
Version: 3.5.2
Libs: -L${libdir} -lprotobuf -pthread
Libs.private: -lz
Cflags: -I${includedir} -pthread
Conflicts: protobuf-lite
```

其中：

* libdir 为库所在目录
* includedir 为头文件所在目录
* Libs 为调用参数，其内容为库所在的目录以及库参数
* cflags 为调用参数，其内容为头文件目录

因此，我们可以照着上面的形式自己写一个\*.pc文件放到 **PKG_CONFIG_PATH** 所指定的目录下。