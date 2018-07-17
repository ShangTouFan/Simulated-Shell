#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

int main(int argc,char *argv[])
{
	if( argc <= 1 )
	{
		printf("mkdir: command not found\n");
		exit(0);
	}
	int i = 1 ;
	for( ; i < argc ; ++i )
	{
		if(access(argv[i],F_OK) == 0 )
		{
			printf("mkdir:Dir is exist\n");
			exit(0);
		}
		mkdir(argv[i],0775);
	}
	return 0 ;
}
