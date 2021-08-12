/* database.c -- database functions

   These are functions that are specific to the database.  They
   include functions that return a person or a family or other type of
   record that are not methods of the type -- e.g., firstindi,
   lastindi -- and functions that are specific to the database --
   e.g., the pathname */

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "lloptions.h"

#include "gedcom.h"		/* RECORD */

#include "python-to-c.h"
#include "types.h"
#include "database.h"

/* forward references */

static PyObject *llpy_firstindi (PyObject *self, PyObject *args);
static PyObject *llpy_lastindi (PyObject *self, PyObject *args);

static PyObject *llpy_firstfam (PyObject *self, PyObject *args);
static PyObject *llpy_lastfam (PyObject *self, PyObject *args);

static PyObject *llpy_individuals (PyObject *self, PyObject *args);
static PyObject *llpy_families (PyObject *self, PyObject *args);

 /* llpy_firstindi (void) --> INDI

   Returns the first INDI in the database in key order.  */

static PyObject *llpy_firstindi (PyObject *self, PyObject *args)
{
  abort ();
}

/* llpy_lastindi (void) --> INDI

   Returns the last INDI in the database in key order.  */

static PyObject *llpy_lastindi (PyObject *self, PyObject *args)
{
  abort ();
}

/* llpy_firstfam (void) --> FAM

   Returns the first FAM in the database in key order.  */

static PyObject *llpy_firstfam (PyObject *self, PyObject *args)
{
  abort ();
}

/* llpy_lastfam (void) --> FAM

   Returns the last FAM in the database in key order.  */

static PyObject *llpy_lastfam (PyObject *self, PyObject *args)
{
  abort ();
}

/* llpy_individuals (void) --> set of INDI

   Returns the set of individuals in the database.  */

static PyObject *llpy_individuals (PyObject *self, PyObject *args)
{
  abort ();
}

/* llpy_families (void) --> set of FAM

   Returns the set of families in the database.  */

static PyObject *llpy_families (PyObject *self, PyObject *args)
{
  abort ();
}

static struct PyMethodDef Lifelines_Database_Methods[] =
  {
   { "firstindi",	llpy_firstindi, METH_NOARGS,
     "firstindi(void) -> INDI: first individual in database (in key order)" },
   { "lastindi",	llpy_lastindi, METH_NOARGS,
     "lastindi(void) -> INDI: last individual in database  (in key order)" },
   { "firstfam",	llpy_firstfam, METH_NOARGS,
     "firstfam(void) -> FAM: first family in database (in key order)" },
   { "lastfam",		llpy_lastfam, METH_NOARGS,
     "lastfam(void) -> FAM: last family in database  (in key order)" },
   { "individuals",	llpy_individuals, METH_NOARGS,
     "individuals(void) -> set of all INDI in the database" },
   { "families",	llpy_families, METH_NOARGS,
     "famalies(void) -> set of all FAM in the database" },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_database_type =
  {
   PyVarObject_HEAD_INIT(NULL,0)
   .tp_name = "llines.Database",
   .tp_doc = "Lifelines GEDCOM Database",
   .tp_basicsize = sizeof (LLINES_PY_DATABASE),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_methods = Lifelines_Database_Methods,
  };
