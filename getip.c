#include <windows.h>
#include <stdio.h>
#include <ras.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <rpc.h>
#include "getip.h"

MyStrFunc docase=me;
char *me(char *in) { return in; }
unsigned char tcase=NOCASE;
int retvalue=0;

void getowner(char *resource,char *varname)
{
	typedef DWORD (__stdcall*WNGetUserFunc)(LPCTSTR, LPTSTR, LPDWORD);  
	
	HINSTANCE mprdll;
	WNGetUserFunc WNetGetUser;
	char *UserName;
	unsigned long i=sz;
	int ret;

	if ((UserName=(char *)malloc(sizeof(char)*sz))==NULL) return;
	mprdll=LoadLibrary ("MPR.DLL");
	if ((WNetGetUser=(WNGetUserFunc)GetProcAddress(mprdll,"WNetGetUserA"))==NULL)
		printf("rem Error: Could not get WNGetUser() from MPR.DLL.  Return Value: %d\n",GetLastError());
	else 
	{
		i=sz;
		if ((ret=WNetGetUser(resource,UserName,&i))!=0)
			printf("rem Error: WNGetUser() returned %d.\n",ret);
		else printf("set %s=%s\n", varname,docase(UserName));
	};
}

void userinfo()
{
	typedef BOOL  (__stdcall*GUNameFunc) (LPTSTR, LPDWORD); 
  
	HINSTANCE advdll;
	GUNameFunc GetUserName;
	unsigned long i=sz;
	char *UserName;
	
	if ((UserName=(char *)malloc(sizeof(char)*sz))==NULL) return;
	advdll=LoadLibrary ("ADVAPI32.DLL");
	if ((GetUserName=(GUNameFunc)GetProcAddress(advdll,"GetUserNameA"))==NULL)
		printf("rem Error: Could not get GetUserName() from ADVAPI32.DLL.  Return Value: %d\n",GetLastError());
	else 
	{
		if (GetUserName(UserName,&i)==0)
		{
			printf("rem Error: GetUserName() returned %d.\n",GetLastError());
			getowner("z:","USERNAME");
		}
		else printf("set USERNAME=%s\n", docase(UserName));
	};	
}

void dateinfo(int what, time_t *usertime, char *userprefix)
{
	struct tm *newtime;
	char *prefix="";
	time_t when;

	if (userprefix!=NULL) prefix=userprefix;
	if (usertime!=NULL) when=*usertime; 
		else time(&when);
	newtime = localtime( &when );
	tzset();
	switch ( what ){
		case modTIME: {printf("set %sTIME=%.2d:%.2d:%.2d\n",prefix,newtime->tm_hour,newtime->tm_min,newtime->tm_sec ); break;};
		case modDATE: {printf("set %sDATE=%.4d.%.2d.%.2d\n",prefix,newtime->tm_year+1900,newtime->tm_mon+1,newtime->tm_mday ); break;};
		case modCTIME:{printf("set %sCTIME=%s\n",prefix,notrail(docase(asctime ( newtime )))); break;};
		case modTZONE:{printf("set %sDAYLIGHT=%s\nset TIMEZONE=%s\n",prefix,docase(_daylight?"YES":"NO"),docase(_tzname[0])); break;};
	};
}

void rasinfo()
{
    typedef DWORD (__stdcall*RASFUNC)(LPRASCONN, LPDWORD, LPDWORD);  
	RASFUNC RasEnumConnections;
	HINSTANCE rasdll;
	RASCONN *rasdata;
	unsigned long size;
	unsigned long connections;
	unsigned int i=81;
	unsigned int nRet;
	rasdll=LoadLibrary ("RASAPI32.DLL");
	if ((RasEnumConnections=(RASFUNC)GetProcAddress(rasdll,"RasEnumConnectionsA"))==NULL)
			printf("set CONN_TOT=0\n");
	else 
	{
		rasdata = (RASCONN*) GlobalAlloc(GPTR, sizeof(RASCONN));
		rasdata->dwSize = sizeof(RASCONN);
		size = sizeof(*rasdata);
  
		if ((nRet=RasEnumConnections(rasdata,&size,&connections))!=0) 
  			printf("rem RasEnumConnections failed: Error = %d\n", nRet);
		else
		{
			printf("set CONN_TOT=%d\n",connections);
			for (i=0;i<connections;i++) 
			{
				printf("set CONN_%d=%s\n",i,docase(rasdata->szEntryName));
				rasdata++;
			};
		};
	}
}

