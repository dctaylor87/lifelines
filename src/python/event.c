/* event.c -- event functions

   These are the bulk of the functions that are documented in the
   'Event' section of the manual.  */


#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "lloptions.h"

#include "gedcom.h"		/* RECORD */

#include "python-to-c.h"
#include "types.h"

static struct PyMethodDef Lifelines_Event_Methods[] =
  {
   { "date",		llpy_date, METH_VARARGS,
     "date(EVENT) -> STRING: value of first DATE line for EVENT." },
   { "place",		llpy_place, METH_VARARGS,
     "place(EVENT) -> STRING: value of first PLAC line for EVENT." },
   { "year",		llpy_year, METH_VARARGS,
     "year(EVENT) --> STRING: year or first string of 3-4 digits in DATE line of EVENT." },
   { "long",		llpy_long, METH_VARARGS,
     "long(EVENT) --> STRING: values of first DATE and PLAC lines of EVENT" },
   { "short",		llpy_short, METH_VARARGS,
     "short(EVENT) --> STRING: abbreviated values of DATE and PLAC lines of EVENT." },

   { NULL, 0, 0, NULL }		/* sentinel */
  };
