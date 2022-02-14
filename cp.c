/**
 * File              : cp.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.09.2021
 * Last Modified Date: 13.10.2021
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "cp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>


bool file_exists (char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

int cp(const char *from, const char *to, bool owerwrite)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

	if (owerwrite) {
		fd_to = open(to, O_WRONLY | O_CREAT , 0666);
	}
	else {
		fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
	}
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}

#if defined _WIN32 || defined _WIN64
#else
int create_dir_if_not_exists(const char *dir){
	printf("Try to create DIR: %s ...\n", dir);
	struct stat st = {0};
	if (stat(dir, &st) == -1) {
		int erro = mkdir(dir, 0700); //create dir
		if (erro == -1) {
			fprintf(stderr,"ERROR to to create DIR: %s\n", dir);
			return -1;
		}			
		else{
			printf("OK\n");
		}
	}	
	else {	
		printf("Directory exists\n");
	}

	return 0;
}


int cp_recursive(const char *from, const char *to, bool owerwrite){
	int erro = 0;

	DIR *dp;
	dp = opendir(from);
	if (dp == NULL) 
	{
		fprintf(stderr, "ERROR OPEN DIR %s\n", from);
		errno = ENOENT;
		return ENOENT;
	}

	//create to_dir, if not exists
	if (create_dir_if_not_exists(to)) {
		return -1;
	}


	struct dirent *entry;
	
	while((entry = readdir(dp))){ //reading files
		if (entry->d_type == DT_DIR){ //is directory
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
				
				char from_dir[1024];
				erro = snprintf(from_dir, 1023, "%s/%s", from, entry->d_name);				
				from_dir[1023] = '\0';
				if (erro == -1){
					fprintf(stderr, "ERROR to handle DIR: %s, error: %d\n", from_dir, erro);
					//return erro;
				}				
				
				char dest_dir[1024];
				erro = snprintf(dest_dir, 1023, "%s/%s", to, entry->d_name);
				dest_dir[1023] = '\0';
				if (erro == -1){
					fprintf(stderr, "ERROR to handle DIR: %s, error: %d\n", dest_dir, erro);
					//return erro;
				}				
				else {
					//create dest_dir, if not exists
					if (create_dir_if_not_exists(dest_dir)) {
						return -1;
					}
				}
				
				erro = cp_recursive(from_dir, dest_dir, owerwrite); 
				if (erro){
					fprintf(stderr, "ERROR to copy DIR: %s TO: %s\n", from_dir, dest_dir);
					return erro;
				}
			}
		}
		if (entry->d_type == DT_REG) { // if entry is a regular file
			char source[1024];
			erro = snprintf(source, 1023, "%s/%s", from, entry->d_name);
			source[1023] = '\0';				
			if (erro == -1){
				fprintf(stderr, "ERROR to handle FILE: %s, error: %d\n", source, erro);
				//return erro;
			}							
			
			char destination[1024];
			erro = snprintf(destination, 1023, "%s/%s", to, entry->d_name);
			destination[1023] = '\0';				
			if (erro == -1){
				fprintf(stderr, "ERROR to handle FILE: %s, error: %d\n", destination, erro);
				//return erro;
			}				
			else {
				printf("Try to copy %s: %s...\n", source, destination);
				if (cp(source, destination, owerwrite)) {
					fprintf(stderr, "ERROR to copy file: %s TO: %s\n", source, destination);
				}
				else {
					printf("OK\n");
				}
			}
		}
	}	

	return 0;
}
#endif
