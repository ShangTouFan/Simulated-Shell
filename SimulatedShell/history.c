#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAXLEN 256

typedef struct Node
{
	int num ;
	char cmd[MAXLEN];
	struct Node *next ;
	struct Node *prio ;
}Node,*Plist;

void InitList(Plist phead)
{
	assert( phead != NULL ) ;
	phead->next = NULL ;
	phead->prio = NULL ;
}

void Insert_tail(Plist phead,int n,char *val)
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
}

int GetLength(Plist phead)
{
	assert( phead != NULL );
	Node *p = phead->next ;
	int count = 0 ;
	while( p != NULL )
	{
		++count ;
		p = p->next ;
	}
	return count ;
}

void Show(Plist phead,int clen)
{
	assert( phead != NULL );
	int i = 0 ;
	int length = GetLength(phead);
	Node *p = phead->next ;
	for( ; i < length-clen ; ++i )
		p = p->next ;

	while( p != NULL )
	{
		printf("%5d %s\n",p->num,p->cmd);
		p = p->next ;
	}
}

void Destroy(Plist phead)
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

int Myatoi(char *src)
{
	int num = 0 ;
	while( *src != '\0' )
	{
		num = num * 10 + *src - '0' ;
		++src;
	}
	return num ;
}

int main(int argc,char *argv[])
{
	int fd = open("/home/A386/Desktop/STF/shell/mybin/history.txt",O_RDONLY);
	Node head ;
	InitList(&head);
	if( fd == -1 )
	{
		perror("fd");
		exit(0);
	}
	char get_cmd[MAXLEN] = {0};
	char c[2] = {0} ;
	int count = 1 ;
	while( read(fd,c,1) > 0 )
	{
		if( strcmp(c,"\n") == 0 )
		{
			Insert_tail(&head,count,get_cmd);
			++count ;
			memset(get_cmd,0,MAXLEN);
			continue ;
		}
		strcat(get_cmd,c);
	}
	int len = 0 ;
	if( argc > 1 && isdigit(argv[1][0]))
		 len = Myatoi(argv[1]);
	else
		len = GetLength(&head);

	Show(&head,len);	
	Destroy(&head);
	return 0 ;
}
