#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define MAXLEN 128

int kill_process(int pid,int signal)
{
	int check = kill(pid,signal) ;
	return check ;
}

int myatoi(char *str)
{
	int n = 0 ;
	while( *str != '\0' )
	{
		n = n*10 + *str - '0' ;
		++str ;
	}
	return n ;
}

int main(int argc,char *argv[])
{
	if( argc < 2 )
	{
		printf("kill: not found pid of a job\n") ;
		exit(0) ;
	}

	char p[MAXLEN] ;

	if( strncmp(argv[1],"-",1) == 0 )
	{
		strcpy(p,&argv[1][1]) ;
		assert( p != NULL ) ;
		int signal = myatoi(p) ;

		if( signal > 64 || signal < 0 )
		{
			printf("mybash:kill:invalid signal specification\n") ;
			exit(0);
		}

		int pid = myatoi(argv[2]) ;
		if( kill_process(pid,signal) )
			printf("kill:error:not a correct signal or pid\n");
	}

	else
	{
		int pid = myatoi(argv[1]) ;
		if( kill_process(pid,SIGTERM) )
			printf("mybash:kill: No such process\n");
	}

	return 0 ;
}
