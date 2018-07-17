#include <stdio.h>
#include <assert.h>

int main()
{
	char cwd[1025] = {} ;
	assert( getcwd(cwd,1024) != NULL );
	printf("%s\n",cwd) ;
	return 0 ;
}
