/**
 * File              : chworkdir.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 21.02.2022
 * Last Modified Date: 06.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "chworkdir.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#ifdef __APPLE__
#elif defined _WIN32 || defined _WIN64
#include <Windows.h>
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#endif

#ifdef __APPLE__
#elif defined _WIN32 || defined _WIN64
#else
int cp(const char *from, const char *to)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

	if (stat(workDir, &st) == -1)
		fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
	else
		return 1;
    
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

//list of files in directory
int copy_recursive(const char *source, const char *destination, int depth){
	char error[BUFSIZ];
	struct dirent *entry;
	DIR *dp;	

	dp = opendir(dir);
	if (dp == NULL){
		sprintf(error, "Can't open directory: %s", dir);
		perror(error);
		return ENOENT;
	}	
	while((entry = readdir(dp))){ //reading files
		char *filename = entry->d_name; 
		char source_file[BUFSIZ];
		sprintf("%s/%s", source, filename);
		char dest_file[BUFSIZ];
		sprintf("%s/%s", destination, filename);
		if (depth < 10) {
			switch (entry->d_type) {
				case  DT_REG: cp(source_file, dest_file); break;
				case  DT_LNK: cp(source_file, dest_file); break;
				case  DT_DIR: depth++; copy_recursive(source_file, dest_file, depth); break;
				default: break;
			}
		}
	}
	closedir(dp);
	return 0;
}

#endif

int changeWorkDir(char *argv[]){
	char *executable=dirname((char *)argv[0]);
	char workDir[BUFSIZ];
#ifdef __APPLE__
		sprintf(workDir, "%s%s", executable, "/../Resources"); //workdir for Apple is BUNDLE/../Resources
#elif defined _WIN32 || defined _WIN64
		sprintf(workDir, "%s", executable); //workdir for Windows is executable dir
#else
		//for Unix systems
		//find homedir and create new dir
		char *progname = basename(argv[0]);
		sprintf(workDir, "%s/%s", getenv("HOME"), progname); 
		
		//create directory if not exists
		struct stat st;
		if (stat(workDir, &st) == -1) {
			if (mkdir(workDir, 0700) == -1) {
				perror("Can't create directory at $(HOME)");
			}			
		}

		//find executable path
		char selfpath[128];
		memset (selfpath, 0, sizeof (selfpath));
		if (readlink ("/proc/self/exe", selfpath, sizeof (selfpath) - 1) < 0)
			{
				perror ("first readlink");
				exit (EXIT_FAILURE);
			};
		execDir=dirname(selfpath);

		char resourceDir[BUFSIZ];
		sprintf(resourceDir, "%s/../share/%s", execDir, progname); //resources dir


		//copy files from resources to workdir
		int copy_recursive(resourceDir, workDir, 0);
		
#endif
	chdir(workDir); //change workdir
	printf("Workdir changed to:%s\n", workDir);
	return 0;
}

