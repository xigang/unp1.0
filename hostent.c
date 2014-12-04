#include "unp.h"
#include <netdb.h>
extern int h_errno;

int main(int argc, char *argv[])
{
  char *ptr, **pptr;
  char str[INET_ADDRSTRLEN];
  struct hostent *hptr;
  //给每个命令行参数调用gethostbyname函数
  while(--argc > 0){
    ptr = *++argv;
    if((hptr = gethostbyname(ptr)) == NULL){
      err_msg("gethostbyname error for host: %s: %s", 
	      ptr, hstrerror(h_errno));
      continue;
    }
    //输出规范的主机名，后跟别名列表
     printf ("official hostname: %s\n",hptr->h_name);     
     for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
       printf ("\talias: %s\n", *pptr);
     //pptr指向一个指针数组，其中每个指针指向一个地址。，对于每个地址调用inet_ntop函数并输出返回的字符串
     switch(hptr->h_addrtype){
     case AF_INET:
       pptr = hptr->h_addr_list;
       for( ; *pptr != NULL; pptr++)
	 printf ("\taddress: %s\n",
		 Inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
       break;
       
     default:
       err_ret("unknown address type");
       break;
     }

  }
  exit(0);
}
