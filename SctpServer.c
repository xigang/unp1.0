#include "unp.h"

int main(int argc, char *argv[])
{
  int sock_fd, msg_flags;
  char readbuf[BUFFSIZE];
  struct sockaddr_in servaddr, cliaddr;
  struct sctp_sndrcvinfo sri;
  struct sctp_event_subscribe evnts;
  int stream_increment = 1;
  socklen_t len;
  size_t rd_sz;
  //默认情况下服务器响应所有的流号是在其上接收消息的流号加1
  if(argc == 2)
    stream_increment = atoi(argv[1]);
  //创建一个SCTP套接字
  sock_fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
  //捆绑一个地址，在待捆绑到该套接字地址结构中填入通陪地址(INADDR_ANY)和服务器的众所周知端口
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  
  Bind(sock_fd, (SA *) &servaddr, sizeof(servaddr));
  //预订感兴趣的通知，服务器修改一到多SCTP套接字的通知预订，服务器查看stcp_sndrcvinfo结构，服务器可从该结构确定消息到达所在的流号
  bzero(&evnts, sizeof(evnts));
  evnts.stcp_data_io_event = 1;
  Setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts));
  //开启外来关联
  Listen(sock_fd, LISTENQ);
  for( ; ; ){
    //服务器初始化客户套接字结构的大小，然后阻塞在等待来自任何一个远程对端的消息之上
    len = sizeof(struct sockaddr_in);
    rd_sz = Sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf), 
			 (SA *)&cliaddr, &len, &sri, &msg_flags);
    //当消息到达时，服务器检查stream_incrment标志变量以确定是否需要增长流号
    if(stream_increment){
      sri.sinfo_stream++;
      //如果流号増长到大于等于最大流号，服务器流号置1；
      if(sri.sinfo_stream >= sctp_get_no_strms(sock_fd, (SA *)&cliaddr, len))
	sri.sinfo_stream = 0;
    }
    //服务器使用来自sri结构的净荷协议ID，标志， 流号发送回消息本身,MSG_EOF标志促使服务器咋应答消息被客户成功确认之后关闭关联
    Sctp_sendmsg(sock_fd, readbuf, rd_sz, (SA*)&cliaddr, len, sri.sinfo_ppid,
		 (sri.sinfo_flags | MSG_EOF), sri.sinfo_stream, 0, 0);
  }
}
