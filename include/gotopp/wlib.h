/*

Implémentation d'une couche UNICODE pour l'I/O avec UNIX
vu que ces crétins n'ont rien défini de tel.

*/

#ifndef _GOTOPP_WLIB_H
#define _GOTOPP_WLIB_H

FILE * _wfopen(const wchar_t * file, const wchar_t * mode);
FILE * _wpopen(const wchar_t * file, const wchar_t * mode);
char * allocmbs(const wchar_t * str, size_t len); //use delete[]
wchar_t *allocws(const char *str, size_t len);
int wmain(int argc, wchar_t * argv[]);
#endif