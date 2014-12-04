#include "unp.h"
#include <time.h>

int main(int argc, char *argv[])
{  
  int listenfd, connfd;
  struct sockaddr_in servaddr;
  char buff[MAXLINE];
  time_t ticks;
  //创建TCP套接字  listenfd为监听套接字，在服务器的整个有效期内保持开放
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  //把服务器的众所周知端口捆绑到套接字
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  //通配地址由常量INADDR——ANY来指定，其值一般为0，告知内核去选择IP地址
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(13);
  //把一个本地协议地址赋予一个套接字，对于协议地址ip和TCP/UDP端口号组合
  Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
  //把套接字转换成监听套接字，这样来自客户的外来连接就可以在该套接字上由内核接受
  Listen(listenfd, LISTENQ);
  //接受客户连接，发送应答
  for ( ; ; )
    {
      /*由内核返回一个全新描述符，代表与返回客户的TCP连接，返回值为已连接套接字
      本函数最多返回三个值：一个即可能是新套接子描述符也可能是出错指示整数、客户进程协议地址
      以及该地址的大小。如果对返回客户协议地址不感兴趣，可以把他们置为空指针
      */
      connfd = Accept(listenfd, (SA*) NULL, NULL);
      ticks = time(NULL);
      snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
      Write(connfd, buff, strlen(buff));
      //终止连接
      Close(connfd);
    }
  return 0;
}
