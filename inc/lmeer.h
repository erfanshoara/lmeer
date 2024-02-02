/*
 * Filename:	lmeer.h
 * Author:		Erfan Shoara (erfan@shoara.net)
 * Date:		Nov 10, 2023
 * Description:	.
 */


#ifndef __LMEER__H__
#define __LMEER__H__


// lib
// {
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <stdint.h>
//#include <stdbool.h>
//#include <stddef.h>
//#include <sys/types.h>
// }


// def
// {
// size of each ESC block
#define __LMEER_SIZE_ECSBLK__	100
// the highest len for the ECSP - for memory safety
#define __LMEER_XLEN_ECS__		100 * __LMEER_SIZE_ECSBLK__
// lmeer version as int
#define __LMEER_NUMR_VERSION__	1
// }


// type
// {
// just for a reference (ERCD: ERror CoDe)
typedef int lmeer_ercd_t;

// ECSP: Error Code Stack & Param
typedef struct 
{
	// pointer to stack of ERCD used per thread in lmeer
	lmeer_ercd_t*	stackP;

	// keep the track of number error added
	size_t			count;

	// keep the track of the current length of stackP
	size_t			len;

	// first error code raised - internal
	lmeer_ercd_t	ercd_init;

	// last error code raised
	lmeer_ercd_t	ercd_last;

	// internal error - for error that occur in lmeer itself (error in error
	// handler)
	lmeer_ercd_t	ercd_intr;

	// first error code raised - external note that this error code will not be
	// added to ECSP since it is an external error
	lmeer_ercd_t	ercd_extr;
} lmeer_ecsp_t;
// }


// var
// {
extern __thread lmeer_ecsp_t LMEER_ECSP_MAIN;

extern const char*	__LMEER_LST_INTR[];
extern const size_t	__LMEER_LEN_INTR;
// }


// func
// {
/*
 * func: lmeer_init_ecsp
 *
 * in:
 * 	>
 * 	>
 *
 * out: -
 *
 * ret:	void
 *
 * desc:
 * 	if ECSP is already initialized, then it will simply return, otherwise it
 * 	will initializes ECSP and its corresponding variables.
 * 	this function will only fail in memory allocation for ECSP, which in that
 * 	case will set the external error code but the status of ECSP will remain
 * 	uninitialized, thus any following call to append ERCD will not do anything
 * 	- this is an extreme scenario where the system is out of memory and cannot
 * 	handle any error
 */
void
lmeer_init_ecsp(
		lmeer_ercd_t	in_ercd_init,
		lmeer_ercd_t	in_ercd_extr
		);

void
lmeer_add_ercd(
		lmeer_ercd_t in_ercd_new
		);

void
lmeer_raise_erdc(
		lmeer_ercd_t	in_ercd_new,
		lmeer_ercd_t	in_ercd_extr
		);

void
alloc_lmeer_print_info(
		char**	ot_strP_info,
		size_t*	ot_lenP_info
		);

void
lmeer_print_info(FILE* in_stream);

void
alloc_lmeer_print_trace(
		char**	ot_strP_trace,
		size_t*	ot_lenP_trace
		);

void
lmeer_print_trace(FILE* in_stream);

lmeer_ercd_t
lmeer_pop_ercd();

void
lmeer_term_ecsp();

void
print_ecsp_mesg(
		char**	in_lst_mesg,
		char**	in_len_lstmsg,
		int**	in_map_imsg
		);

void
alloc_ecsp_mesg(
		char**	ot_strP_mesg,
		size_t*	ot_len_strP,
		char**	in_lst_mesg,
		size_t*	in_len_lstmsg,
		int**	in_map_imsg
		);
// }


#endif
