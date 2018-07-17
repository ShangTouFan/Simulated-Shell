#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

int Myatoi(const char *src)
{
	int num = 0 ;
	while(*src != '\0')
	{
		num = num*10 + *src - '0' ;
		++src;
	}
	return num ;
}

int IsDigit(const char *path)
{
	if(*path >= 0 && *path <= 9)
		return 1 ;
	return 0 ;
}

void Chmod_Digit(const char *path,int mod)
{
	int mod_u,mod_g,mod_o;
	if(mod > 777 || mod < 0)
	{
		printf("mod num error\n");
		exit(0);
	}
	mod_u = mod/100 ;
	mod_g = (mod-mod_u*100)/10 ;
	mod_o = mod-mod_u*100-mod_g*10 ;
	mod = mod_u*8*8 + mod_g*8 +mod_o ;
	if( chmod(path,mod) == -1)
	{
		perror("chmod");
		exit(0);
	}
}

void Get_mode(int u,int g,int o,int r,int w,int x,int sign,mode_t *mode)
{
    switch(sign)  
    {  
        case '+':  
            if(u)  
            {  
                if(r)  
                    *mode |= S_IRUSR;  
                if(w)  
                    *mode |= S_IWUSR;  
                if(x)  
                    *mode |= S_IXUSR;  
            }  
  
            if(g)  
            {  
                if(r)  
                    *mode |= S_IRGRP;  
                if(w)  
                    *mode |= S_IWGRP;  
                if(x)  
                    *mode |= S_IXGRP;  
            }  
  
            if(o)  
            {  
                if(r)  
                    *mode |= S_IROTH;  
                if(w)  
                    *mode |= S_IWOTH;  
                if(x)  
                    *mode |= S_IXOTH;  
            }  
            break;  
  
    case '-':  
            if(u)  
            {  
                if(r)  
                    *mode &= ~S_IRUSR;  
                if(w)  
                    *mode &= ~S_IWUSR;  
                if(x)  
                    *mode &= ~S_IXUSR;  
            }  
  
            if(g)  
            {  
                if(r)  
                    *mode &= ~S_IRGRP;  
                if(w)  
                    *mode &= ~S_IWGRP;  
                if(x)  
                    *mode &= ~S_IXGRP;  
            }  
  
            if(o)  
            {  
                if(r)  
                    *mode &= ~S_IROTH;  
                if(w)  
                    *mode &= ~S_IWOTH;  
                if(x)  
                    *mode &= ~S_IXOTH;  
            }  
            break;  
  
        case '=':  
            if(u)  
            {  
                *mode &= (S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);  
                if(r)  
                    *mode |= S_IRUSR;  
                if(w)  
                    *mode |= S_IWUSR;  
                if(x)  
                    *mode |= S_IXUSR;  
            }  
  
            if(g)  
            {  
                *mode &= (S_IRUSR|S_IWUSR|S_IXUSR|S_IROTH|S_IWOTH|S_IXOTH);  
                if(r)  
                    *mode |= S_IRGRP;  
                if(w)  
                    *mode |= S_IWGRP;  
                if(x)  
                    *mode |= S_IXGRP;  
            }  
  
            if(o)  
            {  
                *mode &= (S_IRGRP|S_IWGRP|S_IXGRP|S_IRUSR|S_IWUSR|S_IXUSR);  
  
                if(r)  
                    *mode |= S_IROTH;  
                if(w)  
                    *mode |= S_IWOTH;  
                if(x)  
                    *mode |= S_IXOTH;  
            }  
            break;  
  
        default:  
            printf("sign error\n");  
            break;  
    }  
}

void Chmod_Alpha(const char *path,const char *src)
{
	mode_t mode ;
	struct stat info ;
	int flag_u,flag_g,flag_o ;
	int r,w,x;
	int sign ;
	if(lstat(path,&info) == -1)
	{
		perror("lstat");
		exit(0);
	}
	mode = info.st_mode ;
	int i = 0 ;
	while( src[i] != '\0')  
        {  
            r = w = x = 0;  
            flag_u = flag_g = flag_o = 0;  
            while(src[i] != ',' && src[i] != '\0')  
            {  
                if(src[i] == 'u')  
                    flag_u = 1;  
                else if(src[i] == 'g')  
                    flag_g = 1;  
                else if(src[i] == 'o')  
					flag_o = 1;  
                else if(src[i] == 'a')  
                    flag_u = flag_g = flag_o = 1;  
                else if(src[i] == '+')  
                    sign = '+';  
                else if(src[i] == '-') 
					sign = '-';  
                else if(src[i] == '=')  
                    sign = '=';  
                else if(src[i] == 'r')  
                    r = 'r';  
                else if(src[i] == 'w') 
					w = 'w';  
                else if(src[i] == 'x')  
                    x = 'x';  
                i++;  
            }  
                Get_mode(flag_u, flag_g, flag_o, r, w, x, sign, &mode);  
                if(src[i] == ',')  
                    i++;  
		}  
 		if(chmod(path,mode) == -1)
		{
			perror("chmod");
			exit(0);
		}
}

int main(int argc,char *argv[])
{
	if(argc < 3)
	{
		printf("missing operand\n");
		exit(0);
	}
	int k = 2 ;
	for( ; k < argc ; ++k )
	{
		char *path = argv[k];
		if( IsDigit(argv[1]))
		{
			int mod = Myatoi(argv[1]);
			Chmod_Digit(path,mod);
		}
		else
			Chmod_Alpha(path,argv[1]);
	}
	return 0 ;
}
