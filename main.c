#include <windows.h>
#include <stdio.h>
#include "getip.h"

void help()
{
	printf(	docase(MYINFO"Usage: getip [dhrtTuzZ.?][f|pbndtz file1 {file2}] [R regkey] [o|rp X:]\n\n"
			"\t?\tThis usage information\n"	
			"\tv\tVersion\t(myver)\n"
			"\th\tHost Info\t(ip, net_id, hostname)\n"
			"\tm\tMAC Address\t(node)\n"
			"\tr\tRAS Info\t(connection total and names)\n"
			"\tR\tRegistry Key\t(regval)\n"
			"\tu\tUser Info\t(username)\n"
			"\td|t|c|T\tdate, time, ctime, timezone respectively\n"
			"\tZ|z|.\tupper, lower, and default case values respectively\n"
			"\to\t[urp] Network Resource: user, resource, or parsed\n"
			"\tf\t[pbndts] File: parse, bigger, newer, date, time, size\n"
			"\n"
			"Getip is free software; you can redistribute it and/or modify it under\n"
			"the terms of the GNU Library General Public License as published by the\n"
			"Free Software Foundation.\n")
		  );
	exit(EXIT_FAILURE);
}

int main(int argc,char **argv)
{
	int cur_char=0;
	int cur_word=1;
	int t;

	if (argc==1) help();
	while (cur_word<argc){
		t=cur_word;
		while (argv[cur_word][cur_char]!='\0'){
			switch (argv[cur_word][cur_char]){
				case 'z': { docase=_strlwr; break; }
				case 'Z': { docase=_strupr; break; }
				case '.': { docase=me; break; }
				case '?': { help(); break; }	
				case 'v': { printf("set MYVER="VERSION"\n"); break; }
				case 'm': { getarp(); break; }
				case 'f': { filestuff(&cur_char,&cur_word,argv); break; }
				case 'u':
				case 'U': { userinfo(); break; }
				case 'h':
				case 'H': { hostinfo(); break; }
				case 'r': { rasinfo(); break; }
				case 'R': { 
							if (argc>cur_word+1) getregkey(argv[++cur_word]);
							cur_char=0;
							break;
						  }
				case 't': { dateinfo(modTIME,NULL,NULL); break; }
				case 'o': {
							if (argc>cur_word) 
								{
									char t=argv[cur_word][++cur_char];
									GetConnection(t,argv[cur_word+1]);
									cur_word++;
								}
							cur_char=0;
							break;
						  }
				case 'c': { dateinfo(modCTIME,NULL,NULL); break; }
				case 'T': { dateinfo(modTZONE,NULL,NULL); break; }
				case 'd':
				case 'D': { dateinfo(modDATE,NULL,NULL); break; }
			};
			if (cur_word>t) break; 
			cur_char++;
		}
		cur_word++;
		cur_char=0;
	}
 return retvalue;
}
