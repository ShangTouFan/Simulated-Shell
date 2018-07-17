#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <termios.h>
#include <pwd.h>
#include <sys/types.h>
#include <shadow.h>

#define MAXLEN 1024
struct termios old,new ;

void input_password(char *passwd)
{
	tcgetattr(0,&old);
	new = old ;
	new.c_lflag &= ~ECHO ;
	tcsetattr(0,TCSANOW,&new) ;
	fflush(stdin) ;
	printf("Password: ");
	fgets(passwd,MAXLEN,stdin) ;
	tcsetattr(0,TCSANOW,&old) ;
	passwd[strlen(passwd) - 1] = 0 ;
	printf("\n") ;
}

int main(int argc,char *argv[])
{
	char usrname[MAXLEN] = "root";
	char passwd[MAXLEN]={0} ;
	if( argv[1] != NULL )
		strcpy(usrname,argv[1]);

	struct passwd *pw = getpwnam(usrname);
	if( pw == NULL )
	{
		printf("su:Please input correct usrname\n");
		exit(0) ;
	}

	input_password(passwd) ;

	{//matching
		struct spwd *sp = getspnam(usrname) ;
		if( sp == NULL )
		{
			printf("error:get salt failed\n") ;
			exit(0) ;
		}
		char salt[MAXLEN] = {0} ;
		int i = 0,count = 0 ;
		for( ; sp->sp_pwdp[i] != '\0'; ++i )
		{
			salt[i] = sp->sp_pwdp[i] ;
			if( sp->sp_pwdp[i] == '$' )
				++count ;

			if( count == 3 )
				break ;
		}
		char *p = crypt(passwd,salt) ;
		assert( p != NULL ) ;

		if( strcmp(p,sp->sp_pwdp) != 0 )
		{
			printf("matching failed\n");
			exit(0) ;
		}
	}

	pid_t p = fork() ;
	assert( p != -1 ) ;
	if( p == 0 )
	{
		setuid(pw->pw_uid) ;
		setenv("HOME",pw->pw_dir,1);
		execl("/home/A386/Desktop/STF/shell/mybin/shell","/home/A386/Desktop/STF/shell/mybin/shell",(char *)0) ;
		perror("error") ;
		exit(0) ;
	}
	else
	{
		wait(NULL) ;
	}

	return 0 ;
}
