#include "unp.h"

static void recvfrom_int(int);
static int count;

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
  socklen_t len;
  char mesg[MAXLINE];
  
  Signal(SIGINT, recvfrom_int);

  for( ; ; ){
    len = clilen;
    Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
    
    count++;
  }
}

static void recvfrom_int(int signo)
{
  printf("\nreceived %d datagrams\n", count);
  exit(0);
}

int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;
  //创建UDP套接字，捆绑服务器的众所周知的端口
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  
  Bind(sockfd, (SA*) &servaddr, sizeof(servaddr));
  //调用函数dg_echo来执行服务器的处理工作
  dg_echo(sockfd, (SA*) &cliaddr, sizeof(cliaddr));
}
