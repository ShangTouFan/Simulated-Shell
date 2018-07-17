#include "ps.h" 

extern int IsDigt(char *);
extern int sign_sum ;
extern char sngpro[MAXLEN];
int Get_Uid(char *);
int cur_tty ;

void InitList(Plist phead)
{
	assert( phead != NULL ) ;
	phead->prio = NULL ;
	phead->next = NULL ;
}

int Get_len(int n)
{
	int count = 0 ;
	if(!n)
	return 1 ;
	if( n < 0 )
	{
		n = 0-n ;
		++count ;
	}
	while(n)
	{
		++count ;
		n /= 10 ;
	}
	return count ;
}

void Insert(Plist phead,char *name)
{
	assert( phead != NULL ) ;
	Node *p = (Node *)malloc(sizeof(Node));
	char path1[MAXLEN];
	char info[STAT_LEN];
	sprintf(path1,"/proc/%s/stat",name);
	FILE *fp1 = fopen(path1,"rb") ;
	fgets(info,STAT_LEN,fp1);
	sscanf(info,"%d",&(p->pid));
	char *s,*t ;
	s = strchr(info,'(')+1 ;
	t = strchr(info,')') ;
	strncpy(p->pname,s,t-s);
	p->pname[t-s] = '\0' ;
	p->uid = Get_Uid(name);
	sscanf(t+2,"%c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
	&(p->state),
	&(p->ppid),
	&(p->pgrp),
	&(p->session),
	&(p->tty),
	&(p->tpgid),
	&(p->flags),
	&(p->minflt),
	&(p->cminflt),
	&(p->majflt),
	&(p->cmajflt),
	&(p->utime),
	&(p->stime),
	&(p->cutime),
	&(p->cstime),
	&(p->counter),
	&(p->priority),
	&(p->timeout),
	&(p->itrealvalue),
	&(p->starttime),
	&(p->vsize),
	&(p->rss),
	&(p->rlim),
	&(p->startcode),
	&(p->endcode),
	&(p->startstack),
	&(p->kstkesp),
	&(p->kstkeip),
	&(p->signal),
	&(p->blocked),
	&(p->sigignore),
	&(p->sigcatch),
	&(p->wchan));
	
	if( p->state == 'R' )
		cur_tty = p->tty ;
	p->next = phead->next ;
	phead->next = p ;
	p->prio = phead ;
	if( p->next != NULL )
		p->next->prio = p ;
	fclose(fp1);
}

static int Myatoi(char *src)
{
	int num = 0 ;
	while(*src != '\0')
	{
		num = num*10 + *src - '0' ;
		++src ;
	}
	return num ;
}

int Get_Uid(char *name)
{
	int tmp = 0 ;
	char path[MAXLEN] ;
	sprintf(path,"/proc/%s/status",name);
	FILE *fp = fopen(path,"rb");
	char buff[MAXLEN] ;
	while(fgets(buff,MAXLEN,fp) != NULL)
	{
		if(strncmp(buff,"Uid",3)==0)
		{
			char *p = &buff[5] ;
			while( IsDigit(p) )
			{
				tmp = tmp*10 + *p - '0' ;
				++p ;
			}
			fclose(fp);
			return tmp ;
		}
			
		memset(buff,0,MAXLEN);
	}
}

Node* Search(Plist phead,char *tag)
{
	assert( phead != NULL ) ;
	int tmp = 0 ;
	Node *p = phead->next ;
	if( IsDigit(tag) )
	{
		 tmp = Myatoi(tag);
		 while( p != NULL )
		 {
		 	if( p->pid == tmp )
				return p ;
			p = p->next ;
		 }
	}
	else
	{
		while( p != NULL )
		{
			if( strcmp(p->pname,tag) == 0 )
				return p ;
			p = p->next ;
		}
	}
	return NULL ;
}

int GetLength(Plist phead)
{
	assert( phead != NULL );
	int count = 0 ;
	Node *p = phead->next ;
	while( p != NULL )
	{
		++count ;
		p = p->next ;
	}
	return count ;
}

int IsEmpty(Plist phead)
{
	return phead->next == NULL ;
}

static char *uid_str(uid_t uid)
{
	struct passwd *pw = getpwuid(uid) ;
	if( pw == NULL )
	{
		printf("%s:%s\n",uid,strerror(errno));
		exit(0);
	}
	return pw->pw_name ;
}

