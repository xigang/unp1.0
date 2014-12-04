#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char *s = "hello_world";
  char d[20];
  memcpy(d, s, (strlen(s) + 1));
  printf ("%s", d);
  getchar();
  return 0;
}
