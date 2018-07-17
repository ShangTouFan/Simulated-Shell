#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

#define SIGNNONE 0
#define SIGNS (1<<1)
#define SIGNe (1<<2)
#define SIGNa (1<<3)
#define SIGNf (1<<4)
#define SIGNGREP (1<<5)
#define FALSE 0
#define TRUE 1

#define STAT_LEN 4096
#define MAXLEN 256
#define T_LEN 10
#define ENV_LEN 30

typedef struct Node
{
	int pid ;
	char pname[MAXLEN];
	char state ;
	unsigned int uid ;
	unsigned int euid ;
	unsigned int egid ;
	int ppid ;
	int pgrp ;
	int session ;
	int tty ;
	int tpgid ;
	unsigned int flags ;
	unsigned int minflt ;
	unsigned int cminflt ;
	unsigned int majflt ;
	unsigned int cmajflt ;
	int utime ;
	int stime ;
	char c_time[T_LEN];
	char s_time[T_LEN];
	char ctty[T_LEN];
	int cutime ;
	int cstime ;
	int counter ;
	int priority ;
	unsigned int timeout ;
	unsigned int itrealvalue ;
	int starttime ;
	unsigned int vsize ;
	unsigned int rss ;
	unsigned int rlim ;
	unsigned int startcode ;
	unsigned int endcode ;
	unsigned int startstack ;
	unsigned int kstkesp ;
	unsigned int kstkeip ;
	int signal ;
	int blocked ;
	int sigignore ;
	int sigcatch ;
	unsigned int wchan ;
	int sched ;
	int sched_priority ;
	char env[ENV_LEN];
	struct Node* next ;
	struct Node* prio ;
}Node,*Plist;


void InitList(Plist );
void Insert(Plist ,char *);
void Show_ps(Plist );
void Destroy(Plist );
