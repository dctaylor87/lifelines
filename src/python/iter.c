/* iter.c -- iter types */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "uiprompts.h"		/* DOASK1 */
#include "gedcom.h"		/* RECORD */

#include "python-to-c.h"
#include "types.h"

/* forward references */

/* these functions return iterators for all individuals, families, and
   sources, respectively, in the database.  */

static PyObject *llpy_events (PyObject *self, PyObject *args);
static PyObject *llpy_individuals (PyObject *self, PyObject *args);
static PyObject *llpy_families (PyObject *self, PyObject *args);
static PyObject *llpy_sources (PyObject *self, PyObject *args);

static void llpy_iter_dealloc (PyObject *self);
static PyObject *llpy_iter_iter (PyObject *self);
static PyObject *llpy_iter_iternext (PyObject *self);

/* start of code */

/* llpy_events (void) --> Returns an iterator for the set of
   events in the database.  */

static PyObject *llpy_events (PyObject *self ATTRIBUTE_UNUSED,
			      PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_events entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_EVEN;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_individuals (void) --> Returns an iterator for the set of
   individuals in the database.  */

static PyObject *llpy_individuals (PyObject *self ATTRIBUTE_UNUSED,
				   PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_individuals entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_INDI;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_families (void) --> Returns an iterator for the set of
   families in the database.  */

static PyObject *llpy_families (PyObject *self ATTRIBUTE_UNUSED,
				PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_families entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_FAM;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_sources (void) --> Returns an iterator for the set of sources
   in the database.  */

static PyObject *llpy_sources (PyObject *self ATTRIBUTE_UNUSED,
			       PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_sources entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_SOUR;
  iter->li_current = 0;

  return (PyObject *)iter;
}

static void llpy_iter_dealloc (PyObject *self)
{
  PyTypeObject *tp = Py_TYPE (self);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_iter_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
#if 0
  PyObject_Del (self);
  Py_DECREF (tp);
#endif
}

static PyObject *llpy_iter_iter (PyObject *self)
{
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_iter_iter entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  /* XXX do we need to do a Py_INCREF here? XXX */
  Py_INCREF (self);

  return (self);
}

static PyObject *llpy_iter_iternext (PyObject *self)
{
  LLINES_PY_ITER *iter = (LLINES_PY_ITER *)self;
  RECORD record;

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_iter_iternext entry: type %c current %d refcnt %ld\n",
	       iter->li_type, iter->li_current, Py_REFCNT (self));
    }

  if (iter->li_current == -1)
    {
      /* trying to use an iter that was previously exhausted, raise an
	 exception */
      PyErr_SetObject (PyExc_StopIteration, Py_None);
      return NULL;
    }
  switch (iter->li_type)
    {
    case LLINES_TYPE_EVEN:
    case LLINES_TYPE_FAM:
    case LLINES_TYPE_INDI:
    case LLINES_TYPE_OTHR:
    case LLINES_TYPE_SOUR:
      iter->li_current = xref_next (iter->li_type, iter->li_current);
      break;
    default:      
      Py_UNREACHABLE ();	/* internal error */
    }
  if (iter->li_current == 0)
    {
      iter->li_current = -1;
      return NULL;		/* iter is now exhausted */
    }

  record = keynum_to_record (iter->li_type, iter->li_current);

  switch (iter->li_type)
    {
    case LLINES_TYPE_EVEN:
      {
	LLINES_PY_EVEN_RECORD *obj = PyObject_New (LLINES_PY_EVEN_RECORD, &llines_event_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->lre_type = LLINES_TYPE_EVEN;
	obj->lre_record = record;

	return (PyObject *)obj;
      }
    case LLINES_TYPE_INDI:
      {
	LLINES_PY_INDI_RECORD *obj = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->lri_type = LLINES_TYPE_INDI;
	obj->lri_record = record;

	return (PyObject *)obj;
      }
    case LLINES_TYPE_FAM:
      {
	LLINES_PY_FAM_RECORD *obj = PyObject_New (LLINES_PY_FAM_RECORD, &llines_family_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->lrf_type = LLINES_TYPE_FAM;
	obj->lrf_record = record;

	return (PyObject *)obj;
      }
    case LLINES_TYPE_SOUR:
      {
	LLINES_PY_SOUR_RECORD *obj = PyObject_New (LLINES_PY_SOUR_RECORD, &llines_source_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->lrs_type = LLINES_TYPE_SOUR;
	obj->lrs_record = record;

	return (PyObject *)obj;
      }
    case LLINES_TYPE_OTHR:
      {
	LLINES_PY_OTHR_RECORD *obj = PyObject_New (LLINES_PY_OTHR_RECORD, &llines_other_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->lro_type = LLINES_TYPE_OTHR;
	obj->lro_record = record;

	return (PyObject *)obj;
      }
    default:
      /* Something mighty weird is happening. Not only was there
	 an internal error for us to be called with an unexpected
	 type, but it was not caught by the previous switch
	 statment.  */
      Py_UNREACHABLE ();
    }
}
static struct PyMethodDef Lifelines_Iter_Methods[] =
  {
   { "individuals",	llpy_individuals, METH_NOARGS,
     "individuals(void) -> iterator for the set of all INDI in the database" },
   { "families",	llpy_families, METH_NOARGS,
     "families(void) -> iterator for the set of all FAM in the database" },
   { "sources",		llpy_sources, METH_NOARGS,
     "sources(void) -> iterator for the set of all SOUR in the database" },
   { "events",		llpy_events, METH_NOARGS,
     "events(void) --> iterator for the set of all EVEN in the database" },
   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_iter_type =
  {
   PyVarObject_HEAD_INIT (NULL, 0)
   .tp_name = "llines.Iter",
   .tp_doc = "Lifelines GEDCOM Iteration Type",
   .tp_basicsize = sizeof (LLINES_PY_ITER),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_iter_dealloc,
   .tp_methods = NULL,
   .tp_iter = llpy_iter_iter,
   .tp_iternext = llpy_iter_iternext,
  };

void llpy_iter_init (void)
{
  int status;

  status = PyModule_AddFunctions (Lifelines_Module, Lifelines_Iter_Methods);

  if (status != 0)
    fprintf (stderr, "llpy_iter_init: attempt to add functions returns %d\n", status);
}
