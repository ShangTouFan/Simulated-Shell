#include "ls.h"

typedef struct g_file
{
	int f_len ;
	int i_len ;
	int s_len ;
	int inode ;
	char F_S[F_SIZE] ;
	long space ;
	char name[MAXLEN];
	char color[COLOR_SIZE];
}g_file;

typedef struct COL_MLEN
{
	int file_mlen;
	int inode_mlen;
	int space_mlen ;
}col_mlen;

int g_row ;
int g_col ;
extern char cw_dir[MAXLEN][MAXLEN];
extern int dir_count ;
extern int sign_sum ;
extern int termwidth ;

static void PrintSpace(int );
static void Get_Modestr(int ,char *);
static int GetLen(long );
static void GetColor(File_Node *,int);
static int IsCMP(char *);
static char *uid_str(uid_t );
static char *gid_str(gid_t );
static void List_Sort_Lastname(Plist) ;
static void List_Sort_Filename(Plist) ;
static void List_Sort_TS(Plist) ;
static int IsEmpty(Plist) ;
void Get_lastname(const char *,char *);
void Show_m(Plist);
void Show_1(Plist);

void InitList(Plist phead)
{
	assert( phead != NULL ) ;
	phead->next = NULL ;
	phead->prio = NULL ;
}

void InitMaxlen(Maxlen *maxlen)
{
	assert( maxlen != NULL ) ;
	
	maxlen->nlink_mlen = 0 ;
	maxlen->hsize_mlen = 0 ;
	maxlen->space_mlen = 0 ;
	maxlen->inode_mlen = 0 ;
	maxlen->usrname_mlen = 0 ;
	maxlen->grpname_mlen = 0 ;
	maxlen->size_mlen = 0 ;
	maxlen->filename_mlen = 0 ;
	maxlen->total_size = 0 ;
}

static char *uid_str(uid_t uid)
{
	struct passwd *pw = getpwuid(uid) ;
	if( pw == NULL && sign_sum & SIGNR )
	{
		printf("%s:%s\n",uid,strerror(errno));
		chdir(cw_dir[--dir_count]);
		return ;
	}
	else if( pw == NULL && !(sign_sum && SIGNR))
	{
		printf("%s:%s\n",uid,strerror(errno));
		exit(0);
	}

	return pw->pw_name ;
}

static char *gid_str(gid_t gid)
{
	struct group *gr = getgrgid(gid) ;
	if( gr == NULL && sign_sum & SIGNR )
	{
		printf("%s:%s\n",gid,strerror(errno));
		chdir(cw_dir[--dir_count]);
		return ;
	}
	else if( gr == NULL && !(sign_sum && SIGNR))
	{
		printf("%s:%s\n",gid,strerror(errno));
		exit(0);
	}
	return gr->gr_name ;
}

static int IsCMP(char *name)
{
	while( *name != '\0' && *name != '.' )
		++name ;
	
	if( strcmp(name,".tar") == 0 || strcmp(name,".tar.gz") == 0 || 
			strcmp(name,".zip") == 0 || strcmp(name,".tgz") == 0 
			|| strcmp(name,".Z") == 0 )
		return TRUE ;

	return FALSE ;
}

static void GetColor(File_Node *p,int mode)
{
	assert( p != NULL ) ;

	switch( mode & S_IFMT )
	{
		case S_IFREG: 
			{
				if( mode & S_ISUID || mode & S_ISGID )
				{
					if( mode & S_IXUSR || mode & S_IXGRP || mode & S_IXOTH)
					{
						strcpy(p->F_SIGN,"*");
						if( sign_sum & SIGNF )
							p->f_len += 1 ;
					}
					strcpy(p->color,Su) ;
					break ;
				}
				else if( mode & S_IXUSR || mode & S_IXGRP || mode & S_IXOTH )
				{
					strcpy(p->color,Green) ;
					strcpy(p->F_SIGN,"*");
					if( sign_sum & SIGNF )
						p->f_len += 1 ;
					break ;
				}
				else if( IsCMP(p->filename) )
				{
					strcpy(p->color,Red) ;
					break ;
				}
				else
				{
					strcpy(p->color,None);
					break ;
				}
			}
		case S_IFLNK: 
			{
				strcpy(p->F_SIGN,"@");
				strcpy(p->color,Cyan) ; 
				if( sign_sum & SIGNF )
					p->f_len += 1 ;
				break ;
			}
		case S_IFDIR: 
					{
						if( sign_sum & SIGNF )
							p->f_len += 1 ;
						strcpy(p->F_SIGN,"/");
						if( mode & S_ISVTX )
						{
							strcpy(p->color,Tmp);
							break ;
						}
						else
						{
							strcpy(p->color,Blue);
							break ;
						}
					}
		case S_IFSOCK:
					{
						if( sign_sum & SIGNF )
							p->f_len += 1 ;
						strcpy(p->F_SIGN,"=");
						strcpy(p->color,Purple) ;
					  	break ;
					}
		case S_IFIFO: 
					 {
						 if( sign_sum & SIGNF )
							 p->f_len += 1 ;
					  	strcpy(p->F_SIGN,"|");
						strcpy(p->color,FIFO) ; 
						break ;
					 }
		case S_IFBLK: strcpy(p->color,Yellow) ; break ;
		case S_IFCHR: strcpy(p->color,Yellow) ; break ;
		default : strcpy(p->color,None) ; break ;
	}
}

