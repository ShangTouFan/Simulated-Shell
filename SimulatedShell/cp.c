#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <dirent.h>

#define PATH_LEN 256
#define RDWR_LEN 128

void Copy_File(const char *src,const char *des)
{
	int fd_src,fd_des;
	struct stat info ;
	char rdwr[RDWR_LEN];
	int cnt = 0 ;
	lstat(src,&info);
	fd_src = open(src,O_RDONLY);
	fd_des = open(des,O_WRONLY | O_CREAT | O_TRUNC,info.st_mode);
	if(fd_src == -1 || fd_des == -1)
	{
		perror("open");
		exit(0);
	}
	while( (cnt = read(fd_src,rdwr,RDWR_LEN)) > 0 )
	{
		write(fd_des,rdwr,cnt);
		memset(rdwr,0,RDWR_LEN);
	}
	close(fd_src);
	close(fd_des);
}

void Copy_Dir(char *src,char *path)
{
	DIR* fp = opendir(src);
	if(fp == NULL)
	{
		perror("opendir");
		exit(0);
	}
	struct stat info ;
	chdir(src);
	struct dirent *pd ;
	while( (pd = readdir(fp)) != NULL)
	{
		if(strncmp(pd->d_name,".",1) == 0)
			continue ;
		if((lstat(pd->d_name,&info)) == -1 )
		{
			perror("pd->d_name:lstat");
			exit(0);
		}
		if( S_ISDIR(info.st_mode))
		{
			strcat(path,"/");
			strcat(path,pd->d_name);
			mkdir(path,info.st_mode);
			strcat(src,"/");
			strcat(src,pd->d_name);
			Copy_Dir(src,path);
		}
		else
		{
			char src_file[PATH_LEN];
			char des_file[PATH_LEN];
			strcpy(src_file,src);
			strcpy(des_file,path);
			strcat(src_file,"/");
			strcat(des_file,"/");
			strcat(src_file,basename(pd->d_name));
			strcat(des_file,basename(pd->d_name));
			Copy_File(src_file,des_file);
		}
	}
	chdir("..");
	closedir(fp);
}

int main(int argc,char *argv[])
{
	char cwd[PATH_LEN];
	getcwd(cwd,PATH_LEN-1);
	struct stat info ;
	char src[PATH_LEN] ;
	char des[PATH_LEN] ;
	if( argc <= 2 )
	{
		printf("cp: command not found\n");
		exit(0);
	}
	if( strncmp(argv[1],"-r",2) == 0 )
	{
		strcpy(src,argv[2]);
		strcpy(des,argv[3]);
		lstat(src,&info);
		if(access(des,F_OK) == -1)	
			mkdir(des,info.st_mode);
		else
		{
			strcat(des,"/");
			strcat(des,basename(src));
			mkdir(des,info.st_mode);
		}
		chdir(src);
		getcwd(src,PATH_LEN-1);
		chdir(cwd);
		chdir(des);
		getcwd(des,PATH_LEN-1);
		chdir(cwd);
		Copy_Dir(src,des);
	}
	else
	{
		strcpy(src,argv[1]);
		strcpy(des,argv[2]);
		int check = lstat(des,&info);
		if( access(des,F_OK)==0 && check != -1 && S_ISDIR(info.st_mode) )
		{
			strcat(des,"/");
			strcat(des,basename(src));
			Copy_File(src,des);
		}
		else
			Copy_File(src,des);
	}
	chdir(cwd);
	return 0 ;
}
