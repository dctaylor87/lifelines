/* source.c -- source function.

   These are the bulk of the functions that operation on source records. */

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "lloptions.h"

#include "gedcom.h"		/* RECORD */

#include "python-to-c.h"
#include "types.h"

/* forward references */

static void llpy_source_dealloc (PyObject *self);

static void llpy_source_dealloc (PyObject *self)
{
  LLINES_PY_SOUR_RECORD *sour = (LLINES_PY_SOUR_RECORD *) self;
  release_record (sour->lrs_record);
  sour->lrs_record = 0;
  sour->lrs_type = 0;
  Py_TYPE(self)->tp_free (self);
  Py_DECREF (Py_TYPE(self));
}

static PyObject *llpy_source_iter(PyObject *self)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (! iter)
    return NULL;

  iter->li_current = 0;
  iter->li_type = LLINES_TYPE_SOUR;

  return (PyObject *)iter;
}

static struct PyMethodDef Lifelines_Source_Methods[] =
  {
   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_source_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Source",
   .tp_doc = "Lifelines GEDCOM Source Record",
   .tp_basicsize = sizeof (LLINES_PY_SOUR_RECORD),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_source_dealloc,
   .tp_iter = llpy_source_iter,
   .tp_methods = Lifelines_Source_Methods,
  };