void hostinfo()
{
	char szHostName[MAXHOSTNAMELEN];
	struct hostent  *heHostEnt;
	WSADATA wsaData;
	typedef int  (__stdcall*GHNameFunc) (char FAR*, int); 
    typedef struct hostent FAR* (__stdcall*GHbyNameFunc) (const char FAR*);
	typedef int	 (__stdcall*WSAFunc) (WORD, LPWSADATA);

	WSAFunc		WSAStartup;
	GHNameFunc	gethostname;
	GHbyNameFunc	gethostbyname;
	
	HINSTANCE ws2dll;
	
	ws2dll=LoadLibrary ("WS2_32.DLL");
	if ((gethostname=(GHNameFunc)GetProcAddress(ws2dll,"gethostname"))==NULL)
		ws2dll=LoadLibrary ("WSOCK32.DLL");
	if ((gethostname=(GHNameFunc)GetProcAddress(ws2dll,"gethostname"))==NULL)
		printf("rem Error: Could not get gethostname() from WS2_32.DLL or WSOCK32.DLL.  Return Value: %d\n",GetLastError());
	else 
	if ((gethostbyname=(GHbyNameFunc)GetProcAddress(ws2dll,"gethostbyname"))==NULL)
		printf("rem Error: Could not get gethostbyame() from WS2_32.DLL or WSOCK32.DLL.  Return Value: %d\n",GetLastError());
	else
	if ((WSAStartup=(WSAFunc)GetProcAddress(ws2dll,"WSAStartup"))==NULL)
		printf("rem Error: Could not get WSAStartup() from WS2_32.DLL or WSOCK32.DLL.  Return Value: %d\n",GetLastError());
	else
	{
		WSAStartup(MAKEWORD(1, 1), &wsaData); 
		if (gethostname(szHostName, MAXHOSTNAMELEN))
			printf("Error: gethostname() failed.\n");
		else
		{
			if ((heHostEnt = gethostbyname(szHostName)) == NULL)
				printf("Error: gethostbyname() failed.\n");
			else 
				printf(
				"set IP=%d.%d.%d.%d\n"
				"set NET_ID=%d\n"
				"set HOSTNAME=%s\n",
				(int) (unsigned char) heHostEnt->h_addr_list[0][0],
				(int) (unsigned char) heHostEnt->h_addr_list[0][1],
				(int) (unsigned char) heHostEnt->h_addr_list[0][2],
				(int) (unsigned char) heHostEnt->h_addr_list[0][3],
				(int) (unsigned char) heHostEnt->h_addr_list[0][2],
				docase(szHostName)
				);
		}
	}
}	

int parsepath(const char *s)
{
	char *t;
	char *a;
	enum {SERVER,SHARE,DRIVE,FILEPATH,FILENAME};
	char *SET_Str[5]={	"set SERVER=%s\n",
						"set SHARE=%s\n",
						"set DRIVE=%c\n",
						"set FILEPATH=%s\n",
						"set FILENAME=%s\n"
					};
	int i;
	a=docase(strdup(s));
	t=strstr(a,"\\\\");
	if (t==a) 
	{
		if (t[i=strcspn((t+=2),"\\")]=='\\')
		{
			*(t+i)=0;
			printf(SET_Str[SERVER],t);
			if (t[i=strcspn(t+=i+1,"\\")]=='\\') 
			{
				char *path,*file;
				*(t+i)=0;
				printf(SET_Str[SHARE],t);
				if (*(path=(t+=i+1)))
				{
					if (file=strrchr(path,'\\')) *file++=0;
					if (*path) printf(SET_Str[FILEPATH],path);
					if (file) if (*file) printf(SET_Str[FILENAME],file);
				}
			}
			else printf(SET_Str[SHARE],t);
	
		}
		else printf(SET_Str[SERVER],t);
	}
	else
	{ 
		t=strstr(a,":");
		if (t==a+1)
		{
			char *path,*file;
			printf(SET_Str[DRIVE],t[-1]);
			path=(t+=2);
			if (*(path))
			{
				if (file=strrchr(path,'\\')) *file++=0;
				if (*path) printf(SET_Str[FILEPATH],path);
				if (file) if (*file) printf(SET_Str[FILENAME],file);
			}
		}
	
	};
	free(a);
	return EXIT_SUCCESS;
}

