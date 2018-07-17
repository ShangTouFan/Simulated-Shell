#include "ls.h"

static void Get_Width();
int SignVal(char) ;
void Get_Width();
void Get_cwdname(char *);
void Start_ls(char *);

int termwidth ;
int sign_sum ;
char cw_dir[MAXLEN][MAXLEN];
char first_cwd[MAXLEN];
char all_dir[MAXLEN][MAXLEN];
int alldir_count ;
int dir_count ;
char cwdname[MAXLEN];
File_Node head ;
Maxlen maxlen ;

void Get_Width()
{
	struct winsize size ;
	termwidth = 80 ;
	if( isatty(STDOUT_FILENO) == 0 )
	{
		printf("not a trr\n");
		exit(0) ;
	}
	if( ioctl(STDOUT_FILENO,TIOCGWINSZ,&size) == -1 || size.ws_col == 0 )
	{
		printf("get size failed\n");
		exit(0) ;
	}
	termwidth = size.ws_col ;
}

void Get_cwdname(char *d_name)
{
	memset(cwdname,0,MAXLEN);
	if( dir_count == 0 || strcmp(cw_dir[dir_count-1],first_cwd) == 0 )
		sprintf(cwdname,"%s",d_name);

	else
	{
		char *p = cw_dir[dir_count-1] ;
		p += strlen(cw_dir[dir_count-1]);
		while( *p != '/' )
			--p ;
		++p;
		sprintf(cwdname,"%s/%s",p,d_name);
	}
}

int SignVal(char sign)
{
	switch(sign)
	{
		case 'a': return SIGNa ;
		case 'A': return SIGNA ;
		case 'i': return SIGNi ;
		case 'l': return SIGNl ;
		case 'L': return SIGNL ;
		case 'f': return SIGNf ;
		case 'F': return SIGNF ;
		case 's': return SIGNs ;
		case 'S': return SIGNS ;
		case 'r': return SIGNr ;
		case 'R': return SIGNR ;
		case 't': return SIGNt ;
		case 'k': return SIGNk ;
		case 'n': return SIGNn ;
		case 'u': return SIGNu ;
		case 'U': return SIGNU ;
		case 'h': return SIGNh ;
		case 'x': return SIGNx ;
		case 'X': return SIGNX ;
		case 'm': return SIGNm ;
		case 'c': return SIGNc ;
		case 'C': return SIGNC ;
		case 'o': return SIGNo ;
		case '1': return SIGN1 ;
		default : printf("not a correct command\n") ;exit(0) ;
	}
}

void Start_ls(char *cwd)
{	
	chdir(cwd);
	DIR *file = opendir(cwd) ;
	if( file == NULL && !(sign_sum && SIGNR))
	{
		printf("%s:%s\n",cwd,strerror(errno));
		exit(0);
	}
	else if( file == NULL && sign_sum && SIGNR)
	{
		printf("%s:%s\n",cwd,strerror(errno));
		chdir(cw_dir[--dir_count]);
		return ;
	}
	struct dirent *pd ;
	struct stat info ;
	InitList(&head) ;
	InitMaxlen(&maxlen) ;
	while( ( pd = readdir(file) ) != NULL )
	{
		if( !(sign_sum & SIGNa) && (sign_sum & SIGNA) && (strcmp(pd->d_name,".") == 0 || strcmp(pd->d_name,"..") ==0 ) )
				continue ;

		if( !(sign_sum & SIGNa) && !(sign_sum & SIGNA) && 
				strncmp(pd->d_name,".",1) == 0 )
			continue ;

		if( sign_sum & SIGNL )
		{
			if( stat(pd->d_name,&info) == -1 && !(sign_sum && SIGNR))
			{
				printf("%s:%s\n",pd->d_name,strerror(errno));
				exit(0) ;
			}
			else if( stat(pd->d_name,&info) == -1 && sign_sum && SIGNR )
			{
				chdir(cw_dir[--dir_count]);
				printf("%s:%s\n",pd->d_name,strerror(errno));
				return ;
			}
		}
		else
		{
			if( lstat(pd->d_name,&info) == -1 && !(sign_sum && SIGNR))
			{
				printf("%s:%s\n",pd->d_name,strerror(errno));
				exit(0) ;
			}		
			else if( lstat(pd->d_name,&info) == -1 && sign_sum && SIGNR)
			{
				chdir(cw_dir[--dir_count]);
				printf("%s:%s\n",pd->d_name,strerror(errno));
				return ;
			}		
		}
		Insert(&head,&maxlen,info,pd->d_name);
	}
	Get_Width() ;
	Show_List(&head,&maxlen);
	File_Node *p = head.next ;
	while( p != NULL )
	{
		if( sign_sum & SIGNR && strncmp(p->filename,".",1) == 0 )
		{
			p = p->next ;
			continue ;
		}
		if( sign_sum & SIGNR && p->modestr[0] == 'd' )
		{
			char cur_cwd[MAXLEN];
			strcpy(cw_dir[dir_count++],cwd);
			strcpy(cur_cwd,cwd);
			strcat(cur_cwd,"/");
			strcat(cur_cwd,p->filename);
			Get_cwdname(p->filename);
			printf("%s:\n",cwdname);
			Start_ls(cur_cwd);
		}
		p = p->next ;
	}
}

int main(int argc ,char *argv[])
{
	char cwd[MAXLEN] ;
	int i = 0 ;
	char *ps ;
	struct passwd *pw = getpwuid(getuid()) ;
	getcwd(cwd,MAXLEN-1) ;
	assert( cwd != NULL ) ;
	strcpy(all_dir[alldir_count++],cwd);
	for( i = 1 ; i < argc ; ++i )
	{
		if( strncmp(&argv[i][0],"-",1) == 0 )
		{
			ps = &argv[i][1] ;
			while( *ps != '\0' )
			{
				sign_sum += SignVal(*ps) ;
				++ps ;
			}
		}
		else if( strncmp(argv[i],"~",1) ==0 )
			strcpy(all_dir[alldir_count++],pw->pw_dir);
		else if( chdir(argv[i]) != -1 )
		{
			chdir(all_dir[0]);
			strcpy(all_dir[alldir_count++],argv[i]);
		}
		else
		{
			printf("not a correct command\n");
			exit(0) ;
		}
	}
	i = alldir_count > 1 ?  1 :  0 ;
	for(  ; i < alldir_count ; ++i )
	{
		chdir(all_dir[i]);
		getcwd(cwd,MAXLEN-1) ;
		assert( cwd != NULL ) ;
		strcpy(cw_dir[dir_count++],cwd);
		strcpy(first_cwd,cwd);
		if( alldir_count > 1 )
			printf("%s:\n",cwd);
		Start_ls(cwd);
		chdir(all_dir[0]);
		Destroy(&head);
	}
	return 0 ;
}