static int GetLen(long n)
{
	if(!n)
		return 1 ;
	int count = 0 ;
	while( n )
	{
		++count;
		n /= 10 ;
	}
	return count ;
}

static void Get_Modestr(int mode,char *modestr)
{
	strcpy(modestr,"----------.");

	if( S_ISDIR(mode) ) modestr[0] = 'd' ;
	if( S_ISLNK(mode) ) modestr[0] = 'l' ;
	if( S_ISBLK(mode) ) modestr[0] = 'b' ;
	if( S_ISCHR(mode) ) modestr[0] = 'c' ;
	if( S_ISFIFO(mode) )modestr[0] = 'p' ;
	if( S_ISSOCK(mode) )modestr[0] = 's' ;

	if( mode & S_IRUSR ) modestr[1] = 'r' ;
	if( mode & S_IWUSR ) modestr[2] = 'w' ;
	if( mode & S_IXUSR ) modestr[3] = 'x' ;
	if( mode & S_ISUID ) modestr[3] = 's' ;

	if( mode & S_IRGRP ) modestr[4] = 'r' ;
	if( mode & S_IWGRP ) modestr[5] = 'w' ;
	if( mode & S_IXGRP ) modestr[6] = 'x' ;
	if( mode & S_ISGID ) modestr[6] = 's' ;

	if( mode & S_IROTH ) modestr[7] = 'r' ;
	if( mode & S_IWOTH ) modestr[8] = 'w' ;
	if( mode & S_IXOTH ) modestr[9] = 'x' ;
	if( mode & S_ISVTX ) modestr[9] = 't' ;
}

void Get_lastname(const char *filename,char *lastname)
{
	char tempstr[MAXLEN] ;
	strcpy(tempstr,filename);
	char *p = strtok(tempstr,".");
	int count = 0 ;
	while( p != NULL )
	{
		strcpy(lastname,p);
		p = strtok(NULL,".");
		++count ;
	}
	if( count <= 1 )
		memset(lastname,0,MAXLEN);
}

void Insert(Plist phead,Maxlen *maxlen,struct stat info,const char *name)
{
	assert( phead != NULL && maxlen != NULL ) ;
	File_Node *p = (File_Node *)malloc(sizeof(File_Node));
	assert( p != NULL ) ;
	p->inode = (int)info.st_ino ;
	Get_Modestr(info.st_mode,p->modestr) ;
	p->nlink = info.st_nlink ;
	p->uid = info.st_uid ;
	p->gid = info.st_gid ;
	strcpy(p->usrname,uid_str(info.st_uid));
	strcpy(p->grpname,gid_str(info.st_gid));
	p->size = (long)info.st_size ;
	p->mtime = info.st_mtime ;
	p->atime = info.st_atime ;
	p->ctime = info.st_ctime ;
	strcpy(p->time,4+ctime(&info.st_mtime)) ;
	strcpy(p->filename,name) ;
	Get_lastname(p->filename,p->lastname);
	if( sign_sum & SIGNk )
	{
		if( p->size < 1024 )
			p->size = 1;

		else
			p->size = p->size%1024 == 0 ? p->size/1024 : p->size/1024 + 1 ;
	}
	if( S_ISLNK(info.st_mode))
	{
		p->space = 0 ;
		p->sp_len = 1 ;
	}
	else
	{
		p->space =  p->size % 4096 == 0 ? p->size/1024 : (p->size/4096)*4 + 4 ;
		p->sp_len = GetLen(p->space);
	}
	maxlen->total_size += p->space ;
	p->i_len = GetLen(p->inode) ;
	p->u_len = strlen(p->usrname) ;
	p->g_len = strlen(p->grpname) ;
	p->si_len = GetLen(p->size) ;
	p->n_len = GetLen(p->nlink);
	p->f_len = strlen(p->filename) ;
	p->h_size = p->size ;
	if( p->si_len  > 4)
	{
		p->hs_len = p->si_len-2 ;
	}
	else
		p->hs_len = p->si_len ;

	GetColor(p,info.st_mode) ;
	if( maxlen->nlink_mlen < p->n_len )
		maxlen->nlink_mlen = p->n_len ;
	
	if( maxlen->hsize_mlen < p->hs_len )
		maxlen->hsize_mlen = p->hs_len ;
	
	if( maxlen->space_mlen < p->sp_len )
		maxlen->space_mlen = p->sp_len ;
	
	if( maxlen->inode_mlen < p->i_len )
		maxlen->inode_mlen = p->i_len ;

	if( maxlen->usrname_mlen < p->u_len )
		maxlen->usrname_mlen = p->u_len ;

	if( maxlen->grpname_mlen < p->g_len )
		maxlen->grpname_mlen = p->g_len ;

	if( maxlen->size_mlen < p->si_len )
		maxlen->size_mlen = p->si_len ;

	if(maxlen->filename_mlen < p->f_len )
		maxlen->filename_mlen = p->f_len ;

	p->next = phead->next ;
	phead->next = p ;
	
	if( p->next != NULL )
		p->next->prio = p ;

	p->prio = phead ;
}

