/*
 * Filename:	lmeer.c
 * Author:		Erfan Shoara (erfan@shoara.net)
 * Date:		Nov 10, 2023
 * Description:	.c file for lmeer.h file
 */


#include <lmeer.h>


// var
// {
__thread lmeer_ecsp_t LMEER_ECSP_MAIN =
{
	.stackP		= NULL,
	.count		= 0,
	.len		= 0,
	.ercd_init	= 0,
	.ercd_last	= 0,
	.ercd_intr	= 0,
	.ercd_extr	= 0
};

const char*	__LMEER_LST_INTR[] =
{
	"No Internal Error.",
	"Failed to allocate memory for ECSP (errno in ercd_last).",
	"Attempted to add before initialization.",
	"Number of errors exceeded the limit.",
	"Failed to reallocate memory for ECSP (errno in ercd_last).",
	"Failed to allocating memory for the info string (errno in ercd_last).",
	"Something went wrong while printing the info string.",
	"Called the alloc_print trace function before initializing lmeer.",
	"Failed to allocating memory for the trace string (errno in ercd_last).",
	"Unexpected failure - ran out of spacing while printing the trace string.",
	"Failed to reallocate memory for trace string (errno in ercd_last).",
	"Unexpected failure - ran out of spacing while printing the trace string.",
	"Called the print trace function before initializing lmeer.",
	"Internal - Failed to allocate memory for trace string.",
	"Attempted to pop before initialization.",
	"Internal - after init but count is zero.",
	"Internal - Invalid Internal Error.",
};

const size_t	__LMEER_LEN_INTR =
	sizeof(__LMEER_LST_INTR)		/
	sizeof(__LMEER_LST_INTR[0]);

// }


// func
// {
void
lmeer_init_ecsp(
		lmeer_ercd_t	in_ercd_init,
		lmeer_ercd_t	in_ercd_extr
		)
{
	// checking if already initialized
	// {
	if (LMEER_ECSP_MAIN.stackP != NULL)
	{
		// TODO: can add the ercd by calling the add function
		// or even re-init ECSP by calling term
		return;
	}
	// }

	// starting new trace
	// {
	LMEER_ECSP_MAIN.stackP = (lmeer_ercd_t*)calloc(
			__LMEER_SIZE_ECSBLK__,
			sizeof(lmeer_ercd_t)
			);
	
	if (LMEER_ECSP_MAIN.stackP == NULL)
	{
		// error in error handler
		// in case there is an error in init, the global vars will be:
		// > init will be the init passed in to the init
		// > extr will be the extr passed in to the init
		// > last will be the error that occurred in init
		LMEER_ECSP_MAIN.ercd_intr	= 1;
		LMEER_ECSP_MAIN.ercd_last	= errno;
		return;
	}
	else
	{
		// all good with the stack, setting the param
		LMEER_ECSP_MAIN.stackP[0]	= in_ercd_init;
		LMEER_ECSP_MAIN.count		= 1;
		LMEER_ECSP_MAIN.len			= __LMEER_SIZE_ECSBLK__;

		LMEER_ECSP_MAIN.ercd_init	= in_ercd_init;
		LMEER_ECSP_MAIN.ercd_extr	= in_ercd_extr;
		LMEER_ECSP_MAIN.ercd_last	= in_ercd_init;
	}
	// }
}


