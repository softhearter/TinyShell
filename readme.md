# -myShell

## 简介
操作系统实验四
>使用c语言实现的简单shell，支持管道和重定向
>程序运行之后会模拟shell使用橙色的字体显示当前的用户名称，主机名称和路径，等待用户输入命令。程序逐次读取用户输入的指令后，将指令按空格拆分成多个字符串命令，然后判断该命令的类型。若命令有误，则用红色字体打印出错误信息
>学习参考自https://github.com/mufeng964497595/myshell.git

## 功能
### 版本1.0 更新2023/5/26

>- [x] 橙色输出字符显示当前用户名、主机名和工作路径
>- [x] exit命令
>- [x] cd命令
>- [x] hello命令
>- [x] help命令(带有姓名和学号)
>- [x] 简单的外部命令
>- [x] 实现输入、输出重定向(参考别人编写)
>- [x] 递归实现多重管道(参考别人编写)
>- [x] 判断命令是否存在

### 版本1.1 更新时间2023/5/27

>- [x] times命令
>- [x] dirs命令(显示当前的工作保存路径)
>- [x] ls命令
