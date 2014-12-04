#include "unp.h"

#define SCTP_MAXLINE 800

void sctpstr_cli(FILE *fp, int sock_fd, struct sockaddr *to, socklen_t tolen)
{
  struct sockaddr_in peeraddr;
  struct sctp_sndrcvinfo sri;
  char sendline[MAXLINE], recvline[MAXLINE];
  socklen_t len;
  int out_sz, rd_sz;
  int msg_flags;
  //初始化sri结构进入循环，客户以清零为sri的sctp_sndrcvinfo结构变量开始，直到用户键入EOF字符终止
  bzero(&sri, sizeof(sri));
  while(fgets(sendline, MAXLINE, fp) != NULL){
    //验证输入，要求输入符合[#]text格式，不符合显示一个出错信息，然后阻塞fgets
    if(sendline[0] != '['){
      printf ("Error, line must be of the from '[streamnum]text'\n");
      continue;
    }
    //转换流号
    sri.sinfo_stream = strtol(&sendline[1], NULL, 0);
    out_sz = strlen(sendline);
    //发送消息
    Sctp_sendmsg(sock_fd, sendline, out_sz,
		 to, tolen, 0 , 0, sri.sinfo_stream, 0, 0);
    //阻塞在消息的等待上，等待服务器的回射消息
    len = sizeof(peeraddr);
    rd_sz = Sctp_recvmsg(sock_fd, recvline, sizeof(recvline),
			 (SA *)&peeraddr, &len, &sri, &msg_flags);
    //显示返回的消息并循环，包括流号，流序号以及文本消息本身
    printf ("From str:%d seq:%d (assoc:0x%x):",
	    sri.sinfo_stream, sri.sinfo_ssn, (u_int)sri.sinfo_assoc_id);
    printf ("%.*s",rd_sz, recvline);
  }
}

//用SCTP把头端阻塞减少到最小情况
void sctpstr_cli_echoall(FILE *fp, int sock_fd, struct sockaddr *to,
			 socklen_t tolen)
{
  struct sockaddr_in peeraddr;
  struct sctp_sndrcvinfo sri;
  char  sendline[MAXLINE], recvline[MAXLINE];
  socklen_t len;
  int rd_sz, strsz, i;
  int msg_flags;
  //初始化数据结构并等待输入，客户初始化用于建立各个流的sri结构，客户的发送和接收将通过这些流进行，客户还清零用于收集用户输入的数据缓冲区
  bzero(sendline, sizeof(sendline));
  bzero(&sri, sizeof(sri));
  while(fgets(sendline, SCTP_MAXLINE - 9, fp) != NULL){
    //客户设置消息的大小后删除缓冲区末尾的换行符
    strsz = strlen(sendline);
    if(sendline[strsz - 1] == '\n'){
      sendline[strsz - 1] = '\0';
      strsz--;
    }
    //发送消息到每个流
    for(i = 0; i < SERV_MAX_SCTP_STRM; i++){
      snprintf(sendline + strsz, sizeof(sendline) - strsz,".msg.%d", i);
      Sctp_sendmsg(sock_fd, sendline, sizeof(sendline), 
		   to, tolen, 0, 0, i, 0, 0);
    }
    //读回回射的消息并显示
    for(i = 0; i < SERV_MAX_SCTP_STRM; i++){
      len = sizeof(peeraddr);
      rd_sz = Sctp_recvmsg(sock_fd, recvline, sizeof(recvline),
			   (SA*)&peeraddr, &len, &sri, &msg_flags);
      printf ("From str:%d seq %d (assoc:0x%x):",
	      sri.sinfo_stream, sri.sinfo_ssn,
	      (u_int)sri.sinfo_assoc_id);
      printf ("%.*s\n",rd_sz, recvline);
    }
  }
}

int main(int argc, char *argv[])
{
  int sock_fd;
  struct sockaddr_in servaddr;
  struct sctp_event_subscribe evnts;
  int echo_to_all = 0;
  //验证参数并创建一个套接字
  if(argc < 2)
    err_quit("Missing host argument -use '%s host [echo] '\n'", argv[0]);
  if(argc > 2){
    printf ("Echoing messages to all streams\n");
    echo_to_all = 1;
  }
  //客户创建一个SCTP一到多式套接字
  sock_fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  //设置服务器地址，inet_pton函数把传递到服务器地址从表达式转换为数值隔格式，与端口合成目的地址
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
  //预订感兴趣的通知
  bzero(&evnts, sizeof(evnts));
  evnts.sctp_data_io_event = 1;
  Setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts));
  //调用回射处理函数，如果设置echo_to_all标志，客户就调用sctp_cli函数，否则调用sctp_cli_echoall
  if(echo_to_all == 0)
    sctpstr_cli(stdin, sock_fd, (SA *)&servaddr, sizeof(servaddr));
  else
    sctpstr_cli_echoall(stdin, sock_fd, (SA *)&servaddr, sizeof(servaddr));
  //客户端预先中止关联的改动部分,关闭套接字前终止关联，MAG_ABORT标志调用sctp_sendmsg函数。该标志导致发送一个ABORT块，从而立即终止当前关联。
  strcpy(byemsg, "googbye");
  Sctp_sendmsg(sock_fd, byemsg, strlen(byemsg), 
	  (SA * )&servaddr, sizeof(servaddr), 0, MAG|ABORT, 0, 0, 0);
  //结束处理
  Close(sock_fd);
  return 0;
}








