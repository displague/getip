/* ipaddr.c */

#include <winsock2.h>
#include <stdio.h>
#include <ras.h>
#include <winbase.h>

#define MAXHOSTNAMELEN 80
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void userinfo()
{
	unsigned long *i;
	char UserName[81];
	if (GetUserName(UserName,i)==1)
		fprintf(stderr,"Error: GetUserName() failed.\n");
	else printf("set USERNAME=%s\n", UserName);
}

void rasinfo()
{
	typedef DWORD (*RASFUNC)(LPRASCONN, LPDWORD, LPDWORD);
	RASFUNC RasEnumConnections;
	HINSTANCE rasdll;
	RASCONN *rasdata;
	unsigned long size;
	unsigned long connections;
	unsigned int i;
	unsigned int nRet;
	rasdll=LoadLibrary ("RASAPI32.DLL");
	if ((RasEnumConnections=(RASFUNC)GetProcAddress(rasdll,"RasEnumConnectionsA"))==NULL)
		fprintf(stderr,"Error: Could not get RasEnumConnections from RASAPI32.DLL\n");
	else 
	{
		rasdata = (RASCONN*) GlobalAlloc(GPTR, sizeof(RASCONN));
		rasdata->dwSize = sizeof(RASCONN);
		size = sizeof(*rasdata);
  
		if ((nRet=RasEnumConnections(rasdata,&size,&connections))!=0) 
  			printf("RasEnumConnections failed: Error = %d\n", nRet);
		else
		{
			printf("set CONN_TOT=%d\n",connections);
			for (i=0;i<connections;i++) 
			{
				printf("set CONN_%d=\"%s\"\n",i,rasdata->szEntryName);
				rasdata++;
			};
		};
//		if (FreeLibrary(rasdll)!=0) fprintf(stderr,"Error: could not unload ras32api.dll\n");
	}
}

void hostinfo()
{
	char szHostName[MAXHOSTNAMELEN];
	struct hostent  *heHostEnt;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(1, 1), &wsaData); 
	if (gethostname(szHostName, MAXHOSTNAMELEN))
		fprintf(stderr,"Error: gethostname() failed.\n");
	else
	{
		if ((heHostEnt = gethostbyname(szHostName)) == NULL)
			fprintf(stderr,"Error: gethostbyname() failed.\n");
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
			 szHostName
			);
	}
}

int main(argc,argv)
int argc;
char **argv;
{
	int i=0;
	int t=1;
	
	if (argc==1){
		printf(	"Usage: getip [hru]\t\tGetip v.2 1998/12/17\n\n"
				"\th:\tHost Info\t(ip, net_id, hostname)\n"
				"\tr:\tRAS Info\t(connection total and names)\n"
				"\tu:\tUser Info\t(username, domain name)\n"
				"\n"
				"getip is free software; you can redistribute it and/or"
				"modify it under the terms of the GNU Library General Public License as"
				"published by the Free Software Foundation.Written by Marques Johansson for SFWMD\n"
			  );
		exit(EXIT_FAILURE);
	}
	while (t<argc){
		while (argv[t][i]!='\0'){
//			printf("[%d][%d]=[%c]\n",t,i,argv[t][i]);
			switch (argv[t][i]){
				case 'u':
				case 'U': { userinfo(); break; }
				case 'h':
				case 'H': { hostinfo(); break; }
				case 'r':
				case 'R': { rasinfo(); break; }
			};
			i++;
		}
		t++;
		i=0;
	}

  return EXIT_SUCCESS;
}
