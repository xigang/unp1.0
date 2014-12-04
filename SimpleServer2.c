#include "unp.h"

void str_echo(int sockfd)
{
  ssize_t n;
  char buf[MAXLINE];

 again:
  while((n = read(sockfd, buf, MAXLINE)) > 0)
    Writen(sockfd, buf, n);
  //EINTR  Interrupted function call (POSIX.1); see signal(7).
  if(n < 0 && errno == EINTR)
    goto again;
  else if(n < 0)
    err_sys("str_echo: read error");
}

/*信号处理函数，用于处理僵死进程,because 每次终止子进程将产生僵死进程，占用内核空间，耗费资源
  用wait在多个子进程同一时刻终止时将产生僵死进程
 */
/*void sig_chld(int signo)
{
  pid_t pid;
  int stat;
  
  pid = wait(&stat);
  printf ("child %d terminated\n",pid);
  return;
  }*/
/*调用waitpid解决同一时刻终止多个子进程的情况
 */

void sig_chld(int signo)
{
  pid_t pid;
  int stat;
  
  while((pid == waitpid(-1, &stat, WNOHANG)))
    printf ("child %d terminated\n",pid);
  return;
}
int main(int argc, char *argv[])
{
  int listenfd, connfd;
  pid_t childpid;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;
  //创建套接字，捆绑服务器的众所周知的端口
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);  
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  //SERV_PORT服务器众所周知的端口
  servaddr.sin_port = htons(SERV_PORT);
  
  Bind(listenfd, (SA*) &servaddr, sizeof(servaddr));
  
  Listen(listenfd, LISTENQ);
  Signal(SIGCHLD, sig_chld);
  for( ; ; ){
    //服务器阻塞于accept调用，等待客户连接的完成
    clilen = sizeof(cliaddr);
    //自己重启被中断的系统调用
    if( (connfd = accept(listenfd, (SA*) &cliaddr, &clilen) )< 0 ){
      if(errno == EINTR)
	continue;
      else
	err_sys("accept error");
    }
    //并发服务器，fork为每一个客户派生一个处理他们的子进程。子进程关闭监听套接字，父进程关闭已连接的套接字
    if((childpid = fork()) == 0){
      Close(listenfd);
      str_echo(connfd);
     exit(0);
    }
    Close(connfd);
  }
}
