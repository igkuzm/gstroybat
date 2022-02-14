/**
 * File              : chworkdir.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.09.2021
 * Last Modified Date: 11.10.2021
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif 

//change directory to BUNDLE/Resources (for MacOS), HOME/UV (for Linux), EXEC (for Windows)
extern int changeWorkDir(char *argv[]);



#ifdef __cplusplus
}
#endif

