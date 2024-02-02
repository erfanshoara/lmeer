/*
 * Filename:	lmeer.c
 * Author:		Erfan Shoara (erfan@shoara.net)
 * Date:		Nov 10, 2023
 * Description:	.
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
	"Failed to allocate memory for ECSP.",
};

const size_t	__LMEER_LEN_INTR =
	sizeof(__LMEER_LST_INTR)	/
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
		// > last will be the error that occured in init
		LMEER_ECSP_MAIN.ercd_intr	= -1;
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
	// var
	// {
	int _i = 0;

	size_t	len_trace_new	= 0;
	lmeer_ercd_t*	_ercdP		= NULL;
	// }

	// main
	// {
	// checking if initialized
	if (!LMEER_ECSP_MAIN.stackP)
	{
		// TODO: add function is called before init - if need to take any
		// action
		LMEER_ECSP_MAIN.ercd_intr = -2;
		return;
	}

	if (LMEER_ECSP_MAIN.count < LMEER_ECSP_MAIN.len)
	{
		// there is still space in the stack, append the error code and
		// anyother corresponding param
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
			// TODO: what to do when there are more than limit error code -
			// extreme scenario
			LMEER_ECSP_MAIN.ercd_intr = -3;
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
				// TODO: error in reallocation
				LMEER_ECSP_MAIN.ercd_intr = -4;
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
	int _ret = 0;
	const size_t xlen_info = 700+1;

	*ot_strP_info = calloc(
			xlen_info,
			sizeof(char)
			);

	if (!*ot_strP_info)
	{
		// TODO: error allocating memory for the info string
		return;
	}

	_ret = snprintf(
			*ot_strP_info,
			xlen_info,
			"LMEER Lib Info:\n"
			"VERSION:   <0d%d>\n"
			"MAX_LEN:   <0d%d>\n"
			"SIZE_BLK:  <0d%d>\n"
			"SIZE_ERCD: <0x%zx>\n"
			"SIZE_ECSP: <0x%zx>\n"
			"\n"
			"LMEER ECSP Info:\n"
			"Stack Pointer:  <0x%lx>\n"
			"Stack Length:   <0x%zx>\n"
			"Error Count:    <0d%zx>\n"
			"First Error:    <%d>\n"
			"Last Error:     <%d>\n"
			"External Error: <%d>\n"
			"Internal Error: <%d>\n"
			"\n",

			__LMEER_NUMR_VERSION__,
			__LMEER_XLEN_ECS__,
			__LMEER_SIZE_ECSBLK__,
			sizeof(lmeer_ercd_t),
			sizeof(lmeer_ecsp_t),

			(uint64_t)LMEER_ECSP_MAIN.stackP,
			LMEER_ECSP_MAIN.len,
			LMEER_ECSP_MAIN.count,
			LMEER_ECSP_MAIN.ercd_init,
			LMEER_ECSP_MAIN.ercd_last,
			LMEER_ECSP_MAIN.ercd_extr,
			LMEER_ECSP_MAIN.ercd_intr
		  );

	if ((_ret < 1) || (xlen_info-1 < _ret))
	{
		// TODO: error
		free(*ot_strP_info);  
		*ot_strP_info = NULL;
		return;
	}

	*ot_lenP_info = xlen_info;

	return;
}

void
lmeer_print_info(FILE* in_stream)
{
	char*	str_info = NULL;
	size_t	len_info = 0;

	if (!LMEER_ECSP_MAIN.stackP)
	{
		return;
	}

	// alloc scope
	// {
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
	// }
}


void
alloc_lmeer_print_trace(
		char**	ot_strP_trace,
		size_t*	ot_lenP_trace
		)
{
	int _i_err = 0;
	int _i_str = 0;
	int _added_string = 0;
	char* str_trace = NULL;
	char* _str_trace = NULL;
	size_t xlen_trace = 0x8 + (0x10 * LMEER_ECSP_MAIN.count) + 1;

	str_trace = (char*)calloc(
			xlen_trace,
			sizeof(char)
			);

	if (!str_trace)
	{
		// TODO: error allocating memory for the trace string
		*ot_lenP_trace = 0;
		return;
	}

	str_trace[0] = 'S';
	for (
			_i_err = 0, _i_str = 1;
			_i_err < LMEER_ECSP_MAIN.count;
			_i_err++
			)
	{
		if (_i_str < xlen_trace-0x13)
		{
			_added_string = snprintf(
					str_trace + _i_str,
					xlen_trace - _i_str - 1,
					">%d",
					LMEER_ECSP_MAIN.stackP[_i_err]
					);

			_i_str += _added_string;
		}
		else if (_i_str < xlen_trace-0x2)
		{
			// if about running out of space
			str_trace[_i_str] = '>';
			_i_str++;
			str_trace[_i_str] = 'X';
			_i_str++;

			break;
		}
		else
		{
			break;
		}
	}

	// checking for space
	if (_i_str < xlen_trace - 0x3)
	{
		str_trace[_i_str] = '>';
		_i_str++;
		str_trace[_i_str] = 'E';
		_i_str++;
		str_trace[_i_str] = '\0';
		// no need for another _i_str++; since _i_str is the len of the string
		// without the terminating null - in otherword the index of the
		// terminating null

		_str_trace = (char*)reallocarray(
				str_trace,
				_i_str,
				sizeof(lmeer_ercd_t)
				);

		if (!_str_trace)
		{
			//TODO: error in error
			free(str_trace);
			*ot_lenP_trace = 0;
			return;
		}

		*ot_strP_trace = _str_trace;
		*ot_lenP_trace = _i_str++;
	}
	else
	{
		// TODO: error
		free(str_trace);
		*ot_lenP_trace = 0;
		return;
	}

	return;
}

void
lmeer_print_trace(FILE* in_stream)
{
	char*	str_trace = NULL;
	size_t	len_trace = 0;

	if (!LMEER_ECSP_MAIN.stackP)
	{
		return;
	}

	// alloc scope
	// {
	alloc_lmeer_print_trace(
			&str_trace,
			&len_trace
			);

	if (!str_trace)
	{
		// TODO: error
		fprintf(
				in_stream,
				"EiE:\n"
			   );
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
	// }
	return;
}

  
lmeer_ercd_t
lmeer_pop_ercd()
{
	if (!LMEER_ECSP_MAIN.stackP)
	{
		// TODO: error called pop before init
		return 0;
	}

	if (LMEER_ECSP_MAIN.count == 0)
	{
		// TODO: unexpected case - initialized but count is 0?
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


void
print_ecsp_mesg(
		char**	in_lst_mesg,
		char**	in_len_lstmsg,
		int**	in_map_imsg
		)
{
	// TODO: prints to stdout the error messaged passed in by in_lst_mesg of
	// the error codes in ECSP
	return;
}

void
alloc_print_ecsp_mesg(
		char**	ot_strP_mesg,
		size_t*	ot_len_strP,
		char**	in_lst_mesg,
		size_t*	in_len_lstmsg,
		int**	in_map_imsg
		)
{
	// TODO: the same as print_ecsp_mesg(), but prints into heap - it will do
	// the allocation and the caller is responsible to free the memory
	return;
}

