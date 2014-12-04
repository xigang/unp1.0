#include "unp.h"
//若成功返回未连接套接字描述符，出错则不返回
int Udp_client(const char *host, const char *serv, SA **saptr, socklen_t *lenp)
{
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;
  
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  //用于转化hostname,service参数
  if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    err_quit("udp_client error for %s, %s: %s", 
	     host, serv, gai_strerror(n));
  ressave = res;
  
  do{
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd >= 0)
      break;
  }while((res->ai_next) != NULL);
  
  if((res == NULL))
    err_sys("udp_lient error for %s, %s", host, serv);
  //malloc用于分配一套接字地址结构的内存空间
  *saptr = Malloc(res->ai_addrlen);
  //把对应创建的套接字的地址结构复制到这个内存空间
  memcpy(*saptr, res->ai_addr, res->ai_addrlen);
  *lenp = res->ai_addrlen;

  freeaddrinfo(ressave);

  return sockfd;
}
int main(int argc, char *argv[])
{
  int sockfd, n;
  char recvline[MAXLINE + 1];
  socklen_t salen;
  struct sockaddr *sa;
  
  if(argc != 3)
    err_quit("uage: daytimeudpcli1 <hostname/IPaddress> <service/port#>");
  //调用Udp——client函数，然后显示将向其发送UDP数据报的服务器的IP地址和端口号。
  sockfd = Udp_client(argv[1], argv[2], (void**) &sa, &salen);
  
  printf ("sending to %s\n", Sock_ntop_host(sa, salen));

  Sendto(sockfd, "", 1, 0, sa, salen);

  n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
  recvline[n] = '\0';
  Fputs(recvline, stdout);
  
  exit(0);
}
