<doc/>

requirements: 需求分析、架构设计、技术路线、流程图


程序实现了一个简易的多线程HTTP server，可以处理并发的静态网页请求。程序需要在linux系统下运行，因为代码实现中使用了linux系统调用（包括文件读取、socket、创建线程等）。
程序可处理并发的http请求。对于每个请求会开启一个新的线程来响应。每个线程的处理流程如下：解析request header->寻找服务器上相应文件->向套接字发送request header和网页文件


背景知识
1.	linux下的socket与文件系统
从Unix内核的角度来看，一个套接字就是通信的一个端点。从Unix程序的角度来看，套接字就是一个有相应描述符的文件句柄。
因为套接字API最初是作为UNIX操作系统的一部分而开发的，所以套接字API与系统的其他I/O设备集成在一起。特别是，当应用程序要为因特网通信而创建一个套接字（socket）时，操作系统就返回一个小整数作为描述符（descriptor）来标识这个套接字。然后，应用程序以该描述符作为传递参数，通过调用函数来完成某种操作（例如通过网络传送数据或接收输入的数据）。

2.	linux下的多线程
POSIX线程（英语：POSIX Threads，常被縮寫為Pthreads）是POSIX的线程标准，定义了创建和操纵线程的一套API。实现POSIX 线程标准的库常被称作Pthreads，一般用于Unix-like POSIX 系统，如Linux、 Solaris。Pthreads定义了一套C语言的类型、函数与常量，它以pthread.h头文件和一个线程库实现。Pthreads API中大致共有100个函数调用，全都以"pthread_"开头。

3.	HTTP响应过程


函数表：
void doit(int fd)
	对fd代表的套接字相应http请求
void read_requesthdrs(rio_t *rp)
	读取http请求header并打印
void parse_uri(char *uri, char *filename)
	将uri转换成具体的服务器上的文件名
void serve_static(int fd, char *filename, int filesize)
	向fd代表的套接字输出http返回header和网页内容
void get_filetype(char* filename, char* filetype)
	根据filename文件名后缀判断文件类型
void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg)
	向socket输出服务器错误信息
void *thread(void *vargp)
	新建一个线程，并运行doit来启动http服务
int main()
	建立套接字监听本机的服务端口，当发现request时建立一个新套接字，并对此建立一个新线程响应请求（doit）


结构体列表：
rio_t
	Rio系列IO函数使用的缓冲区



出现在库中的函数：
为方便代码实现，程序使用了一个库csapp.h，它提供了一些封装好的常用函数和结构体。
Open_listenfd(char *port)
	打开和返回一个监听描述符，以便在port端口上监听请求
Rio_******()
	一套I/O读写函数包，用于在网络传输中提供稳定可靠的IO读写功能。


使用的linux系统调用列表：
getaddrinfo()	将IP地址/端口号转换为socket结构体链表
freeaddrinfo()	释放结构体链表
setsockopt()	
accept()		
listen()		
bind()			
close()			
socket()		
getnameinfo()	

