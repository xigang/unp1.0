#include "unp.h"

void
dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int			 n;
	char			sendline[MAXLINE], recvline[MAXLINE + 1];
	socklen_t		len;
	struct sockaddr	*preply_addr;

	preply_addr = Malloc(servlen);

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

		len = servlen;
		n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
		if (len != servlen || memcmp(pservaddr, preply_addr, len) != 0) {
			printf("reply from %s (ignored)\n",
					Sock_ntop(preply_addr, len));
			continue;
		}

		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
}
int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in servaddr;

  if(argc != 2)
    err_quit("usage:udpcli <IPaddress>");
  //把服务器地址填入套接字地址结构，该结构将传递给dg_cli函数，指明数据报将发往何处
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(7);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
   //创建一个UDP套接字
  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
  //调用dg_cli
  dg_cli(stdin, sockfd, (SA*) &servaddr, sizeof(servaddr));

  exit(0);
}
