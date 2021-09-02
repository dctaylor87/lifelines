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

/* return the first individual in the database (in keynum order) */
static PyObject *llpy_firstindi (PyObject *self, PyObject *args);

/* return the last individual in the database (in keynum order) */
static PyObject *llpy_lastindi (PyObject *self, PyObject *args);

/* return the first family in the database (in keynum order) */
static PyObject *llpy_firstfam (PyObject *self, PyObject *args);

/* return the last family in the database (in keynum order) */
static PyObject *llpy_lastfam (PyObject *self, PyObject *args);

/* these functions return iterators for all individuals, families, and
   sources, respectively, in the database.  */

static PyObject *llpy_individuals (PyObject *self, PyObject *args);
static PyObject *llpy_families (PyObject *self, PyObject *args);
static PyObject *llpy_sources (PyObject *self, PyObject *args);

 /* llpy_firstindi (void) --> INDI

   Returns the first INDI in the database in key order.  */

static PyObject *llpy_firstindi (PyObject *Py_UNUSED(self), PyObject *args ATTRIBUTE_UNUSED)
{
  INT keynum = xref_firsti();
  LLINES_PY_INDI_RECORD *rec;

  if (keynum == 0)
    Py_RETURN_NONE;		/* no individuals in the database */

  rec = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
  if (! rec)
    return NULL;

  rec->lri_type = LLINES_TYPE_INDI;
  rec->lri_record = keynum_to_irecord (keynum);
  return (PyObject *)rec;
}

/* llpy_lastindi (void) --> INDI

   Returns the last INDI in the database in key order.  */

static PyObject *llpy_lastindi (PyObject *Py_UNUSED(self), PyObject *args ATTRIBUTE_UNUSED)
{
  INT keynum = xref_lasti();
  LLINES_PY_INDI_RECORD *rec;

  if (keynum == 0)
    Py_RETURN_NONE;		/* no individuals in the database */

  rec = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
  if (! rec)
    return NULL;

  rec->lri_type = LLINES_TYPE_INDI;
  rec->lri_record = keynum_to_irecord (keynum);
  return (PyObject *)rec;
}

/* llpy_firstfam (void) --> FAM

   Returns the first FAM in the database in key order.  */

static PyObject *llpy_firstfam (PyObject *Py_UNUSED(self), PyObject *args ATTRIBUTE_UNUSED)
{
  INT keynum = xref_firstf();
  LLINES_PY_FAM_RECORD *rec;

  if (keynum == 0)
    Py_RETURN_NONE;		/* no families in the database */

  rec = PyObject_New (LLINES_PY_FAM_RECORD, &llines_family_type);
  if (! rec)
    return NULL;

  rec->lrf_type = LLINES_TYPE_FAM;
  rec->lrf_record = keynum_to_frecord (keynum);
  return (PyObject *)rec;
}

/* llpy_lastfam (void) --> FAM

   Returns the last FAM in the database in key order.  */

static PyObject *llpy_lastfam (PyObject *Py_UNUSED(self), PyObject *args ATTRIBUTE_UNUSED)
{
  INT keynum = xref_lastf();
  LLINES_PY_FAM_RECORD *rec;

  if (keynum == 0)
    Py_RETURN_NONE;		/* no families in the database */

  rec = PyObject_New (LLINES_PY_FAM_RECORD, &llines_family_type);
  if (! rec)
    return NULL;

  rec->lrf_type = LLINES_TYPE_FAM;
  rec->lrf_record = keynum_to_frecord (keynum);
  return (PyObject *)rec;
}

/* llpy_individuals (void) --> Returns an iterator for the set of
   individuals in the database.  */

static PyObject *llpy_individuals (PyObject *Py_UNUSED(self), PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_INDI;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_families (void) --> Returns an iterator for the set of
   families in the database.  */

static PyObject *llpy_families (PyObject *Py_UNUSED(self), PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_FAM;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_sources (void) --> Returns an iterator for the set of sources
   in the database.  */

static PyObject *llpy_sources (PyObject *Py_UNUSED(self), PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_SOUR;
  iter->li_current = 0;

  return (PyObject *)iter;
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
     "individuals(void) -> iterator for the set of all INDI in the database" },
   { "families",	llpy_families, METH_NOARGS,
     "families(void) -> iterator for the set of all FAM in the database" },
   { "sources",		llpy_sources, METH_NOARGS,
     "sources(void) -> iterator for the set of all SOUR in the database" },

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
