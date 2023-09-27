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
/*=============================================================
 * leaks.c -- Handle the report leaks file
 * This module manages the tracking and reporting of record and node leaks.
 *===========================================================*/

#include "sys_inc.h"
#include "standard.h"
#include "gedcomi.h"
#include "lloptions.h"
#include "llstdlib.h"
#include "leaksi.h"

FILE* fpleaks = NULL;

/*=================================================
 * open_leak_log - Open leak log file
 *===============================================*/
void
open_leak_log (void)
{
	STRING report_leak_path = getlloptstr("ReportLeakLog", NULL);
	fpleaks = NULL;
                
	if (report_leak_path)
	{
		fpleaks = fopen(report_leak_path, LLAPPENDTEXT);
	}
}

/*=================================================
 * close_leak_log - Close leak log file
 *===============================================*/
void
close_leak_log (void)
{
	if (fpleaks) {
		fclose(fpleaks);
		fpleaks = NULL;
	}
}

/*=================================================
 * trace_record - Trace record allocation or free
 *===============================================*/
void
track_record(RECORD rec, int op, char *msg, char *file, int line)
{
	char *t = (op == TRACK_OP_ALLOC ? "ALLOC" : (op == TRACK_OP_FREE ? "FREE" : "UNKNOWN" ) );

	if (fpleaks) {
		fprintf(fpleaks, "RECORD(%s,%d): %s: %p %s\n", file, line, t, (void*)rec, msg);
	}

	if (TRACK_BACKTRACE) {
		dump_backtrace(fpleaks);
	}
}

void
track_record_refcnt(RECORD rec, int op, INT refcnt, char *file, int line)
{
	char *t = (op == TRACK_OP_REFCNT_INC ? "INC" : (op == TRACK_OP_REFCNT_DEC ? "DEC" : "UNKNOWN" ) );
	INT adj = (op == TRACK_OP_REFCNT_INC ? -1    : (op == TRACK_OP_REFCNT_DEC ? 1     : 0 ) );

	if (fpleaks) {
		fprintf(fpleaks,"RECORD(%s,%d): %s: %p " FMT_INT "->" FMT_INT "\n", file, line, t, (void*)rec, refcnt+adj, refcnt);
	}

	if (TRACK_BACKTRACE) {
		dump_backtrace(fpleaks);
	}
}

/*=================================================
 * trace_node - Trace node allocation or free
 *===============================================*/
void
track_node(NODE rec, int op, char *msg, char *file, int line)
{
  char *t;

  /* only alloc and free should be encountered, others for completeness */
  switch (op)
    {
    case TRACK_OP_ALLOC:
      t = "ALLOC";
      break;
    case TRACK_OP_FREE:
      t = "FREE";
      break;
    case TRACK_OP_REFCNT_INC:
      t = "INC";
      break;
    case TRACK_OP_REFCNT_DEC:
      t = "DEC";
      break;
    default:
      t = "UNKNOWN";
      break;
    }
  if (fpleaks) {
    fprintf(fpleaks, "NODE(%s,%d): %s: %p %s\n",
	    file, line, t, (void*)rec, msg);
  }

  if (TRACK_BACKTRACE) {
    dump_backtrace(fpleaks);
  }
}

void
track_node_refcnt (NODE node, int op, INT refcnt, const char *tag,
		   const char *function, const char *file, int line)
{
  char *t = (op == TRACK_OP_REFCNT_INC ? "INC" : (op == TRACK_OP_REFCNT_DEC ? "DEC" : "UNKNOWN" ) );
  INT adj = (op == TRACK_OP_REFCNT_INC ? -1    : (op == TRACK_OP_REFCNT_DEC ? 1     : 0 ) );

  /* only refcnt inc and dec should be encountered, others for completeness */
  switch (op)
    {
    case TRACK_OP_ALLOC:
      t = "ALLOC";
      adj = 0;
      break;
    case TRACK_OP_FREE:
      t = "FREE";
      adj = 0;
      break;
    case TRACK_OP_REFCNT_INC:
      t = "INC";
      adj = -1;			/* we are called AFTER the adjustment */
      break;
    case TRACK_OP_REFCNT_DEC:
      t = "DEC";
      adj = 1;			/* we are called AFTER the adjustment */
      break;
    default:
      t = "UNKNOWN";
      adj = 0;
      break;
    }
  if (fpleaks) {
    fprintf(fpleaks,"NODE(%s,%d): %s %s: %p[%s] " FMT_INT "->" FMT_INT "\n",
	    file, line, function, t, (void*)node, tag, refcnt+adj, refcnt);
  }

  if (TRACK_BACKTRACE) {
    dump_backtrace(fpleaks);
  }
}
