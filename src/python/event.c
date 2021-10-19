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
#include "date.h"		/* GDATEVAL */

#include "python-to-c.h"
#include "types.h"

/* forward references */

static PyObject *llpy_date (PyObject *self, PyObject *args);
static PyObject *llpy_place (PyObject *self, PyObject *args);
static PyObject *llpy_year (PyObject *self, PyObject *args);
static PyObject *llpy_long (PyObject *self, PyObject *args);
static PyObject *llpy_short (PyObject *self, PyObject *args);

static PyObject *llpy_dayformat (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_monthformat (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_yearformat (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_eraformat (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_dateformat (PyObject *self, PyObject *args, PyObject *kw);

static void llpy_event_dealloc (PyObject *self);

/* might make this visible, for now it is private */
static struct py_dateformat
{
  int pyd_dayformat;		/* 0-2 */
  int pyd_monthformat;		/* 0-11 */
  int pyd_yearformat;		/* 0-2 */
  int pyd_eraformat;		/* 0-2,[123][12] */
  int pyd_dateformat;
  int pyd_complexformat;
} py_dateformat = { 0, 3, 0, 0, 0, 1};

static PyObject *llpy_date (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_EVEN_NODE *event = (LLINES_PY_EVEN_NODE *) self;
  NODE node = event->lne_node;
  STRING str = event_to_date (node, FALSE);

  if (! str)			/* no date recorded for event */
    Py_RETURN_NONE;

  return Py_BuildValue ("s", str);
}

static PyObject *llpy_place (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_EVEN_NODE *event = (LLINES_PY_EVEN_NODE *) self;
  NODE node = event->lne_node;
  STRING str = event_to_plac (node, FALSE);

  if (! str)			/* no place recorded for event */
    Py_RETURN_NONE;

  return Py_BuildValue ("s", str);
}

static PyObject *llpy_year (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_EVEN_NODE *event = (LLINES_PY_EVEN_NODE *) self;
  STRING str = event_to_date (event->lne_node, FALSE);
  GDATEVAL gdv;
  char buffer[20];

  if (! str)
    Py_RETURN_NONE;

  gdv = extract_date (str);
  str = date_get_year_string (gdv); /* prefer year's string if it has one */
  if (! str || ! *str)
    {
      /* year string is NULL or empty */
      INT year = date_get_year (gdv);
      if (year != BAD_YEAR)
	{
	  snprintf (buffer, sizeof (buffer), FMT_INT, year);
	  str = buffer;
	}
      else
	str = NULL;
    }
  free_gdateval (gdv);
  if (! str)
    Py_RETURN_NONE;

  return Py_BuildValue ("s", str);
}

static PyObject *llpy_long (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_EVEN_NODE *event = (LLINES_PY_EVEN_NODE *) self;
  NODE node = event->lne_node;
  struct tag_rfmt rpt_format = { NULL, NULL, "%1 %2" };
  STRING str = event_to_string (node, &rpt_format);

  if (! str)
    str = "";

  return Py_BuildValue ("s", str);
}

static PyObject *llpy_short (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_EVEN_NODE *event = (LLINES_PY_EVEN_NODE *) self;
  NODE node = event->lne_node;
  struct tag_rfmt rpt_format = { shorten_date, shorten_plac, "%1 %2" };
  STRING str = event_to_string (node, &rpt_format);

  if (! str)
    str = "";

  return Py_BuildValue ("s", str);
}

static PyObject *llpy_dayformat (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "format", NULL };
  int format = -1;
  int old_format = py_dateformat.pyd_dayformat;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|i", keywords, &format))
    return NULL;

  if ((format < -1) || (format > 2))
    {
      PyErr_SetString (PyExc_ValueError, "dayformat: format has a bad value, must be in the range [-1,2]");
      return NULL;
    }
  if (format != -1)
    py_dateformat.pyd_dayformat = format;

  return Py_BuildValue ("i", old_format);
}

static PyObject *llpy_monthformat (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "format", NULL };
  int format = -1;
  int old_format = py_dateformat.pyd_monthformat;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|i", keywords, &format))
    return NULL;

  if ((format < -1) || (format > 11))
    {
      PyErr_SetString (PyExc_ValueError, "monthformat: format has a bad value, must be in the range [-1,11]");
      return NULL;
    }
  if (format != -1)
    py_dateformat.pyd_monthformat = format;

  return Py_BuildValue ("i", old_format);
}

static PyObject *llpy_yearformat (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "format", NULL };
  int format = -1;
  int old_format = py_dateformat.pyd_yearformat;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|i", keywords, &format))
    return NULL;

  if ((format < -1) || (format > 2))
    {
      PyErr_SetString (PyExc_ValueError, "yearformat: format has a bad value, must be in the range [-1,2]");
      return NULL;
    }
  if (format != -1)
    py_dateformat.pyd_yearformat = format;

  return Py_BuildValue ("i", old_format);
}

static PyObject *llpy_eraformat (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "format", NULL };
  int format = -1;
  int old_format = py_dateformat.pyd_eraformat;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|i", keywords, &format))
    return NULL;

  switch (format)
    {
    case 0:
    case 1:
    case 2:
    case 11:
    case 12:
    case 21:
    case 22:
    case 31:
    case 32:
      py_dateformat.pyd_eraformat = format;
      /* FALL THROUGH */
    case -1:
      return Py_BuildValue ("i", old_format);

    default:
      PyErr_SetString (PyExc_ValueError, "eraformat: format has a bad value");
      return NULL;
    }
}

