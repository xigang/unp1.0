#include "unp.h"

void dg_echo(int sockfd, SA*pcliaddr, socklen_t clilen)
{
  int n;
  socklen_t len;
  char mesg[MAXLINE];
  
  for( ; ; ){
    len = clilen;
    //使用recvfrom读入下一个到达服务器端口的数据报，在使用sendto把它发送回发送者
    n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
    
    Sendto(sockfd, mesg, n, 0, pcliaddr, len);
  }
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
