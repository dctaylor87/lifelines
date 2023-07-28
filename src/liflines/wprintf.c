/* wprintf.c --

   these are duplicated in tools/wprintf.c and ui/ui_cli.c

   moving them out of liflines/screen.c is just the first step to
   getting rid of the duplication. */

#include <stdarg.h>

#include "llstdlib.h"
#include "feedback.h"
#include "gedcom.h"

/*=================================================
 * llwprintf -- Called as wprintf(fmt, arg, arg, ...)
 *===============================================*/
void
llwprintf (STRING fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	llvwprintf(fmt, args);
	va_end(args);
}

/*=========================================
 * message -- handle generic message
 * delegates to msg_outputv
 * TODO: replace with msg_error/info/status
 *=======================================*/
void
message (STRING fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	msg_outputv(MSG_ERROR, fmt, args);
	va_end(args);
}

/*=========================================
 * msg_error -- handle error message
 * delegates to msg_outputv
 *=======================================*/
void
msg_error (STRING fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	msg_outputv(MSG_ERROR, fmt, args);
	va_end(args);
}

/*=========================================
 * msg_info -- handle regular messages
 * delegates to msg_outputv
 *=======================================*/
void
msg_info (STRING fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	msg_outputv(MSG_INFO, fmt, args);
	va_end(args);
}

/*=========================================
 * msg_status -- handle transitory/status messages
 * delegates to msg_outputv
 *=======================================*/
void
msg_status (STRING fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	msg_outputv(MSG_STATUS, fmt, args);
	va_end(args);
}
