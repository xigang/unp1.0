#include "unp.h"

void str_cli(FILE* fp, int sockfd)
{
  int maxfdp1, stdineof;
  fd_set rset;
  char buf[MAXLINE];
  int n;

  stdineof = 0;
  FD_ZERO(&rset);

  for( ; ; ){
    if(stdineof == 0)
      FD_SET(fileno(fp), sockfd);
    FD_SET(sockfd, &rset);
    maxfdp1 = max(fileno(fp), sockfd) + 1;
    Select(maxfdp1, &rset, NULL, NULL, NULL);
    
    if(FD_ISSET(sockfd, &rset)){
      if( (n = Read(sockfd, buf, MAXLINE)) == 0){
	if(stdineof == 1)
	  return;
	else
	  err_quit("str_cli :server terminated prematuely ");
       }
      Write(fileno(fp), buf, n);
    }
    if(FD_ISSET(fileno(fp), buf, MAXLINE)){
      if((n = Read(fileno(fp), buf, MAXLINE)) == 0){
	stdineof = 1;
	Shutdowm(sockfd, SHUT_WR);
	FD_CLR(fileno(fp), &rset);
	continue;
      }
      Writen(sockfd, buf, n);
    }
  }
}

int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in servaddr;
  
  if(argc != 2)
    err_quit("usage:tcpcli<IPaddress>");
  //创建套接字，装填网际网套接字地址结构
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  //指定服务器的众所周知的端口
  servaddr.sin_port = htons(SERV_PORT);
  
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  //connect建立与服务器的连接,引起TCP的三次握手
  Connect(sockfd, (SA*) &servaddr, sizeof(servaddr));
  //完成剩余部分的客户处理工作
  str_cli(stdin, sockfd);
  //进程终止处理的部分工作是关闭所有打开的描述符，因此客户打开的套接字由内核关闭
  exit(0);
}

