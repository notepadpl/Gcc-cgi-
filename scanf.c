/* scanf example */
#include <stdio.h>

int main ()
{
  char scanflag [80];
  int i;

  printf ("Enter your family name: ");
  scanf ("%s",scanflag);  
  printf ("Mr. %s \n",scanflag);

  return 0;
}