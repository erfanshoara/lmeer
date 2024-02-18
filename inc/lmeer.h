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
#include <string.h>
// }


// def
// {
// lmeer version as int
#define __LMEER_NUMR_VERSION__	1

// size of each ESC block
#define __LMEER_SIZE_ECSBLK__	100

// the highest len for the ECSP - for memory safety
#define __LMEER_XLEN_ECS__		100 * __LMEER_SIZE_ECSBLK__

// string format for ERCD
#define __LMEER_ECSTR_FRM__		"%d"

// max len of ercd as string
#define __LMEER_ECSTR_XLEN__	10


// chars used in printing the error trace
// format will be:
// "{INI}({SEP}{ERR})*{SEP}{END}" : all error coded fitted
// "{INI}({SEP}{ERR})*{SEP}{EXT}" : some error code are mising
// {ERR} is a single error code
// {
#define __LMEER_TRC_INI__		'S'
#define __LMEER_TRC_SEP__		'>'
#define __LMEER_TRC_END__		'E'
#define __LMEER_TRC_EXT__		'X'
// }

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
// thread-safe global variable used in LMEER
// this is the main var being used in all methods that stores everything
extern __thread lmeer_ecsp_t LMEER_ECSP_MAIN;

/*
 * this is a list of error messages for the internal errors
 * [ 0]: "No Internal Error."
 * [ 1]: "Failed to allocate memory for ECSP (errno in ercd_last)."
 * [ 2]: "Attempted to add before initialization."
 * [ 3]: "Number of errors exceeded the limit."
 * [ 4]: "Failed to reallocate memory for ECSP (errno in ercd_last)."
 * [ 5]: "Failed to allocating memory for the info string "
 * 			"(errno in ercd_last)."
 *
 * [ 6]: "Something went wrong while printing the info string."
 * [ 7]: "Called the alloc_print trace function before initializing lmeer."
 * [ 8]: "Failed to allocating memory for the trace string "
 * 			"(errno in ercd_last)."
 *
 * [ 9]: "Unexpected failure - "
 * 			"ran out of spacing while printing the trace string."
 *
 * [10]: "Failed to reallocate memory for trace string (errno in ercd_last)."
 * [11]: "Unexpected failure - "
 * 			"ran out of spacing while printing the trace string."
 *
 * [12]: "Called the print trace function before initializing lmeer."
 * [13]: "Internal - Failed to allocate memory for trace string."
 * [14]: "Attempted to pop before initialization."
 * [15]: "Internal - after init but count is zero."
 * [16]: "Internal - Invalid Internal Error."
 */
extern const char*	__LMEER_LST_INTR[];
extern const size_t	__LMEER_LEN_INTR;
// }


// func
// {

/*
 * in:
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
lmeer_print_info(
		FILE* in_stream
		);

void
alloc_lmeer_print_trace(
		char**	ot_strP_trace,
		size_t*	ot_lenP_trace
		);

void
lmeer_print_trace(
		FILE* in_stream
		);

void
alloc_lmeer_print_interr(
		char**	ot_strP_interr,
		size_t*	ot_lenP_interr
		);

void
lmeer_print_interr(
		FILE* in_stream
		);

const char*
lmeer_interr2str();

lmeer_ercd_t
lmeer_pop_ercd();

void
lmeer_term_ecsp();

// }


#endif
