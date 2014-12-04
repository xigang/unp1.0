#include "unp.h"
//协议无关时间获取服务器程序
int Udp_server(const char *host, const char *serv, socklen_t *addrlenp)
{
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;
  
  bzero(&hints, sizeof(hints));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    err_quit("udp_server error for %s, %s: %s", 
	     host, serv, gai_strerror(n));
  ressave = res;
  
  do{
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd < 0)
      continue;
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
      break;
    Close(sockfd);
  }while((res = res->ai_next) != NULL);
  
  if(res == NULL)
    err_sys("udp_server error for %s, %s", host, serv);
  
  if(addrlenp)
    *addrlenp = res->ai_addrlen;
  
  freeaddrinfo(ressave);
  
  return(sockfd);
}
int main(int argc, char *argv[])
{
  int sockfd;
  ssize_t n;
  char buff[MAXLINE];
  time_t ticks;
  socklen_t len;
  struct sockaddr_storage cliaddr;
  
  if(argc == 2)
    sockfd = Udp_server(NULL, argv[1], NULL);
  else if(argc == 3)
    sockfd = Udp_server(argv[1], argv[2], NULL);
  else
    err_quit("uasgeL daytimeudpsrv [<host>] <service or port>");

  for( ; ; ){
    len = sizeof(cliaddr);
    n = Recvfrom(sockfd, buff, MAXLINE, 0, (SA*)&cliaddr, &len);
    printf ("datagram from %s\n",Sock_ntop((SA*)&cliaddr, len));

    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    Sendto(sockfd, buff, strlen(buff), 0, (SA*)&cliaddr, len);
  }
}