int GetLength(Plist phead)
{
	assert( phead != NULL ) ;
	File_Node *p = phead->next ;
	int count = 0 ;
	while( p != NULL )
	{
		++count ;
		p = p->next ;
	}
	return count ;

}

static void PrintSpace(int n)
{
	int i = 0 ;
	for( ; i < n ; ++i )
		printf(" ");
}

void Show_m(Plist phead)
{
	assert( phead != NULL ) ;
	File_Node *p = phead->next ;
	int width = 0 ;
	while( p->next != NULL )
	{
		width += (p->f_len+2) ;
		if(width > termwidth )
		{
			printf("\n");
			width = p->f_len+2 ;
		}
		printf("%s%s"None", ",p->color,p->filename);
		p = p->next ;
	}
	printf("%s%s"None,p->color,p->filename);
	printf("\n");
}

void Show_1(Plist phead)
{
	assert( phead != NULL ) ;
	File_Node *p = phead->next ;
	while( p->next != NULL )
	{
		printf("%s%s"None"\n",p->color,p->filename);
		p = p->next ;
	}
}

void Div_ls(Plist phead,int length,int m_size)
{
	assert( phead != NULL );
	if( IsEmpty(phead) )
		return ;
	if( sign_sum & SIGNm )
	{
		Show_m(phead);
		return ;
	}
	if( sign_sum & SIGN1 )
	{
		Show_1(phead);
		return ;
	}
	int i = 0 ;
	int j = 0 ;
	int k = 0 ;
	File_Node *p = phead->next ;
	g_col = termwidth / m_size ;
	if( g_col < 1 )
		g_col = 1; 
	if( g_col > 8 )
		g_col = 8 ;

	g_row = length % g_col == 0 ?  length/g_col : length/g_col+1 ;
	col_mlen *c_mlen = (col_mlen *)calloc(g_col,sizeof(col_mlen)) ;
	g_file **g_name = ( g_file**)calloc(g_row,sizeof(g_file*));
	for( ; i < g_row ; ++i )
		g_name[i] = (g_file*)calloc(g_col,sizeof(g_file));

	if( sign_sum & SIGNx )
	{
		for( i = 0 ;  i < g_row && k < length && p != NULL ; ++i )
		{
			for( j = 0 ; j < g_col && k < length && p != NULL  ; ++j )
			{
				g_name[i][j].inode = p->inode ;
				g_name[i][j].i_len = p->i_len ;
				g_name[i][j].space = p->space ;
				g_name[i][j].s_len = p->sp_len ;
				g_name[i][j].f_len = p->f_len ;
				strcpy(g_name[i][j].F_S,p->F_SIGN);
				strcpy(g_name[i][j].name,p->filename) ;
				strcpy(g_name[i][j].color,p->color);
			if( c_mlen[j].file_mlen < g_name[i][j].f_len )
				c_mlen[j].file_mlen = g_name[i][j].f_len ;
			if( c_mlen[j].inode_mlen < g_name[i][j].i_len)
				c_mlen[j].inode_mlen = g_name[i][j].i_len ;
			if( c_mlen[j].space_mlen < g_name[i][j].s_len)
				c_mlen[j].space_mlen = g_name[i][j].s_len ;
			p = p->next ;
			++k ;
			}
		}
	k = 0 ;
	}

	else
	{
		for( j = 0 ; j < g_col && k < length && p != NULL ; ++j )
		{
			for( i = 0 ; i < g_row && k < length && p != NULL  ; ++i )
			{
				g_name[i][j].inode = p->inode ;
				g_name[i][j].i_len = p->i_len ;
				g_name[i][j].space = p->space ;
				g_name[i][j].s_len = p->sp_len ;
				g_name[i][j].f_len = p->f_len ;
				strcpy(g_name[i][j].F_S,p->F_SIGN);
				strcpy(g_name[i][j].name,p->filename) ;
				strcpy(g_name[i][j].color,p->color);
			if( c_mlen[j].file_mlen < g_name[i][j].f_len )
				c_mlen[j].file_mlen = g_name[i][j].f_len ;
			if( c_mlen[j].inode_mlen < g_name[i][j].i_len)
				c_mlen[j].inode_mlen = g_name[i][j].i_len ;
			if( c_mlen[j].space_mlen < g_name[i][j].s_len)
				c_mlen[j].space_mlen = g_name[i][j].s_len ;
			p = p->next ;
			++k ;
			}
		}
	}
	k = 0 ;

	for( i = 0 ; i < g_row  ; ++i )
	{
		for( j = 0 ; j < g_col ; ++j )
		{
			if( g_name[i][j].inode == 0 )
				continue ;

			if( sign_sum & SIGNi )
			{
				printf("%-*d",g_name[i][j].i_len,g_name[i][j].inode);
				PrintSpace(c_mlen[j].inode_mlen-g_name[i][j].i_len+2);
			}
			if( sign_sum & SIGNs )
			{
				PrintSpace(c_mlen[j].space_mlen-g_name[i][j].s_len);
				printf("%d ",g_name[i][j].space);
			}
			if( sign_sum & SIGNf )
				strcpy(g_name[i][j].color,None);
			if( !(sign_sum & SIGNF) )
				strcpy(g_name[i][j].F_S,"");
			printf("%s%-*s"None"%s",g_name[i][j].color,g_name[i][j].f_len-1,g_name[i][j].name,g_name[i][j].F_S);
			PrintSpace(c_mlen[j].file_mlen-g_name[i][j].f_len+2);
			++k;
		}
		printf("\n");
	}
}

