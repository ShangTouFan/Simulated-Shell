#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>

#define SIGNNONE 1
#define SIGNr (1<<1)
#define TRUE 1
#define FALSE 0
#define PATH_LEN 256

int sign_sum ;
char path[PATH_LEN][PATH_LEN];
int path_cnt ;

int Get_Sum(char val)
{
	switch(val)
	{
		case 'r' : return SIGNr ;break ;
		default : return 0 ;break ;
	}
}

void Rmv_DIR(char *name)
{
	DIR*fp = opendir(name);
	chdir(name);
	if( fp == NULL )
	{
		perror("opendir");
		exit(0);
	}
	struct dirent *pd ;
	struct stat info ;
	while( (pd = readdir(fp)) != NULL )
	{
		if( strncmp(pd->d_name,".",1) == 0 )
			continue ;
		if(lstat(pd->d_name,&info) == -1)
		{
			perror("stat");
			exit(0);
		}
		if( S_ISDIR(info.st_mode) )
			Rmv_DIR(pd->d_name);
		else
			remove(pd->d_name);
		
	}
	chdir("..");
	remove(name);
	closedir(fp);
}

void MainLoop()
{
	int i = 0 ;
	if(!(sign_sum & SIGNr) )
	{
		for( i = 0 ; i < path_cnt ; ++i )
		{
			if( remove(path[i]) == -1 )
			{
				perror("rm");
				exit(0);
			}
		}
	}
	else
	{
		for( i = 0 ; i < path_cnt ; ++i )
			Rmv_DIR(path[i]);
	}
}

int main(int argc,char *argv[])
{
	if( argc <= 1 )
	{
		printf("rm: command not found\n");
		exit(0);
	}
	char cwd[PATH_LEN];
	getcwd(cwd,PATH_LEN-1);
	assert(cwd != NULL);
	int i = 1 ;
	for( ; i < argc ; ++i )
	{
		if( argv[i][0] != '-' )
		{
			strcpy(path[path_cnt++],argv[i]);
		}
		else
		{
			char *p = argv[i];
			while( *p != '\0')
			{
				sign_sum += Get_Sum(*p);
				++p ;
			}
		}
	}
	MainLoop();
	chdir(cwd);
	return 0 ;
}