/* llpy_stddate_str(STRING date) ==> STRING -- takes the given date,
   breaks it apart, formats it according to the previously specified
   formats, and returns the resulting string.  */

static PyObject *llpy_stddate_str  (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "date", NULL };
  char *input_date;
  char *output_date;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "s", keywords, &input_date))
    return NULL;

  output_date = do_format_date (input_date,
				py_dateformat.pyd_dayformat,
				py_dateformat.pyd_monthformat,
				py_dateformat.pyd_yearformat,
				py_dateformat.pyd_dateformat,
				py_dateformat.pyd_eraformat, FALSE);

  return Py_BuildValue ("s", output_date);
}

/* llpy_stddate_node(NODE event) ==> STRING -- takes the given event,
   finds the date, breaks it apart, formats it according to the
   previously specified formats, and returns the resulting string.  */

static PyObject *llpy_stddate_node  (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  LLINES_PY_NODE *py_node = (LLINES_PY_NODE *) self;
  char *input_date;
  char *output_date;

  input_date = event_to_date (py_node->lnn_node, FALSE);

  output_date = do_format_date (input_date,
				py_dateformat.pyd_dayformat,
				py_dateformat.pyd_monthformat,
				py_dateformat.pyd_yearformat,
				py_dateformat.pyd_dateformat,
				py_dateformat.pyd_eraformat, FALSE);

  return Py_BuildValue ("s", output_date);
}

static PyObject *llpy_dateformat (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "format", NULL };
  int format = -1;
  int old_format = py_dateformat.pyd_dateformat;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|i", keywords, &format))
    return NULL;

  if ((format < -1) || (format > 14))
    {
      PyErr_SetString (PyExc_ValueError, "dateformat: format has a bad value, must be in the range [-1,2]");
      return NULL;
    }
  if (format != -1)
    py_dateformat.pyd_dateformat = format;

  return Py_BuildValue ("i", old_format);
}

static void llpy_event_dealloc (PyObject *self)
{
  LLINES_PY_EVEN_NODE *even = (LLINES_PY_EVEN_NODE *) self;
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_family_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  /* for now we do not do a release_record, since it is a NODE, not a
     RECORD.  If that changes, this will need to be updated.  */
  even->lne_node = 0;
  even->lne_type = 0;
  Py_TYPE(self)->tp_free (self);
#if 0
  Py_DECREF (Py_TYPE(self));
#endif
}