void
lmeer_add_ercd(
		lmeer_ercd_t in_ercd_new
		)
{
	// checking if not initialized
	if (!LMEER_ECSP_MAIN.stackP)
	{
		// add function is called before init - if need to take any action
		LMEER_ECSP_MAIN.ercd_intr = 2;
		return;
	}

	// var
	// {
	size_t	len_trace_new		= 0;
	lmeer_ercd_t*	_ercdP		= NULL;
	// }

	// main
	// {
	if (LMEER_ECSP_MAIN.count < LMEER_ECSP_MAIN.len)
	{
		// there is still space in the stack, append the error code and any
		// other corresponding param
		LMEER_ECSP_MAIN.stackP[LMEER_ECSP_MAIN.count]	= in_ercd_new;
		LMEER_ECSP_MAIN.ercd_last						= in_ercd_new;
		LMEER_ECSP_MAIN.count++;
	}
	else
	{
		// there is no more space in the stack and it needs to expand

		// it does the add in case XNUM is not set as a number of BLK
		// size - by mistake
		len_trace_new = LMEER_ECSP_MAIN.len + __LMEER_SIZE_ECSBLK__;

		if (len_trace_new > __LMEER_XLEN_ECS__)
		{
			// error in error
			// what to do when there are more than limit error code -
			// extreme scenario
			LMEER_ECSP_MAIN.ercd_intr = 3;
		}
		else
		{
			// we still can expand the stack - attempt to realloc
			// {
			_ercdP = (lmeer_ercd_t*)reallocarray(
					LMEER_ECSP_MAIN.stackP,
					len_trace_new,
					sizeof(lmeer_ercd_t)
					);

			if (!_ercdP)
			{
				// error in reallocation
				LMEER_ECSP_MAIN.ercd_intr = 4;
				LMEER_ECSP_MAIN.ercd_last = errno;
				return;
			}
			// }

			// after expanding the stack - add the new ercd and param
			// {
			LMEER_ECSP_MAIN.stackP[LMEER_ECSP_MAIN.count]	= in_ercd_new;
			LMEER_ECSP_MAIN.ercd_last						= in_ercd_new;
			LMEER_ECSP_MAIN.len								= len_trace_new;
			LMEER_ECSP_MAIN.count++;
			// }
		}
	}
	// }
}


void
lmeer_raise_erdc(
		lmeer_ercd_t	in_ercd_new,
		lmeer_ercd_t	in_ercd_extr
		)
{
	if (LMEER_ECSP_MAIN.stackP)
	{
		// if already initialized then add and ignore the external
		lmeer_add_ercd(in_ercd_new);
	}
	else
	{
		// if hasn't initialized yet, then init
		lmeer_init_ecsp(in_ercd_new, in_ercd_extr);
	}

	return;
}


void
alloc_lmeer_print_info(
		char**	ot_strP_info,
		size_t*	ot_lenP_info
		)
{
	// note this function can be called before init as well
	// that's why there is no NULL check for lmeer init

	int _ret = 0;

	const char		str_info_format[] =
		"LMEER Lib Info:\n"
		"VERSION:   <%d>\n"
		"\n"
		"LMEER ECSP Info:\n"
		"Stack Pointer:  <%18p>\n"
		"Stack Length:   <%#zx>\n"
		"Error Count:    <%#zx>\n"
		"First Error:    <"__LMEER_ECSTR_FRM__">\n"
		"Last Error:     <"__LMEER_ECSTR_FRM__">\n"
		"External Error: <"__LMEER_ECSTR_FRM__">\n"
		"Internal Error: <"__LMEER_ECSTR_FRM__">\n"
		"\n";

	/*
	 * This is the len of the format string for the info, plus adding the
	 * length of all variables after conversion with maximum possible length.
	 * The length of the format strings for variables (e.g. "%018p") is not
	 * deducted and they will act as a safeguard for the string - no need to
	 * walk on the edge, so we don't. Followings are showing what each addition
	 * is for:
	 * > 0x0A					* 1	: int			: the version number
	 * > 0x10					* 1	: void*			: 64-bit virtual address
	 * > 0x08					* 2	: size_t		: length and count of ECSP
	 * > __LMEER_ECSTR_XLEN__	* 4	: lmeer_ercd_t	: lmeer.h defined ERCD
	 * > 1							: char			: terminating NULL - '\0'
	 */
	const size_t xlen_info_format =
		sizeof(str_info_format)	+
		0x0A					* 1	+
		0x10					* 1	+
		0x08					* 2	+
		__LMEER_ECSTR_XLEN__	* 4	+
		1;

	*ot_strP_info = (char*)calloc(
			xlen_info_format,
			sizeof(char)
			);

	if (!*ot_strP_info)
	{
		// error allocating memory for the info string
		LMEER_ECSP_MAIN.ercd_intr = 5;
		LMEER_ECSP_MAIN.ercd_last = errno;
		return;
	}

	_ret = snprintf(
			*ot_strP_info,
			xlen_info_format,
			str_info_format,

			__LMEER_NUMR_VERSION__,
			LMEER_ECSP_MAIN.stackP,
			LMEER_ECSP_MAIN.len,
			LMEER_ECSP_MAIN.count,
			LMEER_ECSP_MAIN.ercd_init,
			LMEER_ECSP_MAIN.ercd_last,
			LMEER_ECSP_MAIN.ercd_extr,
			LMEER_ECSP_MAIN.ercd_intr
		  );

	if (
			(_ret < 1)						||
			((xlen_info_format - 1) < (size_t)_ret)
			)
	{
		// error - the return size does not make sense
		free(*ot_strP_info);  
		*ot_strP_info = NULL;

		LMEER_ECSP_MAIN.ercd_intr = 6;
		return;
	}

	*ot_lenP_info = xlen_info_format;

	return;
}


