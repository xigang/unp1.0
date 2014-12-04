#include "unp.h"

int main(int argc, char *argv[])
{
  int Listenfd, n;
  const int on = 1;
  struct addrinfo hints, *res,*ressave;

  bzero(&hints, sizeof(hintse));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    err_quit("tcp_listen error for %s, %s: %s",
	     host, serv, gai_strerror(n));
  ressave = res;
  
  do{
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(listenfd < 0)
      continue;

    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(one));
    if(bind(listenfd, res->ai_addr, res->ai_addrlen,) == 0)
      break;
    
    Close(listenfd);
  }while((res = res->ai_next) != NULL);
  
  if(res == NULL)
    err_sys("tcp_listen error for %s,%s", host, serv);
  
  Listen(listenfd, LISTENQ);
  
  if(addrlenp)
    *addrlenp = res->ai_addrlen;

  freeaddrinfo(ressave);
  
  return(listenfd);
}
