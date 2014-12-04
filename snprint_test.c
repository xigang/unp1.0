#include <stdio.h>

int main(int argc, char *argv[])
{
  char tlist_3[10] = {0};
  int len_3 = 0;
  
  len_3 = snprintf(tlist_3, sizeof(tlist_3), "this is a overflow test!\n" );
  len_3 = sprintf(tlist_3, "this is a overflow test!\n" );
  printf ("len_3 = %d, tlist_3 = %s\n", len_3, tlist_3);
  return 0;
}
