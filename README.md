c4 - C in four functions
========================

An exercise in minimalism.

Try the following:

    gcc -o c4 c4.c
    ./c4 hello.c
    ./c4 -s hello.c
    
    ./c4 c4.c hello.c
    ./c4 c4.c c4.c hello.c

## Limitations

* 只支持`int`和`char`类型
* 不支持`unsigned`类型
* 不支持`struct`
* 不支持声明数组，例如`int a[3]`
* 全局变量定义必须放在原文件最前面，局部变量定义只能放在函数最前面
* 不支持`continue`
* 不支持`/* */`注释
* 不支持`switch`语句
* 不支持宏定义
* 不支持`#include`
* 不支持`for`语句