void
lmeer_print_info(FILE* in_stream)
{
	char*	str_info = NULL;
	size_t	len_info = 0;

	// alloc scope
	// [
	alloc_lmeer_print_info(
			&str_info,
			&len_info
			);

	fprintf(
			in_stream,
			"%s",
			str_info
		   );

	free(str_info);
	// ]
}


void
alloc_lmeer_print_trace(
		char**	ot_strP_trace,
		size_t*	ot_lenP_trace
		)
{
	// checking if lmeer is not initialized yet
	if (LMEER_ECSP_MAIN.stackP == NULL)
	{
		// error: printing trace before init
		LMEER_ECSP_MAIN.ercd_intr = 7;
		return;
	}

	size_t	_i_err			= 0;
	size_t	_i_str			= 0;
	int		_added_string	= 0;
	char*	str_trace		= NULL;
	char*	_str_trace		= NULL;

	// '>' is temporary - it is set to the SEP char on the next line
	char	_str_format[]	= ">" __LMEER_ECSTR_FRM__;
	_str_format[0] = __LMEER_TRC_SEP__;

	/*
	 * here is an example of an error trace:
	 * 	"S>1>3>4>5>5>3>5>6>E"
	 * 	broken down:
	 * 	"[S][{>1}>3>4>5>5>3>5>6][>E]"
	 * 	  |    |                  |  |
	 * 	  +----\------------------\--\--> the init char - has len of 1
	 * 	       |                  +--\--> the ending sep and char - len of 2
	 * 	       |                     |    >> together : len of (3)
	 * 	       |                     |
	 * 	       |                     +--> the terminating null - len of (1)
	 * 	       |
	 * 	       +------------------------> one of the errors + a sep (len =
	 * 	                                  (len of error_code + len of sep) *
	 * 	                                  num of error_codes)
	 */
	size_t	xlen_trace			=
		(3)	+
		(
			(__LMEER_ECSTR_XLEN__ + 1)	*
			LMEER_ECSP_MAIN.count
		)	+
		1;

	str_trace = (char*)calloc(
			xlen_trace,
			sizeof(char)
			);

	if (!str_trace)
	{
		// error allocating memory for the trace string
		*ot_lenP_trace = 0;

		LMEER_ECSP_MAIN.ercd_intr = 8;
		LMEER_ECSP_MAIN.ercd_last = errno;
		return;
	}

	str_trace[0] = __LMEER_TRC_INI__;
	for (
			_i_err = 0, _i_str = 1;
			_i_err < LMEER_ECSP_MAIN.count;
			_i_err++
			)
	{
		/* 4 comes from:
		 * 	1 sep for the error
		 * 	1 for the END char
		 * 	1 sep for the END
		 * 	1 for the term null
		 */
		if (
				_i_str			<
				(
					xlen_trace - (__LMEER_ECSTR_XLEN__ + 4)
				)
			)
		{
			_added_string = snprintf(
					str_trace	+ _i_str,
					xlen_trace	- _i_str - 1,
					_str_format,
					LMEER_ECSP_MAIN.stackP[_i_err]
					);

			_i_str += _added_string;
		}
		else if (_i_str < xlen_trace-2)
		{
			// if about running out of space
			str_trace[_i_str] = __LMEER_TRC_SEP__;
			_i_str++;
			str_trace[_i_str] = __LMEER_TRC_EXT__;
			_i_str++;

			break;
		}
		else
		{
			// error: running out of space
			LMEER_ECSP_MAIN.ercd_intr = 9;
			LMEER_ECSP_MAIN.ercd_last = errno;
			break;
		}
	}

	// checking for space
	if (_i_str < (xlen_trace - 0x3))
	{
		str_trace[_i_str] = __LMEER_TRC_SEP__;
		_i_str++;
		str_trace[_i_str] = __LMEER_TRC_END__;
		_i_str++;
		str_trace[_i_str] = '\0';
		// no need for another _i_str++; since _i_str is the len of the string
		// without the terminating null - in other word the index of the
		// terminating null

		// since we have some unused space, let's save those
		_str_trace = (char*)reallocarray(
				str_trace,
				_i_str,
				sizeof(lmeer_ercd_t)
				);

		if (!_str_trace)
		{
			// error: relloaction failed
			free(str_trace);
			*ot_lenP_trace = 0;

			LMEER_ECSP_MAIN.ercd_intr = 10;
			LMEER_ECSP_MAIN.ercd_last = errno;
			return;
		}

		*ot_strP_trace = _str_trace;
		*ot_lenP_trace = _i_str++;
	}
	else
	{
		// error: unexpected error - running out of space
		free(str_trace);
		*ot_lenP_trace = 0;

		LMEER_ECSP_MAIN.ercd_intr = 11;
		LMEER_ECSP_MAIN.ercd_last = errno;
		return;
	}

	return;
}