void Show_ll(Plist phead,Maxlen *maxlen)
{
	assert( phead != NULL ) ;
	if( IsEmpty(phead) )
		return ;
	File_Node *p = phead->next ;
	printf("%s %d\n","total",maxlen->total_size);
	while( p != NULL )
	{
		if( sign_sum & SIGNi )
		{
			printf("%-*d",p->i_len,p->inode);
			PrintSpace(maxlen->inode_mlen-p->i_len+1);
		}
		if( sign_sum & SIGNs )
		{
			PrintSpace(maxlen->space_mlen-p->sp_len);
			printf("%d ",p->space);
		}
		printf("%s",p->modestr);
		PrintSpace(maxlen->nlink_mlen-p->n_len);
		printf("%*d ",p->n_len,p->nlink);
		if( sign_sum & SIGNn )
			printf("%3d %3d ",p->uid,p->gid);
		else
		{
			printf("%-*s",p->u_len,p->usrname);
			PrintSpace(maxlen->usrname_mlen-p->u_len+1);
			if( !(sign_sum & SIGNo))
			{
				printf("%-*s",p->g_len,p->grpname);
				PrintSpace(maxlen->grpname_mlen-p->g_len+1);
			}
		}
		if( sign_sum & SIGNh)
		{
			if( p->size < 1024 )
			{
				PrintSpace(maxlen->hsize_mlen-p->hs_len);
				printf("%*d ",p->hs_len,p->size);
			}
			else
			{
				p->h_size /= 1024 ;
				if( p->h_size > 10 )
				{	
					PrintSpace(maxlen->hsize_mlen-p->hs_len);
					printf("%*.fK ",p->hs_len-1,p->h_size);
				}
				else
				{
					PrintSpace(maxlen->hsize_mlen-p->hs_len) ;
					printf("%*.1fK ",p->hs_len-1,p->h_size);
				}
			}
		}
		else
		{
			PrintSpace(maxlen->size_mlen-p->si_len);
			printf("%ld ",p->size);
		}
		printf("%.12s ",p->time);
		if( !(sign_sum & SIGNF ) )
				strcpy(p->F_SIGN,"");
		if( sign_sum & SIGNf )
			strcpy(p->color,None);
		printf("%s%-s"None"%s",p->color,p->filename,p->F_SIGN);
		printf("\n");
		p = p->next ;
	}
}

