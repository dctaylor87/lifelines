/* 
   Copyright (c) 1991-1999 Thomas T. Wetmore IV

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
/*==============================================================
 * screen.h -- Header file for curses-based screen I/O
 * Copyright (c) 1991-94 by T.T. Wetmore IV; all rights reserved
 * pre-SourceForge version information:
 *   2.3.4 - 24 Jun 93    2.3.5 - 20 Aug 93
 *   3.0.0 - 26 Jul 94    3.0.2 - 16 Dec 94
 *============================================================*/

#ifndef _SCREEN_H
#define _SCREEN_H

#include "gedcom.h"
#include "indiseq.h"

#include <curses.h>

#include <stdarg.h>

#ifndef TRUE
#       define TRUE ((BOOLEAN)1)
#endif
#ifndef FALSE
#       define FALSE ((BOOLEAN)0)
#endif

#define MAIN_SCREEN        1
#define ONE_PER_SCREEN     2
#define ONE_FAM_SCREEN     3
#define TWO_PER_SCREEN     4
#define TWO_FAM_SCREEN     5
#define LIST_SCREEN        7
#define AUX_SCREEN         8
/* must be at bottom of list */
#define MAX_SCREEN        10

#ifndef ACS_TTEE
#	define ACS_TTEE '+'
#	define ACS_RTEE '+'
#	define ACS_LTEE '+'
#	define ACS_BTEE '+'
#	define ACS_VLINE '|'
#	define ACS_HLINE '-'
#endif

#ifdef BSD
#	undef ACS_TTEE
#	undef ACS_RTEE
#	undef ACS_LTEE
#	undef ACS_BTEE
#	undef ACS_VLINE
#	undef ACS_HLINE

#	define ACS_TTEE ' '
#	define ACS_RTEE ' '
#	define ACS_LTEE ' '
#	define ACS_BTEE ' '
#	define ACS_VLINE ' '
#	define ACS_HLINE '-'
#endif

/*=========================================
 * UIWINDOWs -- Main screen, menus and popups
 *=======================================*/
/* wrapper for WINDOW */
typedef struct uiwindow_s {
	WINDOW * win;
	struct uiwindow_s * parent;
	BOOLEAN permsub; /* TRUE if a fixed subwindow */
} UIWINDOW;
#define uiw_win(x) (x->win)
#define uiw_parent(x) (x->parent)
#define uiw_permsub(x) (x->permsub)

extern INT ll_lines; /* number of lines used by LifeLines (usually LINES) */
extern INT ll_cols;  /* number of columns used by LifeLines (usually COLSREQ) */
extern INT cur_screen;
extern UIWINDOW *stdout_win;
extern UIWINDOW *main_win;
extern UIWINDOW *ask_win;
extern UIWINDOW *ask_msg_win;
extern UIWINDOW *choose_from_list_win;
extern UIWINDOW *start_menu_win;
extern UIWINDOW *add_menu_win;
extern UIWINDOW *del_menu_win;
extern UIWINDOW *scan_menu_win;
extern UIWINDOW *utils_menu_win;

/* Function Prototype */
INT aux_browse(NODE, INT mode, BOOLEAN reuse);
INT ask_for_char(STRING, STRING, STRING);
INT ask_for_char_msg(STRING, STRING, STRING, STRING);
STRING ask_for_input_filename (STRING ttl, STRING path, STRING prmpt);
STRING ask_for_db_filename(STRING, STRING, STRING);
STRING ask_for_output_filename (STRING ttl, STRING path, STRING prmpt);
INT choose_one_from_indiseq(STRING, INDISEQ);
void display_screen(INT);
void dbprintf(STRING, ...);
INT fam_browse(NODE, INT mode, BOOLEAN reuse);
STRING get_answer (UIWINDOW*, STRING);
INT indi_browse(NODE, INT mode, BOOLEAN reuse);
int init_screen(void);
INT list_browse(INDISEQ seq, INT top, INT *cur, INT mark, NODE * pindi);
void llvwprintf(STRING fmt, va_list args);
void main_menu(void);
STRING message_string (void);
void paint_main_screen(void);
void paint_screen(INT screen);
void paint_two_fam_screen(void);
void paint_list_screen(void);
void show_horz_line(UIWINDOW*, INT, INT, INT);
void show_vert_line(UIWINDOW*, INT, INT, INT);
void shw_array_of_strings(UIWINDOW*, STRING*, INT, INT, INT);
void term_screen(void);
INT twofam_browse(NODE, NODE, INT mode);
INT twoindi_browse(NODE, NODE, INT mode);
void wfield(INT, INT, STRING);
void wpos (INT, INT);

#ifndef _FEEDBACK_H
#include "feedback.h"
#endif /* _FEEDBACK_H */

#endif /* _SCREEN_H */
