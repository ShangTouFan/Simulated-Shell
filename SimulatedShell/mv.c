#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PATH_LEN 256
#define ARGV_LEN 32

char *myargv[ARGV_LEN];

int main(int argc,char *argv[])
{
	if(argc < 3)
	{
		printf("missing parm\n");
		exit(0);
	}
	struct stat info ;
	char src[PATH_LEN];
	char des[PATH_LEN];
	strcpy(src,argv[1]);
	strcpy(des,argv[2]);
	int check = lstat(src,&info);
	if( check == -1 )
	{
		perror("lstat");
		exit(0);
	}
	else if( S_ISDIR(info.st_mode) )
	{
		myargv[0] = "cp" ;
		myargv[1] = "-r" ;
		int i = 1,j = 2 ;
		for( ; i < argc ; ++i)
		{
			myargv[j++] = argv[i];
		}
		pid_t pid = fork();
		if( pid == 0 )
			execv("/home/A386/Desktop/STF/shell/mybin/cp",myargv);
		else
			wait(NULL);
			
		memset(myargv,0,sizeof(myargv));
		myargv[0] = "rm" ;
		myargv[1] = "-r" ;
		myargv[2] = argv[1] ;
		execv("/home/A386/Desktop/STF/shell/mybin/rm",myargv);
	}
	else
	{
		myargv[0] = "cp" ;
		int i = 1 ;
		for( ; i < argc ; ++i)
		{
			myargv[i] = argv[i];
		}
		pid_t pid = fork();
		if( pid == 0 )
			execv("/home/A386/Desktop/STF/shell/mybin/cp",myargv);
		else
			wait(NULL);
			
		memset(myargv,0,sizeof(myargv));
		myargv[0] = "rm" ;
		myargv[1] = argv[1] ;
		execv("/home/A386/Desktop/STF/shell/mybin/rm",myargv);
	}
	exit(0) ;
}
