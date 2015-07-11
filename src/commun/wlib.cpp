#ifdef _UNICODE
#ifndef _WIN32
#include <gotopp/base.h>

char * allocmbs(const wchar_t * str, size_t len)
{
	const wchar_t *pstr=str;
	size_t l=wcsrtombs(0,&pstr,len,0);
	char * ret=new char[l+1];
	pstr=str;
	wcsrtombs(ret,&pstr,len,0);
	ret[l]=0;
	return ret;
}

wchar_t * allocws(const char * str, size_t len)
{
	const char *pstr=str;
	size_t l=mbsrtowcs(0,&pstr,len,0);
	wchar_t * ret=new wchar_t[l+1];
	pstr=str;
	mbsrtowcs(ret,&pstr,len,0);
	ret[l]=0;
	return ret;
}

FILE * _wfopen(const wchar_t * file, const wchar_t * mode)
{
	char * _file=allocmbs(file,wcslen(file));
	char * _mode=allocmbs(mode,wcslen(mode));
	FILE * ret=fopen(_file,_mode);
	delete[]_file;
	delete[]_mode;
	return ret;
}

FILE * _wpopen(const wchar_t * file, const wchar_t * mode)
{
	char * _file=allocmbs(file,wcslen(file));
	char * _mode=allocmbs(mode,wcslen(mode));
	FILE * ret=popen(_file,_mode);
	delete[]_file;
	delete[]_mode;
	return ret;
}

int main(int argc, char **argv)
{
	typedef wchar_t* pwchar_t;
	wchar_t** _argv=new pwchar_t[argc];
	for (int a=0; a<argc; a++)
		_argv[a]=allocws(argv[a],strlen(argv[a]));
	wmain(argc,_argv);
	for (int a=0; a<argc; a++)
		delete[]_argv[a];
	delete[]_argv;
}
#endif
#endif
