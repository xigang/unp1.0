#include "unp.h"   //包含自己编写的头文件

int main(int argc, char *argv[])
{
  int sockfd, n;
  char recvline[MAXLINE + 1];
  struct sockaddr_in servaddr;

  if(argc != 2)
    err_quit("usage: a.out <IPaddress>");
  //创建TCP套接字，socket函数创建一个网际（AF_INET）字节流(SOCK_INET)套接字，该函数返回小整数描述符。
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) , 0)
    err_sys("socket error");
  //把网际套接字地址结构清零
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  //调用htons转换二进制端口号，由主机字节顺序为网络字节顺序
  servaddr.sin_port = htons(13);
  //把命令行参数转换为合适的格式,在ASCII字符串与网络字节序的二进制值之间转换网际地址
  if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    err_quit("inet_pton error for %s, argv[1]");
  //建立与服务器的连接
  if(connect(sockfd, (SA*) &servaddr, sizeof(servaddr)) < 0)
    err_sys("connect error");
  //读入并输出与服务器的应答
  while((n = read(sockfd, recvline, MAXLINE)) > 0)
    {
      recvline[n] = 0;
      if(fputs(recvline, stdout) == EOF)
	err_sys("fputs error");
     }
  if (n < 0)
    {
      err_sys("read error");
    }
  //终止程序
  exit(0);
}
