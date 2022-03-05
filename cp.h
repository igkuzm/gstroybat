/**
 * File              : cp.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 04.09.2021
 * Last Modified Date: 05.03.2022
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef k_lib_cp_h__
#define k_lib_cp_h__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>

//copy file FROM path to destination TO path
int k_lib_cp(const char *from, const char *to);

#define cp(from, to)	\
({	\
	int ___c = k_lib_cp(from, to);\
	___c;	\
})

#ifdef __cplusplus
}
#endif
#endif