static struct PyMethodDef Lifelines_Event_Methods[] =
  {
   { "date",		llpy_date, METH_NOARGS,
     "date(EVENT) -> STRING: value of first DATE line for EVENT." },
   { "place",		llpy_place, METH_NOARGS,
     "place(EVENT) -> STRING: value of first PLAC line for EVENT." },
   { "year",		llpy_year, METH_NOARGS,
     "year(EVENT) --> STRING: year or first string of 3-4 digits in DATE line of EVENT." },
   { "long",		llpy_long, METH_NOARGS,
     "long(EVENT) --> STRING: values of first DATE and PLAC lines of EVENT" },
   { "short",		llpy_short, METH_NOARGS,
     "short(EVENT) --> STRING: abbreviated values of DATE and PLAC lines of EVENT." },
   { "stddate",		llpy_stddate_node, METH_NOARGS,
     "stddate(EVENT) --> STRING: formatted date string." },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

static struct PyMethodDef Lifelines_Date_Functions[] =
  {
   { "dayformat",	(PyCFunction)llpy_dayformat, METH_VARARGS,
     "dayformat(INT) --> INT: set day format for stddate calls\n\
\t\t-1 (do not change format, just return existing format)\n\
\t\t0 (leave space before single digit days)\n\
\t\t1 (use leading 0 before single digit days)\n\
\t\t2 (no space or leading 0 before single digit days)." },
   { "monthformat",	(PyCFunction)llpy_monthformat, METH_VARARGS,
     "monthformat(INT) --> INT: set month format for stddate calls.\n\
\t\t-1 (do not change format, just return existing format)\n\
\t\t0\tnumber with space before single digit months\n\
\t\t1\tnumber with leading zero before single digit months\n\
\t\t2\tnumber with no space for zero before single digit months\n\
\t\t3\tupper case abbreviation (eg.g. JAN, FEB) (localized)\n\
\t\t4\tcapitalized abbreviation (e.g., Jan, Feb) (localized)\n\
\t\t5\tupper case full word (e.g., JANUARY, FEBRUARY) (localized)\n\
\t\t6\tcapitalized full word (e.g., January, February) (localized)\n\
\t\t7\tlower case abbreviations (e.g., jan, feb) (localized)\n\
\t\t8\tlower case full word (e.g., january, february) (localized)\n\
\t\t9\tupper case abbreviation in English per GEDCOM (e.g., JAN, FEB)\n\
\t\t10\tlower case roman letter (e.g., i, ii)\n\
\t\t11\tupper case roman letter (e.g., I, II)\n" },
   { "yearformat",	(PyCFunction)llpy_yearformat, METH_VARARGS,
     "yearformat(INT) --> INT: set year format for stddate calls\n\
\t\t-1 (do not change format, just return existing format)\n\
\t\t0 (use leading spaces before years with less than four digits)\n\
\t\t1 (use leading 0 before years with less than four digits\n\
\t\t2 (no space or leading 0 before years)." },
   { "eraformat",	(PyCFunction)llpy_eraformat, METH_VARARGS,
     "eraformat(INT) --> INT: set era format for stddate calls\n\
\t\t-1 (do not change format, just return existing format)\n\
\t\t0 (no AD/BC markers)\n\
\t\t1 (trailing B.C. if appropriate)\n\
\t\t2 (trailing A.D. or B.C.)\n\
\t\t11 (trailing BC if appropriate)\n\
\t\t12 (trailing AD or BC)\n\
\t\t21 (trailing B.C.E. if appropriate)\n\
\t\t22 (trailing C.E. or B.C.E.)\n\
\t\t31 (trailing BC if appropriate)\n\
\t\t32 (trailing CE or BCE)." },
   { "dateformat",	(PyCFunction)llpy_dateformat, METH_VARARGS,
     "dateformat(INT) --> INT: set date format for stddate calls.\n\
\t\t-1 (do not change format, just return existing format\n\
\t\t0  (d0 mo yr)\n\
\t\t1  (mo da, yr)\n\
\t\t2  (mo/da/yr)\n\
\t\t3  (da/mo/yr)\n\
\t\t4  (mo-da-yr)\n\
\t\t5  (da-mo-yr)\n\
\t\t6  (modayr)\n\
\t\t7  (damoyr)\n\
\t\t8  (yr mo da)\n\
\t\t9  (yr/mo/da)\n\
\t\t10 (yr-mo-da)\n\
\t\t11 (yrmoda)\n\
\t\t12 (yr, year only omitting all else)\n\
\t\t13 (da/mo yr)\n\
\t\t14 (As in GEDCOM" },
   { "stddate",		llpy_stddate_str, METH_VARARGS | METH_KEYWORDS,
     "stddate(EVENT) --> STRING: formatted date string." },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_event_type =
  {
   PyVarObject_HEAD_INIT(NULL,0)
   .tp_name = "llines.Event",
   .tp_doc = "Lifelines GEDCOM Event Node",
   .tp_basicsize = sizeof (LLINES_PY_EVEN_NODE),
   .tp_itemsize = 0,
   .tp_flags= Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_event_dealloc,
   .tp_methods = Lifelines_Event_Methods,
  };

void llpy_event_init (void)
{
  int status;

  status = PyModule_AddFunctions (Lifelines_Module, Lifelines_Date_Functions);
  if (status != 0)
    fprintf (stderr, "llpy_event_init: attempt to add functions returned %d\n", status);
}
