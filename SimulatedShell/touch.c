#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>


int main(int argc,char *argv[])
{
	if( argc <= 1 )
	{
		printf("touch: command not found\n");
		exit(0);
	}
	int i = 1 ;
	for( ; i < argc ; ++i )
	{
		int check = creat(argv[i],0664);
		assert(check != -1);
	}
	return 0 ;
}
