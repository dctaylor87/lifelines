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

static void llpy_other_dealloc (PyObject *self);

static void llpy_other_dealloc (PyObject *self)
{
  LLINES_PY_OTHR_RECORD *othr = (LLINES_PY_OTHR_RECORD *) self;
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_family_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  release_record (othr->lro_record);
  othr->lro_record = 0;
  othr->lro_type = 0;
  Py_TYPE(self)->tp_free (self);
#if 0
  Py_DECREF (Py_TYPE(self));
#endif
}

static PyObject *llpy_other_iter(PyObject *self)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (! iter)
    return NULL;

  iter->li_current = 0;
  iter->li_type = LLINES_TYPE_OTHR;

  return (PyObject *)iter;
}

static struct PyMethodDef Lifelines_Other_Methods[] =
  {
   { "key", (PyCFunction)_llpy_key, METH_VARARGS | METH_KEYWORDS,
     "(OTHR).key([strip_prefix]) --> STRING.  Returns the database key of the record.\n\
If STRIP_PREFIX is True (default: False), the non numeric prefix is stripped." },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_other_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Other",
   .tp_doc = "Lifelines GEDCOM Other Record",
   .tp_basicsize = sizeof (LLINES_PY_OTHR_RECORD),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_other_dealloc,
   .tp_iter = llpy_other_iter,
   .tp_methods = Lifelines_Other_Methods,
  };
