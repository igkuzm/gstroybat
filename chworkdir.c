/**chworkdir.h
 * File              : chworkdir.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.09.2021
 * Last Modified Date: 10.02.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include "chworkdir.h"
#include <stdbool.h>
#include "cp.h"
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

bool has_extension(char const *name, char const *extension) //check file extension
{
    size_t len = strlen(name);
	size_t ext_len = strlen(extension);
	char buf[128];
	sprintf(buf, ".%s", extension);
    return len > ext_len - 1 && strcmp(name + len - ext_len - 1, buf) == 0;
}

//copy files with extensions (filetypes_ext, filetype_ext_count) from EXEC/../share/UV to WorkDir
int copyResourcesToWorkDir(char *sourceDir, char *destinationDir, char *filetypes_ext[], int filetype_ext_count, bool overwrite){
	//reading resources directory
	printf("copy files from %s to %s\n", sourceDir, destinationDir);
	
	struct dirent *entry;
	DIR *dp;

	dp = opendir(sourceDir);
	if (dp == NULL) 
	{
		fprintf(stderr, "ERROR OPEN DIR %s\n", sourceDir);
		errno = ENOENT;
		return ENOENT;
	}
	while((entry = readdir(dp))){ //reading files
		int i;
		for (i = 0; i < filetype_ext_count; ++i) {
			if (has_extension(entry->d_name, filetypes_ext[i])) { //if file has filetype extension
				printf("Try to copy file: %s\n", entry->d_name);
				
				char source[1024];
				char destination[1024];
				
				sprintf(source, "%s/%s", sourceDir, entry->d_name);
				sprintf(destination, "%s/%s", destinationDir, entry->d_name);

				if (!file_exists(destination) || overwrite){ //check if file exists and need of overwrite
					printf("Copy source: %s to dest: %s\n", source, destination);
					int erro = cp(source, destination, true);
					if (erro){
						fprintf(stderr, "ERROR TO COPY SOURCE: %s TO DEST: %s, err:%d\n", source, destination, erro);	
					};
				} else {
					printf("No need to overwrite, file exists: %s\n", destination);
				} 
			}
		}
	}
	return 0;
}

int changeWorkDir(char *argv[]){
	int erro=0;
	char *executable=dirname((char *)argv[0]);
	char *workDir=calloc(BUFSIZ,sizeof(char));
	if (!workDir) {
		fprintf(stderr, "ERROR. Cannot allocate memory\n");		
		errno = ENOMEM;
		return ENOMEM;
	}	
#ifdef __APPLE__
		sprintf(workDir, "%s%s", executable, "/../Resources"); //workdir for Apple is BUNDLE/../Resources
#elif defined _WIN32 || defined _WIN64
		sprintf(workDir, "%s", executable); //workdir for Windows is executable dir
#else
		//for Unix systems
		//find homedir and create Stroybat
		printf("HOMEDIR: %s\n", getenv("HOME"));
		erro = sprintf(workDir, "%s%s", getenv("HOME"), "/stroybat"); 
		if (erro == -1) {
			fprintf(stderr,"%s\n", "ERROR to get homedir");
		}
		struct stat st = {0};
		if (stat(workDir, &st) == -1) {
			erro = mkdir(workDir, 0700); //create HOME/Stroybat
			if (erro == -1) {
				fprintf(stderr,"%s\n", "ERROR to to create homedir/stroybat dir");
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
		executable=dirname(selfpath);

		char resourcedir[1024];
		sprintf(resourcedir, "%s%s", executable, "/../share/stroybat"); //resources dir

		//copy files from resources to workdir
		char *filetypes_ext[] = {"db", "conf"}; //we need *.sqlite and *.conf files
		erro = copyResourcesToWorkDir(resourcedir, workDir, filetypes_ext, 2, false);//copy resources
		if (erro) {
			fprintf(stderr, "ERROR TO COPY RESOURCES, err:%d\n", erro);
		}

		////copy Templates from resources to workdir
		//char templates_source_dir[1024]; //source dir of templates		
		//erro = sprintf(templates_source_dir, "%s%s", resourcedir, "/Templates"); 
		//if (erro == -1) {
			//fprintf(stderr,"%s\n", "ERROR to get Templates source dir");
		//}		

		//char templates_dest_dir[1024];
		//erro = sprintf(templates_dest_dir, "%s%s", workDir, "/Templates"); 
		//if (erro == -1) {
			//fprintf(stderr,"%s\n", "ERROR to get Templates destination dir");
		//}
		//struct stat templates_st = {0};
		//if (stat(templates_dest_dir, &templates_st) == -1) {
			//erro = mkdir(templates_dest_dir, 0700); //create Templates dir
			//if (erro == -1) {
				//fprintf(stderr,"%s\n", "ERROR to to create homedir/stroybat/Templates dir");
			//}			
		//}		
		
		//char *templates_ext[] = {"rtf"};		
		//erro = copyResourcesToWorkDir(templates_source_dir, templates_dest_dir, templates_ext, 1, false);//copy resources
		//if (erro) {
			//fprintf(stderr, "ERROR TO COPY RESOURCES, err:%d\n", erro);
		//}
		
#endif
	chdir(workDir); //change workdir
	printf("Workdir changed to:%s\n", workDir);
	free(workDir);
	return erro;
}
