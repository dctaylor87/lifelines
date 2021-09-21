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

static void llpy_event_dealloc (PyObject *self);

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
