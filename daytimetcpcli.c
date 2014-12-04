#include "unp.h"

//该版本的客户程序同时支持IPv4和IPv6;
int main(int argc, char *argv[])
{
  int sockfd, n;
  char recvline[MAXLINE + 1];
  socklen_t len;
  struct sockaddr_storage ss;
  //命令行参数来指定服务名或端口号，允许本程序连接到其他端口
  if(argc != 3)
    err_quit("usage: daytimetcpcli <hostname/IPaddress> <service/port#>");
  //连接到服务器
  sockfd = Tcp_connect(argv[1], argv[2]);
  //显示服务器地址
  len = sizeof(ss);
  Getpeername(sockfd, (SA *)&ss, &len);
  printf ("connected to %s\n", Sock_ntop_host((SA *)&ss, len));

  while((n = Read(sockfd, recvline, MAXLINE)) > 0){
    recvline[n] = 0;
    Fputs(recvline, stdout);
  }
  exit(0);
}
