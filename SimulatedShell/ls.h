#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <errno.h>

#define SIGNNONE 1
#define SIGNa (1<<1)
#define SIGNA 1
#define SIGNi (1<<2)
#define SIGNl (1<<3)
#define SIGNL (1<<4)
#define SIGNf (1<<5)
#define SIGNF (1<<6)
#define SIGNs (1<<7)
#define SIGNS (1<<8)
#define SIGNr (1<<9)
#define SIGNR (1<<10)
#define SIGNt (1<<11)
#define SIGNn (1<<12)
#define SIGNU (1<<13)
#define SIGNk (1<<14)
#define SIGNh (1<<15)
#define SIGNx (1<<16)
#define SIGNm (1<<17)
#define SIGN1 (1<<18)
#define SIGNu (1<<19)
#define SIGNo (1<<20)
#define SIGNX (1<<21)
#define SIGNc (1<<22)
#define SIGNC (1<<23)

#define ERROR -1

#define None "\033[0m"
#define Green "\033[1;32m"
#define Blue "\033[1;34m"
#define Red "\033[1;31m"
#define Cyan "\033[1;36m"
#define Su	"\033[37;41m"
#define Purple "\033[1;35m"
#define Yellow "\033[1;33m"
#define Tmp "\033[30;42m"
#define FIFO "\033[40;33m"
#define MAXLEN 256
#define SIZE 128
#define NAME_SIZE 32
#define TIME_SIZE 20
#define COLOR_SIZE 16
#define MODESIZE 12
#define F_SIZE 2
#define TRUE 1
#define FALSE 0

typedef struct List_of_file
{
	int inode ;
	char modestr[MODESIZE] ;
	int nlink ;
	int uid ;
	int gid ;
	char usrname[NAME_SIZE] ;
	char grpname[NAME_SIZE] ;
	long size ;
	long space ;
	double h_size ;
	char time[TIME_SIZE] ;
	int i_len ;
	int f_len ;
	int u_len ;
	int g_len ;
	int n_len ;
	int hs_len ;
	int si_len ;
	int sp_len ;
	int atime ;
	int mtime ;
	int ctime ;
	char F_SIGN[F_SIZE];
	char filename[MAXLEN] ;
	char lastname[MAXLEN] ;
	char color[COLOR_SIZE] ;
	struct List_of_file *next ;
	struct List_of_file *prio ;
}File_Node,*Plist;

typedef struct File_Maxlen
{
	int inode_mlen ;
	int nlink_mlen ;
	int usrname_mlen ;
	int grpname_mlen ;
	int space_mlen ;
	int size_mlen ;
	int hsize_mlen ;
	int filename_mlen ;
	long total_size ;
}Maxlen;

void InitMaxlen(Maxlen *) ;
void InitList(Plist) ;
void Insert(Plist,Maxlen *,struct stat,const char *) ;
int GetLength(Plist) ;
void Show_List(Plist,Maxlen *) ;
void Reverse(Plist) ;
void Destroy(Plist) ;
void Div_ls(Plist,int,int);
void Show_ll(Plist,Maxlen *);


