/**
 * File              : getbundle.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 07.10.2022
 * Last Modified Date: 07.10.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

/*
 * Get bundle of application
 * for mac os - APP/Contents/Resources,
 * for win - executable directory
 * for linux/unix - /usr/[local]/share/APP
 */

#ifndef k_lib_getbundle_h__
#define k_lib_getbundle_h__
#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h> //dirname
#ifdef _WIN32 
#include <Windows.h>	
#elif defined __APPLE__
#else
#include <unistd.h> //readlink
#endif

	char * getbundle(char *argv[]){
		if (!argv || !argv[0])
			return NULL;
	
#ifdef _WIN32
		char * executable = dirname((char *)argv[0]);
		return executable;
#else
		char * bundle = (char *)malloc(BUFSIZ);
		if (!bundle)
			return NULL;
#ifdef __APPLE__
		char * executable = dirname((char *)argv[0]);
		sprintf(bundle, "%s%s", executable, "/../Resources");
		free(executable);
		return bundle; 
#else 
		char selfpath[128];
		if (readlink ("/proc/self/exe", selfpath, sizeof (selfpath) - 1) < 0){
			free(bundle);
			return NULL;
		}

		char *executable=dirname(selfpath);
		char *progname = basename(argv[0]);
		
		sprintf(bundle, "%s/../share/%s", executable, progname);		
		free(progname);
		free(executable);
		return bundle; 
#endif
#endif
		return NULL;
	}

#ifdef __cplusplus
}
#endif
#endif //k_lib_getbundle_h__	