void GetConnection(char opt, char *local)
{
    typedef DWORD (__stdcall*MPRFUNC)(LPCTSTR, LPTSTR, LPDWORD);  
	MPRFUNC WNetGetConnection;
	HINSTANCE mprdll;
	
	if (opt=='u')
	{
		getowner(local,"OWNER");
		return ;
	}

	mprdll=LoadLibrary ("MPR.DLL");
	if ((WNetGetConnection=(MPRFUNC)GetProcAddress(mprdll,"WNetGetConnectionA")))
	{
		char *remote;
		int size=256;
		if ((remote=(char *)malloc(size))==NULL) return;
		if (WNetGetConnection(local,remote,&size)==0)
		{
			if (opt=='r') printf("set RESOURCE=%s\n",remote);
			if (opt=='p') parsepath(remote);
		};
		free(remote);
	}
}
int comparefiles(int fd[2], enum FUNCS funct)
{
	struct stat info[2];
	int result,i,missing=0;

	if ((fd[0]==-1) && (fd[1]==-1)) return 3;
	if (fd[0]==-1) return 1; 
	if (fd[1]==-1) return 0; 
	
	for (i=0; i<2 ; i++)
		if ((result=fstat( fd[i], &info[i]))!=0)
		{
			printf("rem Error: %s\n",strerror(errno));
			return 2;
		};
	if (funct==newer)
	{
		if (info[0].st_ctime==info[1].st_ctime)
			return 3; 
		if (info[1].st_ctime>info[0].st_ctime)
			return 1;
	};
	if (funct==bigger)
	{
		if (info[0].st_size==info[1].st_size)
			return 3;
		if (info[1].st_size>info[0].st_size)
			return 1;
	};
	return 0;
}

char *notrail(char *in)
{
	in[strlen(in)-1]=0;
	return in;
}

void filestuff(int *cur_char, int *cur_word, const char **argv)
{
	char *filename[4]={NULL,NULL,docase("*Error*"),docase("*None*")};
	enum FUNCS dofunc=unknown;
	
	filename[0]=filename[1]=NULL;
		
	(*cur_char)++;
	switch (argv[(*cur_word)][(*cur_char)]){
		case 'b': { dofunc=bigger; break; }
		case 'n': { dofunc=newer; break; }
		case 'd': { dofunc=getdate; break; }
		case 'c': { dofunc=getfctm; break; }
		case 't': { dofunc=gettime; break; }
		case 's': { dofunc=getsize; break; }
		case 'p': { dofunc=parse; break; }
		default:  dofunc=unknown;
	}

	if (dofunc==unknown){
		printf("rem Incorrect usage of f param.\n");
		goto quit;
	}
	if (argv[++(*cur_word)]==NULL) goto quit;
	filename[0]=docase(strdup(argv[*cur_word]));
	if (dofunc & COMPARE) 
	{
		int err[2], fd[2];
		char *compstr[2]={"set BIGGER=%s\n","set NEWER=%s\n"};
		if (argv[++(*cur_word)]==NULL) goto quit;
		filename[1]=docase(strdup(argv[*cur_word]));
		fd[0]=open(filename[0],O_RDONLY);err[0]=errno;
		fd[1]=open(filename[1],O_RDONLY);err[1]=errno; 
		retvalue=comparefiles(fd,dofunc);
			if  (((fd[0]==-1) && (err[0]!=ENOENT)) || ((fd[1]==-1) && (err[1]!=ENOENT)))
			{	
				printf("rem Error %i: %s\n",err[0], strerror(err[0]));
				printf(compstr[dofunc-COMPARE-1],filename[retvalue=2]);
			}
			else printf(compstr[dofunc-COMPARE-1],filename[retvalue]);
		if (retvalue==3) retvalue=0;
	} 
	else
	{
		switch(dofunc){
			case parse: {parsepath(filename[0]);break;}	
			case getdate:
			case gettime:
			case getsize:
			case getfctm:
				{
					int fd;
					struct stat info;
					int result;

					fd=open(filename[0],O_RDONLY);
					if ((result=fstat( fd, &info))!=0)
						{
							printf("rem Could not get handle on file %s.\n",filename[0]);
							goto quit;
						};
					if (dofunc==getfctm) dateinfo(modCTIME,&info.st_ctime,"FILE");
					if (dofunc==getdate) dateinfo(modDATE,&info.st_ctime,"FILE");
					if (dofunc==gettime) dateinfo(modTIME,&info.st_ctime,"FILE");
					if (dofunc==getsize) printf("set FILESIZE=%i\n",info.st_size);
					break;
				} 
		}
	};

quit:
	*cur_char=0;
	if (filename[1]!=NULL) 
	{
		free(filename[0]);
		free(filename[1]);
	}
	else 
		if (filename[0]!=NULL) 
			free(filename[0]);
}

