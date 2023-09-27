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
  LLINES_PY_RECORD *sour = (LLINES_PY_RECORD *) self;
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_source_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  release_record (sour->llr_record);
  sour->llr_record = 0;
  sour->llr_type = 0;
  Py_TYPE(self)->tp_free (self);
}

#if 0
static PyObject *llpy_source_iter(PyObject *self)
{
  LLINES_PY_RECORD_ITER *iter = PyObject_New (LLINES_PY_RECORD_ITER,
					      &llines_record_iter_type);

  if (! iter)
    return NULL;

  iter->li_current = 0;
  iter->li_type = LLINES_TYPE_SOUR;

  return (PyObject *)iter;
}
#endif

static struct PyMethodDef Lifelines_Source_Methods[] =
  {
   { "key", (PyCFunction)_llpy_key, METH_VARARGS | METH_KEYWORDS,
     "(SOUR).key([strip_prefix]) --> STRING.  Returns the database key of the record.\n\
If STRIP_PREFIX is True (default: False), the non numeric prefix is stripped." },
   { "top_node", (PyCFunction)_llpy_top_node, METH_NOARGS,
     "top_node(void) --> NODE.  Returns the top of the NODE tree associated with the RECORD." },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_source_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Source",
   .tp_doc = "Lifelines GEDCOM Source Record",
   .tp_basicsize = sizeof (LLINES_PY_RECORD),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_source_dealloc,
#if 0
   .tp_iter = llpy_source_iter,
#endif
   .tp_hash = llines_record_hash,
   .tp_richcompare = llines_record_richcompare,
   .tp_methods = Lifelines_Source_Methods,
  };