void Show_List(Plist phead,Maxlen *maxlen)
{
	assert( phead != NULL ) ;
	int i = 0 ;
	int j = 0 ;
	int len = GetLength(phead) ;
	int m_size = maxlen->filename_mlen ;
	if( !(sign_sum & SIGNU) && !(sign_sum & SIGNf) )
	{
		if( sign_sum & SIGNX )
			List_Sort_Lastname(phead) ;

		else if( sign_sum & SIGNt || sign_sum & SIGNu ||
				sign_sum & SIGNc || sign_sum & SIGNS )
			List_Sort_TS(phead);

		else
			List_Sort_Filename(phead);
	}

	if( sign_sum & SIGNr )
		Reverse(phead) ;

	if( sign_sum & SIGNC || (!(sign_sum & SIGNl) && !(sign_sum & SIGNn) ))
	{
		if( sign_sum & SIGNi )
			m_size += (maxlen->inode_mlen+4) ;

		if( sign_sum & SIGNs )
			m_size += maxlen->space_mlen+2 ;

		Div_ls(phead,len,m_size);
	}
	else
	{
		Show_ll(phead,maxlen);
	}
	chdir(cw_dir[--dir_count]);
	if( sign_sum & SIGNR )
		printf("\n");
}

static int IsEmpty(Plist phead)
{
	return phead->next == NULL ;
}

static void List_Sort_TS(Plist phead)
{
	assert( phead != NULL ) ;
	if( IsEmpty(phead) )
		return ;
	File_Node *p = phead->next ;
	File_Node *q ;
	int len = GetLength(phead) ;
	int i = 0 ;
	int p_tmp ;
	int q_tmp ;
	for( ; i < len-1 ; ++i )
	{
		p = phead->next ;
		while( p->next != NULL )
		{
			q = p->next ; 
			if( sign_sum & SIGNt )
			{
				p_tmp = p->mtime ;
				q_tmp = q->mtime ;
			}
			else if( sign_sum & SIGNu )
			{
				p_tmp = p->atime;
				q_tmp = q->atime ;
			}
			else if( sign_sum & SIGNc )
			{
				p_tmp = p->ctime ;
				q_tmp = q->ctime ;
			}
			else if( sign_sum & SIGNS )
			{
				p_tmp = p->size ;
				q_tmp = q->size ;
			}

			if( p_tmp < q_tmp )
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

static void List_Sort_Lastname(Plist phead)
{
	assert( phead != NULL ) ;
	if( IsEmpty(phead) )
		return ;
	List_Sort_Filename(phead);
	File_Node *p ;
	File_Node *q ;
	int len = GetLength(phead);
	int i = 0 ;

	for( ; i < len-1 ; ++i )
	{
		p = phead->next ;
		while( p->next != NULL )
		{
			q = p->next ; 
			if( strcmp(p->lastname,q->lastname) > 0 )
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

static void List_Sort_Filename(Plist phead)
{
	assert( phead != NULL ) ;
	if( IsEmpty(phead) )
		return ;
	File_Node *p ;
	File_Node *q ;
	int len = GetLength(phead) ;
	int i = 0 ;

	for( ; i < len-1 ; ++i )
	{
		p = phead->next ;
		while( p->next != NULL )
		{
			q = p->next ; 
			if( strcmp(p->filename,q->filename) > 0 )
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

void Reverse(Plist phead)
{
	assert( phead != NULL );
	File_Node *p = phead->next ;
	File_Node *q ;
	phead->next = NULL ;
	phead->prio = NULL ;
	while( p != NULL )
	{
		q = p->next ;
		p->next = phead->next ;
		phead->next = p ;
		if( p->next != NULL )
			p->next->prio = phead ;
		p->prio = phead ;
		p = q ;
	}
}

void Destroy(Plist phead)
{
	assert( phead != NULL ) ;
	File_Node *p = phead->next ;
	File_Node *q ;

	while( p != NULL )
	{
		q = p->next ;
		p->next = NULL ;
		p->prio = NULL ;
		free(p) ;
		p = q ;
	}
}
