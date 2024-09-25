/* scanf example */
#include <stdio.h>

int main ()
{
  char str [80];
  int i;

  printf ("Enter your family name: ");
  scanf ("%s",str);  
  printf ("Mr. %s \n",str);

  return 0;
}