#ifdef USE_NT
#include <winnetwk.h>
#include <winnls.h>

void moreuserinfo()
{
	typedef DWORD (__stdcall*WNGetProviderNameFunc)(DWORD, LPTSTR, LPDWORD);  
	WNGetProviderNameFunc WNetGetProviderName;
	struct _USER_INFO_2 *UserInfo;
	WCHAR *wszUserName;
	wszUserName=(WCHAR *)malloc(sizeof(WCHAR)*sz);
	LPBYTE ServerName;
	char temp[sz];
	
	WCHAR wszDomain[256]; 
	char ProviderName[500];
	 
	if ((WNetGetProviderName=(WNGetProviderNameFunc)GetProcAddress(mprdll,"WNetGetProviderNameA"))==NULL)
	fprintf(stdout,"rem Error: Could not get WNGetProviderName() from MPR.DLL.  Return Value: %d\n",GetLastError());
	else 
	{
		if ((ret=WNetGetProviderName(0,ProviderName,&i))!=0)
			fprintf(stdout,"rem Error: WNGetProviderName() returned %d.\n",ret);
		else printf("set DOMAINNAME=%s\n", ProviderName);
	};
	MultiByteToWideChar( CP_ACP, 0, UserName, strlen(UserName)+1, wszUserName, sizeof(wszUserName) );
	MultiByteToWideChar( CP_ACP, 0, a, strlen(a)+1, wszDomain, sizeof(wszDomain) );
	NetGetAnyDCName(NULL, NULL, &ServerName);
	WideCharToMultiByte( CP_ACP, 0,(LPWSTR)  ServerName, -1,temp, sz, NULL, NULL );
	printf("set SERVER=%s\n",docase(temp));
	UserInfo=(struct _USER_INFO_2*) malloc(sizeof(struct _USER_INFO_2));
	ret=NetApiBufferAllocate(sizeof(struct _USER_INFO_2),&UserInfo);
	if (ret == 0)
	{
		ret=NetUserGetInfo((LPWSTR) ServerName,(LPWSTR) &wszUserName,2,(LPBYTE*)&UserInfo);
		if (ret != 0)
			fprintf(stdout, "rem Error: NetUserGetInfo() returned %d.\n",ret);
		else 
		{
			WideCharToMultiByte( CP_ACP, 0, UserInfo->usri2_full_name, -1, temp, sz, NULL, NULL );
			printf("set FULLNAME=%s\n",docase(temp));
			WideCharToMultiByte( CP_ACP, 0, UserInfo->usri2_home_dir, -1,  temp, sz, NULL, NULL );
			printf("set HOMEDIR=%s\n",docase(temp));
			WideCharToMultiByte( CP_ACP, 0, UserInfo->usri2_logon_server, -1, temp, sz, NULL, NULL );
			printf("set LOGONSERVER=%s\n",docase(temp));
			WideCharToMultiByte( CP_ACP, 0, UserInfo->usri2_script_path, -1, temp, sz, NULL, NULL );
			printf("set SCRIPT=%s\n",docase(temp));
		}
		NetApiBufferFree(UserInfo);
	}
	else fprintf (stdout,"rem Error: NetApiBufferAllocate() returned %d.\n",ret);
};
#endif