#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define MAXLEN 128

int main(int argc,char *argv[])
{
	char *cur_argv[MAXLEN] = {0} ;
	int i = 2 ;
	int j = 1 ;

	while( j < argc )
		cur_argv[i++] = argv[j++] ;

	cur_argv[0] = "ls" ;
	cur_argv[1] = "-l" ;

	execv("/home/A386/Desktop/STF/shell/mybin/ls",cur_argv) ;
	exit(0) ;
}
