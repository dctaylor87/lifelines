/* iter.c -- iter types */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "uiprompts.h"		/* DOASK1 */
#include "gedcom.h"		/* RECORD */
#include "types.h"

/* forward references */

static void llpy_iter_dealloc (PyObject *self);
static PyObject *llpy_iter_iter (PyObject *self);
static PyObject *llpy_iter_iternext (PyObject *self);

/* start of code */

static void llpy_iter_dealloc (PyObject *self)
{
  abort ();
}

static PyObject *llpy_iter_iter (PyObject *self)
{
  /* XXX do we need to do a Py_INCREF here? XXX */
  return (self);
}

static PyObject *llpy_iter_iternext (PyObject *self)
{
  LLINES_PY_ITER *iter = (LLINES_PY_ITER *)self;
  RECORD record;

  if (iter->li_current == -1)
    {
      /* XXX trying to use an iter that was previously exhausted,
	 raise an exception XXX */
      return NULL;
    }
  switch (iter->li_type)
    {
    case LLINES_TYPE_INDI:
    case LLINES_TYPE_FAM:
    case LLINES_TYPE_SOUR:
    case LLINES_TYPE_OTHR:
      iter->li_current = xref_next (iter->li_type, iter->li_current);
      break;
    default:      
      Py_UNREACHABLE ();	/* internal error */
    }
  if (iter->li_current == 0)
    {
      iter->li_current = -1;
      return NULL;		/* iter exhausted */
    }

  record = keynum_to_record (iter->li_type, iter->li_current);

  switch (iter->li_type)
    {
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