void Sort(Plist phead)
{
	assert( phead != NULL ) ;
	if( IsEmpty(phead) )
		return ;
	Node *p ;
	Node *q ;
	int len = GetLength(phead);
	int i = 0 ;

	for( ; i < len-1 ; ++i )
	{
		p = phead->next ;
		while( p->next != NULL )
		{
			q = p->next ; 
			if( p->pid > q->pid )
			{
				p->prio->next = q ;
				if( q->next != NULL )
					q->next->prio = p ;

				p->next = q->next ;
				q->prio = p->prio ;
				p->prio = q ;
				q->next = p ;
				continue ;
			}
			p = p->next ;
		}
	}
}

void Sort_tty(Plist phead)
{
	assert( phead != NULL ) ;
	if( IsEmpty(phead) )
		return ;
	Node *p ;
	Node *q ;
	int len = GetLength(phead);
	int i = 0 ;

	for( ; i < len-1 ; ++i )
	{
		p = phead->next ;
		while( p->next != NULL )
		{
			q = p->next ; 
			if( p->tty < q->tty )
			{
				p->prio->next = q ;
				if( q->next != NULL )
					q->next->prio = p ;

				p->next = q->next ;
				q->prio = p->prio ;
				p->prio = q ;
				q->next = p ;
				continue ;
			}
			p = p->next ;
		}
	}
}

void tty_time(Node *p)
{
	if(p->tty <= 1031 && p->tty > 1024 )
		sprintf(p->ctty,"tty%d",(p->tty-1024)) ;
	else if( p->tty >= 34816 )
		sprintf(p->ctty,"pts/%d",(p->tty-34816)) ;
	else
		strcpy(p->ctty,"?");

	sprintf(p->c_time,"%02d:%02d:%02d",p->utime/360000,(p->utime/6000)%60,(p->utime/100)%60);
}

void Show_Ps(Plist phead)
{
	assert( phead != NULL ) ;
	if( IsEmpty(phead) )
		return ;
	Sort(phead);
	if(!(sign_sum & SIGNS) && sign_sum & SIGNf )
		Sort_tty(phead);
	Node* p = phead->next ;
	if( sign_sum & SIGNS && sign_sum & SIGNf )
		printf("%-8s ","USER");
	printf("%5s ","PID");
	printf("%-8s ","TTY");
	if(!(sign_sum & SIGNS) && sign_sum != SIGNNONE )
		printf("%-6s ","STAT");
	printf("%8s ","TIME");
	printf("%s\n","CMD");
	for( ; p != NULL ; p = p->next )
	{
		tty_time(p);
		if( sign_sum & SIGNGREP )
		{
			Node *cur = Search(phead,sngpro);
			if( p == NULL )
			{
				perror("grep");
				exit(0);
			}
			if( p != cur )
				continue ;
		}
		if( sign_sum == SIGNNONE || sign_sum == (SIGNf+SIGNS))
		{
			if(	(strcmp(p->pname,"bash") != 0 || p->tty != cur_tty) &&
					p->state != 'R') 
				continue ;
		}
		if( !(sign_sum & SIGNS) && (sign_sum & SIGNe || sign_sum & SIGNf))
		{
			if(p->tty < 34816)
				continue ;
		}
		if( sign_sum & SIGNa )
		{
			if( sign_sum & SIGNS )
			{
				if(p->tty < 34816 || strstr(p->pname,"bash") != NULL )
					continue ;
			}
			else
			{
				if(p->tty < 34816 && !(p->tty <= 1031 && p->tty > 1024))
					continue ;
			}
		}
		if(sign_sum & SIGNS && sign_sum & SIGNf )
			printf("%-8s ",uid_str(p->uid));
		printf("%5d ",p->pid);
		printf("%-8s ",p->ctty);
		if(!(sign_sum & SIGNS) && sign_sum != SIGNNONE )
			printf("%-6c ",p->state);
		printf("%8s ",p->c_time);
		if(!(sign_sum & SIGNS) && sign_sum & SIGNf &&
				strstr(p->pname,"bash") == NULL )
			printf(" \\_ ");
		printf("%s\n",p->pname);
	}
}

void Destroy(Plist phead)
{
	assert( phead != NULL ) ;
	Node *p = phead->next ;
	Node *q ;

	while( p != NULL )
	{
		q = p->next ;
		p->next = NULL ;
		p->prio = NULL ;
		free(p) ;
		p = q ;
	}
}
