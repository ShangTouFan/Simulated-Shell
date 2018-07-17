#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <assert.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
//#include <curses.h>
//#include <term.h>

#define MAXLEN 256

typedef struct Node
{
	int num ;
	char cmd[MAXLEN];
	struct Node *next ;
	struct Node *prio ;
}Node,*Plist;
static struct termios new,old ;
static int peek_character = -1 ;
static void init_keyboard() ;
static void close_keyboard() ;
static int kbhit();
static int readch();
static void InitList(Plist);
static void Insert_tail(Plist ,int,char *);
static void Destroy(Plist );
static char all_path[MAXLEN][MAXLEN] ;
static int cur_pos = 2 ;
static Node head ;
static Node tail ;
static int count = 1 ;

static void init_keyboard()
{
	tcgetattr(0,&old);
	new = old ;
	new.c_lflag &= ~ICANON ;
	new.c_lflag &= ~ECHO ;
	new.c_lflag &= ~ISIG ;
	new.c_cc[VMIN] = 1 ;
	new.c_cc[VTIME] = 0 ;
	tcsetattr(0,TCSANOW,&new);
}

static void close_keyboard()
{
	tcsetattr(0,TCSANOW,&old);
}

static int kbhit()
{
	unsigned char ch ;
	int nread ;
	if( peek_character != -1 )
		return 1 ;
	new.c_cc[VMIN] = 0 ;
	tcsetattr(0,TCSANOW,&new);
	nread = read(0,&ch,1);
	new.c_cc[VMIN] = 1 ;
	tcsetattr(0,TCSANOW,&new);
	if(nread == 1)
	{
		peek_character = ch ;
		return 1 ;
	}
	return 0 ;
}

static int readch()
{
	char ch ;

	if( peek_character != -1 )
	{
		ch = peek_character ;
		peek_character = -1 ;
		return ch ;
	}
	read(0,&ch,1);
	return ch ;
}
   
static void InitList(Plist phead)
{
	assert( phead != NULL ) ;
	phead->next = NULL ;
	phead->prio = NULL ;
}

static void Insert_tail(Plist phead,int n,char *val)
{
	assert( phead != NULL );
	Node *p = (Node *)malloc(sizeof(Node));
	p->num = n ;
	strcpy(p->cmd,val);
	Node *q = phead ;
	while( q->next != NULL )
		q = q->next ;

	p->next = q->next ;
	q->next = p ;
	p->prio = q ;
	tail.prio = p ;
}

static GetList(Plist phead)
{
	int fd = open("/home/A386/Desktop/STF/shell/mybin/history.txt",O_RDONLY) ;
	if( fd == -1 )
	{
		printf("fd:%s\n",strerror(errno));
		exit(0);
	}
	char pcur[MAXLEN] = {0};
	char c ;
	int i = 0 ;
	while( read(fd,&c,1) > 0 )
	{
		if( c != 10 )
			pcur[i++] = c ;
	
		else if ( c == 10 && i > 0 )
		{
			pcur[i] = 0 ;
			Insert_tail(&head,count++,pcur);
			i = 0 ;
			memset(pcur,0,MAXLEN);
		}
	}
	close(fd);
}

static void Destroy(Plist phead)
{
	assert( phead != NULL ) ;
	Node *p = phead->next ;
	Node *q ;
	phead->next = NULL ;
	while( p != NULL )
	{
		q = p->next ;
		p->next = NULL ;
		p->prio = NULL ;
		free(p);
		p = q ;
	}
}

void Init_AllPath()
{
	getcwd(all_path[0],MAXLEN-1) ;
	getcwd(all_path[1],MAXLEN-1) ;
}

void Clear(char *Argv[])
{
	memset(Argv,0,MAXLEN*sizeof(char*)) ;
}

void Print_Tag()
{
	char username[MAXLEN] = {0} ;
	char cwd[MAXLEN] = {0} ;
	struct passwd *pw = getpwuid(getuid()) ;
	int check = gethostname(username,MAXLEN) ;
	strtok(username,".");
	getcwd(cwd,MAXLEN-1) ;
	int len = strlen(cwd) ;
	char *p = cwd+len ;

	if( strcmp(cwd,pw->pw_dir) == 0 )
		p = "~" ;

	else
	{
		while( *p != '/' && len-- )
			--p ;
	
		if( strlen(p) != 1 )
			++p ;
	}

	assert( pw != NULL && check == 0 ) ;
	if( strcmp(pw->pw_name,"root") == 0 )
		printf("[mybash%s@%s %s]# ",pw->pw_name,username,p);

	else
		printf("[mybash%s@%s %s]$ ",pw->pw_name,username,p);

	fflush(stdout);
} 

void Str_Tok(char *cmd,char *Argv[])
{
	int i = 0 ;
	char *p = strtok(cmd," ") ;
	while( p != NULL )
	{
		Argv[i++] = p ;
		p = strtok(NULL," ") ;
	}
}

