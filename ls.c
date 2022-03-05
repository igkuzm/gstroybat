/**
 * File              : ls.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.02.2022
 * Last Modified Date: 05.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "ls.h"
#include <errno.h>
#include <string.h>
#if defined _WIN32 || defined _WIN64
#include <Windows.h>
#else
#include <dirent.h>
#endif

//list of files in directory
int k_lib_ls(const char *dir, void *user_data, int (*callback)(KLibFile *file, void *user_data)){
	char error[BUFSIZ];
#if defined _WIN32 || defined _WIN64
	WIN32_FIND_DATAA findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	char fullpath[BUFSIZ];
	sprintf(fullpath, "%s\\*", dir);

	hFind = FindFirstFileA(fullpath, &findData);
	if (hFind = INVALID_HANDLE_VALUE) {
		sprintf(error, "Can't open directory: %s", dir);
		return ENOENT;
	}
	while(FindNextFileA(hFind, &findData) != 0) {
		KLibFile file;
		file.filename = findData.cFileName; 
		file.filepath = dir;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			file.filetype = KLIBFILETYPE_DIR;
		else
			file.filetype = KLIBFILETYPE_FILE;
		
		if (callback(&file, user_data) != 0)
			return 1;
	}
	dwError = GetLastError();
	FindClose(hFind);
	if (dwError != ERROR_NO_MORE_FILES) {
		return dwError;
	}
#else	
	struct dirent *entry;
	DIR *dp;	

	dp = opendir(dir);
	if (dp == NULL){
		sprintf(error, "Can't open directory: %s", dir);
		perror(error);
		return ENOENT;
	}	
	while((entry = readdir(dp))){ //reading files
		KLibFile file;
		file.filename = entry->d_name; 
		file.filepath = dir;
		switch (entry->d_type) {
			case  DT_REG: file.filetype = KLIBFILETYPE_FILE; break;
			case  DT_DIR: file.filetype = KLIBFILETYPE_DIR; break;
			case  DT_LNK: file.filetype = KLIBFILETYPE_LINK; break;
			default: file.filetype = KLIBFILETYPE_OTHER; break;
		}
		if (callback(&file, user_data) != 0)
			return 1;
	}
	closedir(dp);
#endif
	return 0;
}
