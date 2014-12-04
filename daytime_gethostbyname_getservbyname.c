#include "unp.h"

int main(int argc, char *argv[])
{
  int sockfd, n;
  char recvline[MAXLINE + 1];
  struct sockaddr_in servaddr;
  struct in_addr **pptr;
  struct in_addr *inetaddrp[2];
  struct in_addr inetaddr;
  struct hostent *hp;
  struct servent *sp;
  
  if(argc != 3)
    err_quit("usage: daytimetcpcli1 <hostname> <service>");

  if((hp = gethostbyname(argv[1])) == NULL){
    //inet_aton函数将一个字符串IP地址转换为一个32位的网络序列地址
    if(inet_aton(argv[1], &inetaddr) == 0){
      err_quit("hostname error for %s: %s", argv[1], 
	       hstrerror(h_errno));
    }else{
      inetaddrp[0] = &inetaddr;
      inetaddrp[1] = NULL;
      pptr = inetaddrp;
    }
  }else{
    pptr = (struct in_addr **) hp->h_addr_list;
  }
  if((sp = getservbyname(argv[2], "tcp")) == NULL)
    err_quit("getservbyname error for %s", argv[2]);
  //尝试每个服务器主机地址
  for( ; *pptr != NULL; pptr++){
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = sp->s_port;
    //memcpy将*ptr起始位置开始的 sizeof(struct in_addr)字节拷贝到内存地址servaddr中
    memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
    printf ("trying %s\n", Sock_ntop((SA *) &servaddr, sizeof(servaddr)));;
    
    if(connect(sockfd, (SA*) &servaddr, sizeof(servaddr)) == 0)
      break;
    err_ret("connect error");
    close(sockfd);
  }
  //如果循环终止的原因是没有一个connect调用成功，那就终止程序
  if(*pptr == NULL)
    err_quit("unable to connect");
  //读取服务器的应答
  while((n = Read(sockfd, recvline, MAXLINE)) > 0){
    recvline[n] = 0;
    Fputs(recvline, stdout);
  }
  exit(0);
}
