/**
 * File              : openfile.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 18.09.2021
 * Last Modified Date: 30.09.2021
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "openfile.h"
#include <strings.h>
#include <stdlib.h>
#if defined _WIN32 || defined _WIN64
#include <windows.h>
#endif


int openFile(char *filename){
	char filePath[1024];
#ifdef __APPLE__
	sprintf(filePath, "open %s", filename);	
	system(filePath);
#elif defined _WIN32 || defined _WIN64
	ShellExecute(NULL, "open", filename, NULL, NULL, SW_SHOWDEFAULT);
#else
	sprintf(filePath, "xdg-open %s", filename);	
	system(filePath);
#endif	
	
	return 0;
}