void getregkey(char *where)
{
	typedef LONG (__stdcall*RegQFunc) (HKEY, LPTSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD); 
	typedef LONG (__stdcall*RegOFunc) (HKEY, LPCTSTR, DWORD, REGSAM, PHKEY); 
	typedef LONG (__stdcall*RegCFunc) (HKEY); 
	RegQFunc RegQueryValueEx;
	RegOFunc RegOpenKeyEx;
	RegCFunc RegCloseKey;
	HINSTANCE advlib;
	HKEY Result; 

	char *answer, *holder;
	unsigned long size;
	unsigned long type;
	char *hkeys[4]={"HKEY_CLASSES_ROOT","HKEY_CURRENT_USER","HKEY_LOCAL_MACHINE","HKEY_USERS"};
	char *keypath=strdup(where);
	char *query=strrchr(keypath,'\\');
	int i;

	if (query) *query++=0; else {printf("set REGVAL=**novalue**\n");return;};
	
	_strupr(keypath);
	for (i=0;i<4;i++)
	{
		if (keypath==strstr(keypath,hkeys[i]))
		{ 
			Result=((HKEY)(0x80000000+i));//i'm sure this is bad
			//Result=(HKEY_CLASSES_ROOT+i);
			keypath+=1+strlen(hkeys[i]);
		}
	}
	advlib=LoadLibrary ("ADVAPI32.DLL");
	if ((RegQueryValueEx=(RegQFunc)GetProcAddress(advlib,"RegQueryValueExA"))==NULL)
	{
		printf( "rem Could not get RegQueryValueExA from ADVAPI32.DLL\n");
		return;
	}
	if ((RegOpenKeyEx=(RegOFunc)GetProcAddress(advlib,"RegOpenKeyExA"))==NULL)
	{
		printf( "rem Could not get RegOpenKeyExA from ADVAPI32.DLL\n");
		return;
	}
	if ((RegCloseKey=(RegCFunc)GetProcAddress(advlib,"RegCloseKey"))==NULL)
	{
		printf( "rem Could not get RegcloseKey from ADVAPI32.DLL\n");
		return;
	}
	size=256; holder=answer=(char *)malloc(size);
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, keypath, 0, KEY_READ, &Result)!=0) {
		printf("set REGVAL=**nokey**\n");
		return;
	}
	if (RegQueryValueEx(Result,query,NULL,&type,answer,&size)!=0){
		printf("set REGVAL=**novalue**\n");
		return;
	}
	RegCloseKey(Result);
	switch (type)
	{
		case REG_SZ:
		case REG_EXPAND_SZ: {printf("set REGVAL=%s\n",answer); break;}
		case REG_MULTI_SZ:
			{
				int s;
				printf("set REGVAL={%s",answer);
				while (s=strlen(answer)) 
				{
					answer+=s; 
					printf("%s",answer);
				};
				printf("}\n");
				break;
			}
		case REG_DWORD_LITTLE_ENDIAN:{printf("set REGVAL=%i\n",*answer);break;}
		case REG_BINARY:
		case REG_DWORD_BIG_ENDIAN:
		case REG_LINK:
		case REG_NONE:
		case REG_RESOURCE_LIST:{printf("set REGVAL=**can't display**\n");break;}
	};
	free(holder);
}

void getarp()
{
	typedef RPC_STATUS RPC_ENTRY UuidFunc(UUID*);
	UuidFunc *UuidCreate;

	HINSTANCE rpclib;
	UUID uuid;

	rpclib=LoadLibrary ("RPCRT4.DLL");
	if ((UuidCreate=(UuidFunc*)GetProcAddress(rpclib,"UuidCreate"))==NULL)
	{
		printf( "rem Could not get UuidCreate from RPCRT4.DLL\n");
		return;
	}
	
    if ( UuidCreate(&uuid) == RPC_S_OK )
		printf("set NODE=%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
			uuid.Data4[2],
			uuid.Data4[3],
			uuid.Data4[4],
			uuid.Data4[5],
			uuid.Data4[6],
			uuid.Data4[7]
			);
}