void Write_History(char *cmd)
{
	int fd = open("/home/A386/Desktop/STF/shell/mybin/history.txt", O_WRONLY | O_APPEND) ;
	if( fd == -1 )
	{
		printf("fd:%s\n",strerror(errno));
		exit(0);
	}
	write(fd,cmd,strlen(cmd));
	write(fd,"\n",1);
	close(fd);
}

void Get_Cmd(char *cmd,char *Argv[])
{
	Node *now = &tail ;
	Clear(Argv) ;
	char ch = 0 ;
	init_keyboard();
	int i = 0 ;
	int j ;
	while( ch != '\n' )
	{
		if( kbhit())
		{
			ch = readch();
			switch(ch)
			{
				case 127 : 
					{
						if( i > 0 )
						{
							int len = strlen(cmd);
							printf("\033[D");
							printf("\033[s");
							for( j = i-1; cmd[j] != 0 ; ++j )
								cmd[j] = cmd[j+1];
							if( i > 1 )
								printf("\033[%dD",i-1);
							printf("\033[K");
							printf("%s",cmd);
							printf("\033[u");
							--i;
							fflush(stdout);
						}
					}
					break ;
				case 27 :
					{
						ch = readch();
						if( ch == 91 )
						{
							ch = readch();
							if( ch == 68 )
							{
								if( i > 0 )
								{
									printf("\033[D");
									--i;
								}
							}
							else if( ch == 67 )
							{
								if( i < strlen(cmd) )
								{
									++i;
									printf("\033[C");
								}
							}
							else if( ch == 65 )
							{
								if( now->prio != NULL)
									now = now->prio ;
								if( now != NULL )
								{
									int len = strlen(now->cmd);
									if( i > 0 )
										printf("\033[%dD",i);
									printf("\033[K");
									strcpy(cmd,now->cmd);
									printf("%s",cmd);
									i = len ;
								}
							}
							else if( ch == 66 )
							{
								if( now->next != NULL )
									now = now->next ;
								if( now != NULL )
								{
									int len = strlen(now->cmd);
									if( i > 0 )
									printf("\033[%dD",i);
									printf("\033[K");
									strcpy(cmd,now->cmd);
									printf("%s",cmd);
									i = len ;
								}
							}
						}
					}
						fflush(stdout);
						break ;
				case 10:
					{
						if( i > 0 )
							printf("\033[%dD",i);
						if( j > 0 )
							printf("\033[%dC",j);
						printf("%c",ch);
						fflush(stdout);
					}
					break ;
				default :
				{
					j = strlen(cmd);
					for( ; j > i ; --j )
						cmd[j] = cmd[j-1];
					cmd[i] = ch ;
					printf("\033[C");
					printf("\033[s");
					printf("\033[%dD",i+1);
					printf("\033[K");
					printf("%s",cmd);
					printf("\033[u");
					++i;
					fflush(stdout);
				}
				break;
			}
		}
	}
	close_keyboard();
	Write_History(cmd);
	Insert_tail(&head,count++,cmd);
	Str_Tok(cmd,Argv) ;
}

void Call_cd(const char *path)
{
	struct passwd *pw = getpwuid(getuid()) ;
	if( path == NULL || strcmp(path,"~") == 0 )
	{
		chdir(pw->pw_dir) ;
		getcwd(all_path[cur_pos++],MAXLEN-1) ;
	}

	else if( strcmp(path,"-") == 0 )
	{
		printf("%s\n",all_path[cur_pos-2]);
		chdir(all_path[cur_pos-2] ) ;
		getcwd(all_path[cur_pos++],MAXLEN-1) ;
	}
	
	else if( chdir(path) != -1 )
		getcwd(all_path[cur_pos++],MAXLEN-1) ;
}
int main()
{
	int fd = open("/home/A386/Desktop/STF/shell/mybin/history.txt",O_CREAT | O_TRUNC ,0664) ;
	char cmd[MAXLEN]={0} ;
	char *Argv[MAXLEN] ;
	InitList(&head);
	Init_AllPath() ;
	GetList(&head);
	while(1)
	{
		memset(cmd,0,MAXLEN);
		Print_Tag() ;
		Get_Cmd(cmd,Argv) ;
		if( strncmp(cmd,"exit",4) == 0 )
			exit(0) ;
		
		else if( strncmp(cmd,"cd",2) == 0 )
		{
			Call_cd(Argv[1]);
			continue ;
		}

		else
		{
			pid_t p = fork() ;

			if( p == 0 )
			{
				if( cmd[0] == '/' || cmd[0] == '.' )
				{
					execv(cmd,Argv);
					perror(Argv[0]);
					exit(0) ;
				}

				else
				{
					char path[MAXLEN] = "/home/A386/Desktop/STF/shell/mybin/" ;					strcat(path,Argv[0]) ;
					execv(path,Argv) ;
					perror(Argv[0]) ;
					exit(0) ;
				}
			
			}

			else
				wait(NULL) ;
			
		}
	}
	Destroy(&head);
	close(fd);
}

