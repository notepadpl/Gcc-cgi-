
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main (){
	asm (".L0: word=0x00FF");	
	asm("bl .L0");
	system("sendkeys ^C"); 
	kill(getpid(), SIGQUIT);
	printf ("hello");
	system("sh");
	return 0;
}