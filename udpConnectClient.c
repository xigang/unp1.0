#include "unp.h"

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
  int n;
  char sendline[MAXLINE], recvline[MAXLINE + 1];
  
  Connect(sockfd, (SA*) pservaddr, servlen);
  
  while(Fgets(sendline, MAXLINE, fp) != NULL){
    Write(sockfd, sendline, strlen(sendline));
    
    n = Read(sockfd, recvline, MAXLINE);

    recvline[n] = 0;
    Fputs(recvline, stdout);
  }
}
int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in servaddr;

  if(argc != 2)
    err_quit("usage:udpcli <IPaddress>");
  //把服务器地址填入套接字地址结构，该结构将传递给dg_cli函数，指明数据报将发往何处
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
   //创建一个UDP套接字
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
  //调用dg_cli
  dg_cli(stdin, sockfd, (SA*) &servaddr, sizeof(servaddr));

  exit(0);
}


