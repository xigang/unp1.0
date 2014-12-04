#include "unp.h"

void sig_chld(int signo)
{
  pid_t pid;
  int stat;
  
  while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf ("child %d terminated\n", pid);
  return;
}
int main(int argc, char *argv[])
{
  int listenfd, connfd, udpfd, nready, maxfdp1;
  char mesg[MAXLINE];
  pid_t childpid;
  socklen_t len;
  fd_set rset;
  ssize_t n;
  const int on = 1;
  struct sockaddr_in cliaddr, servaddr;
  
  void sig_chld(int);

  //创建TCP套接字的监听程序,
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  
  Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
  
  Listen(listenfd, LISTENQ);

  //创建UDP套接字并捆绑与TCP套接字相同的端口。因为TCP端口独立于UDP端口
  udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  Bind(udpfd, (SA *) &servaddr, sizeof(servaddr));
  //给SIGCHLD建立信号处理函数，因为TCP连接由某个子进程处理
  Signal(SIGCHLD, sig_chld); //必须调用waitpid函数以免留下僵死进程
  //select初始化一个描述符集，并计算出两个描述符的较大者
  FD_ZERO(&rset);
  maxfdp1 = max(listenfd, udpfd) + 1;
  for( ; ; ){
    FD_SET(listenfd, &rset);
    FD_SET(udpfd, &rset);
    //调用select只是为了等待监听TCP套接字的可读条件或UDP套接字的可读条件
    //sig_chld信号处理函数可能中断我们对select的调用，我们于是需要处理EINTR错误
    if((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0){
      if(errno == EINTR)
	continue;
      else
	err_sys("select error");
    }
    //当监听TCP套接字可读时，accept一个新的客户连接，fork一个子进程，并在在子进程中调用str_echo回显函数。
    if(FD_ISSET(listenfd, &rset)){
      len = sizeof(cliaddr);
      connfd = Accept(listenfd, (SA *) &cliaddr, &len);
      
      if((childpid == Fork()) == 0){
	Close(listenfd);
	str_echo(connfd);
	exit(0);
      }
      Close(connfd);
    }
    //处理数据报的到达，如果UDP套接字可读，那么已有一个数据报到达，调用recvfrom读入它，在使用sendto把它发回给客户
    if(FD_ISSET(udpfd, &rset)){
      len = sizeof(cliaddr);
      n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *) &cliaddr, &len);
      
      Sendto(udpfd, mesg, n, 0, (SA *) &cliaddr, len);
    }
  }
}
