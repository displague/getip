#include <time.h>

#define VERSION	".4b"
#define MYINFO "Getip v"VERSION" 1999.02.03\tby Marques Johansson <displague@hotmail.com>\n"

#define sz 256
#define MAXHOSTNAMELEN 80
#ifndef EXIT_SUCCESS
enum { EXIT_SUCCESS, EXIT_FAILURE }
#endif

enum { modTIME, modDATE, modCTIME, modTZONE };
enum { NOCASE, LOWER, UPPER};
enum FUNCS {unknown, getdate, gettime, getsize, getfctm, parse, COMPARE=80, bigger,newer};

typedef char * (__cdecl*MyStrFunc)(char *);

void getowner(char *,char *);
void userinfo();
void dateinfo(int, time_t *, char *);
void rasinfo();
void hostinfo();
void help();
int parsepath(const char *);
int comparefiles(int[2], enum FUNCT);
char *notrail(char *);
void filestuff(int *, int *, const char **);
void getregkey(char *);
void GetConnection(char, char *);
void getarp();
char *me(char *);
MyStrFunc docase;

int retvalue;
unsigned char tcase;