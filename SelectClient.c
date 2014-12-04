#include "unp.h"

void str_cli(FILE* fp, int sockfd)
{
  
  int maxfdp1;
  fd_set rset;
  char sendline[MAXLINE], recvline[MAXLINE];
  //初始化描述符集合
  FD_ZERO(&rset);
  for( ; ; ){
    //用FD_SET打开两位，一位对应标准I/O文件指针fp， 一位对应于套接字sockfd。fileno函数把标准I/O文件指针转换为对应的描述符，select and poll只工作在描述符上
    FD_SET(fileno(fp), &rset);
    FD_SET(sockfd, &rset);
    maxfdp1 = max(fileno(fp), sockfd) + 1;
    Select(maxfdp1, &rset, NULL, NULL, NULL);
    //如果在select返回时套接字是可读的，那就先用readline读入回设文本行
    if(FD_ISSET(sockfd, &rset)){
      if(Readline(sockfd, recvline, MAXLINE))
	err_quit("str_cli: server terminated prematurely");
    }
    if(FD_ISSET(fileno(fp), &rset)){
      if(Fgets(sendline, MAXLINE, fp) == NULL)
	return;
      Writen(sockfd, sendline, strlen(sendline));
    }
  }
}

int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in servaddr;
  
  if(argc != 2)
    err_quit("usage:tcpcli<IPaddress>");
  //创建套接字，装填网际网套接字地址结构
  sockfd = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  //指定服务器的众所周知的端口
  servaddr.sin_port = htons(SERV_PORT);
  
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  //connect建立与服务器的连接,引起TCP的三次握手
  Connect(sockfd, (SA*) &servaddr, sizeof(servaddr));
  //完成剩余部分的客户处理工作
  str_cli(stdin, sockfd);
  //进程终止处理的部分工作是关闭所有打开的描述符，因此客户打开的套接字由内核关闭
  exit(0);
}
