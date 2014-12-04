#include "unp.h"

#define NDG 2000
#define DGLEN 1400
//查看无任何流量控制的UDP对数据报传输的影响，该函数写2000个1400字节大小的UDP数据报给服务器
void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
  int i;
  char sendline[DGLEN];
  
  for(i = 0; i < NDG; i++){
    Sendto(sockfd, sendline, DGLEN, 0, pservaddr, servlen);
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

