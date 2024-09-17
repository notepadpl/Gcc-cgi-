
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main (){
	asm (".L0: word=0x00FF");	
	asm("bl .L0");
	return 0;
}