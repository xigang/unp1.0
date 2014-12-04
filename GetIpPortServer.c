#include "unp.h"
#include <time.h>

int main(int argc, char *argv[])
{
  int listenfd, connfd;
  //值-结果变量
  socklen_t len;
  //存放客户的协议地址
  struct  sockaddr_in servaddr, cliaddr;
  char buff[MAXLINE];
  time_t ticks;
  
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(13);

  Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
  
  Listen(listenfd, LISTENQ);
  
  /*接受连接并显示客户地址
   *调用inet_ntop将套接字地址结构中的32位IP地址转换为点分十进制ASCCII字符串
   *调用ntohs将16位的端口号从网络字节序转换为主机字节序
   */
  for( ; ; ){
    //初始化套接子地址结构大小
    len = sizeof(cliaddr);
    connfd = Accept(listenfd, (SA*)&cliaddr, &len);
    printf ("connection from %s, port %d\n",
	    Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
	    ntohs(cliaddr.sin_port));
    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    Write(connfd, buff, strlen(buff));
    
    Close(connfd);
  }
 
  exit(0);
}
