#include "ps.h"

int sign_sum ;
char sngpro[MAXLEN];
Node g_head ;

int IsDigit(char *name)
{
	if( *name >= '0' && *name <= '9' )
		return TRUE ;

	return FALSE ;

}

int Get_Sign(char sign)
{
	switch(sign)
	{
		case '|' : return 0 ;
		case '-' : return SIGNS ;
		case 'a' : return SIGNa ;
		case 'e' : return SIGNe ;
		case 'f' : return SIGNf ;
		default  : printf("command not found\n");exit(0);
	}
}

void MainLoop()
{
	DIR *dir = opendir("/proc");
	if( dir == NULL )
	{
		perror("dir");
		exit(0);
	}
	struct dirent *pd ;
	while( (pd = readdir(dir)) != NULL )
	{
		if( IsDigit(pd->d_name) )
			Insert(&g_head,pd->d_name);
	}
	Show_Ps(&g_head);

}

int main(int argc,char *argv[])
{
	int i = 1 ;
	for( ; i < argc ; ++i )
	{
		char *p = argv[i] ;
		while( *p != '\0' )
		{
			if( strncmp(p,"grep",4) == 0 )
			{
				sign_sum += SIGNGREP ;
				strcpy(sngpro,argv[++i]);
				++i ;
				break ;
			}

			sign_sum += Get_Sign(*p);
			++p;
		}
	}
	MainLoop();
	Destroy(&g_head);
	return 0 ;
}