void
lmeer_print_trace(
		FILE* in_stream
		)
{
	// checking if not initialized yet
	if (!LMEER_ECSP_MAIN.stackP)
	{
		// error: called before lmeer init
		LMEER_ECSP_MAIN.ercd_intr = 12;
		return;
	}

	// var
	// {
	char*	str_trace = NULL;
	size_t	len_trace = 0;
	// }


	// alloc scope
	// [
	alloc_lmeer_print_trace(
			&str_trace,
			&len_trace
			);

	if (!str_trace)
	{
		// error: internal error calling the alloc function
		LMEER_ECSP_MAIN.ercd_intr = 13;
		return;
	}
	else
	{
		fprintf(
				in_stream,
				"%s\n",
				str_trace
			   );
	}

	free(str_trace);
	// ]
	return;
}


const char*
lmeer_interr2str()
{
	if ((size_t)LMEER_ECSP_MAIN.ercd_intr < __LMEER_LEN_INTR)
	{
		return __LMEER_LST_INTR[LMEER_ECSP_MAIN.ercd_intr];
	}
	else
	{
		// error - invalid internal error
		// last index is for invalid error
		return __LMEER_LST_INTR[__LMEER_LEN_INTR-1];
	}
}
  

void
alloc_lmeer_print_interr(
		char**	ot_strP_interr,
		size_t*	ot_lenP_interr
		)
{
	const char*		_str	= lmeer_interr2str();
	const size_t	len_str = strlen(_str) + 1;

	*ot_strP_interr = (char*)calloc(
			len_str,
			sizeof(char)
			);

	memcpy(
			*ot_strP_interr,
			_str,
			len_str - 1
		  );

	*ot_strP_interr[len_str-1] = '\0';
	*ot_lenP_interr = len_str;

	return;
}


void
lmeer_print_interr(
		FILE* in_stream
		)
{
	const char* _str = lmeer_interr2str();

	fprintf(
			in_stream,
			"%s\n",
			_str
		   );

	return;
}


lmeer_ercd_t
lmeer_pop_ercd()
{
	if (!LMEER_ECSP_MAIN.stackP)
	{
		// error called pop before init
		LMEER_ECSP_MAIN.ercd_intr = 14;
		return 0;
	}

	if (LMEER_ECSP_MAIN.count == 0)
	{
		// error: unexpected case - initialized but count is 0?
		LMEER_ECSP_MAIN.ercd_intr = 15;
		return 0;
	}


	LMEER_ECSP_MAIN.count--;

	LMEER_ECSP_MAIN.ercd_last =
		LMEER_ECSP_MAIN.stackP[LMEER_ECSP_MAIN.count-1];

	return LMEER_ECSP_MAIN.stackP[LMEER_ECSP_MAIN.count];
}


void
lmeer_term_ecsp()
{
	free(LMEER_ECSP_MAIN.stackP);

	LMEER_ECSP_MAIN.stackP		= NULL;
	LMEER_ECSP_MAIN.count		= 0;
	LMEER_ECSP_MAIN.len			= 0;
	LMEER_ECSP_MAIN.ercd_init	= 0;
	LMEER_ECSP_MAIN.ercd_last	= 0;
	LMEER_ECSP_MAIN.ercd_intr	= 0;
	LMEER_ECSP_MAIN.ercd_extr	= 0;
}

