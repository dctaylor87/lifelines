#include <ansidecl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "llstdlib.h"		/* __fatal */
#include "standard.h"		/* ASSERT */
#include "feedback.h"
#include "gedcom.h"		/* RECORD, used by screen.h */
#include "indiseq.h"		/* INDISEQ, used by screen.h */
#include "screen.h"
#include "messages.h"

#include "uiio.h"
#include "uiioi.h"

/* forward references */

static int
curses_input (void *data, char **buffer, int *length, char **err_msg);

static int
curses_output (void *data, const char *buffer, char **err_msg);

static int
curses_error (void *data, const char *buffer, char **err_msg);
#if 0				/* for now... */
static void
curses_outputv (void *data, char **err_msg,
		MSG_LEVEL level, STRING fmt, va_list args);
static void append_to_msg_list (STRING msg);
static void display_status (STRING text);
#endif

/* local variables */

static struct uiio _uiio_curses =
  {
    0,
    0,
    0,
    curses_input,
    curses_output,
    curses_error,
    curses_outputv
  };

UIIO *uiio_curses = &_uiio_curses;

static int
curses_input (void *data, char **buffer, int *length, char **err_msg)
{
  ASSERT (0);			/* not yet implemented */
}

static int
curses_output (void *data, const char *buffer, char **err_msg)
{
  ASSERT (0);			/* not yet implemented */
}

static int
curses_error (void *data, const char *buffer, char **err_msg)
{
  ASSERT (0);			/* not yet implemented */
}

#if 0	   /* for now... */
/* curses_outputv -- output message varargs style arguments
   Actually all other msg functions delegate to here.
   @level:     -1=error,0=info,1=status
   @fmt:   [IN]  printf style format string
   @args:  [IN]  vprintf style varargs
   Puts into message list and/or into status area */
 
static void
curses_outputv (ARG_UNUSED(void *data), ARG_UNUSED(char **err_msg),
		MSG_LEVEL level, STRING fmt, va_list args)
{
  char buffer[250];
  STRING ptr;
  unsigned int width = MAINWIN_WIDTH-5;
  /* prefix errors & infos with * and space respectively */
  switch(level) {
  case MSG_ERROR:
    buffer[0] = '*';
    ptr = &buffer[1];
    break;
  case MSG_INFO:
    buffer[0] = ' ';
    ptr = &buffer[1];
    break;
  default:
    ptr = buffer;
    break;
  }
  /* now make string to show/put on msg list */
  llstrncpyvf(ptr, sizeof(buffer)-1, uu8, fmt, args);
  /* first handle transitory/status messages */
  if (level==MSG_STATUS) {
    if (lock_std_msg)
      return; /* can't display it, status bar is locked */
    if (status_showing[0] && !status_transitory) {
      /* we are overwriting something important
	 so it is already on the msg list, we just need to make
	 sure the msg list gets displayed */
      if (!viewing_msgs)
	msg_flag = TRUE;
    }
    display_status(buffer);
    return;
  }
  /* everything important goes onto msg list */
  append_to_msg_list(buffer);
  /* update flag about whether we need to show msg list to user */
  /* being careful in case we are currently *in* the msg list
     show routine */
  if (!viewing_msgs && (length_list(msg_list)>1 || lock_std_msg)) {
    msg_flag = TRUE;
  }
  /* now put it to status area if appropriate */
  if (!lock_std_msg) {
    if (strlen(buffer)>width) {
      buffer[width-4]=0;
      strcat(buffer, "...");
      /*
	TODO: This doesn't make sense until the msg list handles long strings
	if (!viewing_msgs)
	msg_flag = TRUE;
      */
    }
    display_status(buffer);
  }
}

/* append_to_msg_list -- put msg on the msg list

   This is a list that we show the user
   when the current command completes,
   unless it only had one item, and it got
   put on the status bar, and it wasn't too wide. */
 
static void
append_to_msg_list (STRING msg)
{
  if (!msg_list)
    msg_list = create_list2(LISTDOFREE);
  enqueue_list(msg_list, strsave(msg));
}
/* display_status -- put string in status line
   We don't touch the status_transitory flag
   That is caller's responsibility. */
 
static void
display_status (STRING text)
{
  UIWINDOW uiwin = main_win;
  WINDOW *win = uiw_win(uiwin);
  INT row;
  /* first store it */
  llstrncpy(status_showing, text, sizeof(status_showing), uu8);
  if ((INT)strlen(text)>ll_cols-6) {
    status_showing[ll_cols-8] = 0;
    strcat(status_showing, "...");
  }
  /* then display it */
  row = ll_lines-2;
  clear_hseg(win, row, 2, ll_cols-2);
  wmove(win, row, 2);
  mvccwaddstr(win, row, 2, status_showing);
  place_cursor_main();
  wrefresh(win);
}
#endif

void
uiio_curses_init (void)
{
  /* for now, there is nothing to do -- curses initialization is
     handled elsewhere */
}
