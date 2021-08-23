#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "llpy-externs.h"

#include "llstdlib.h"
#include "gedcom.h"
#include "pvalue.h"

#include "types.h"

/* one time initialization related code */

PyTypeObject llines_record_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Record",
   .tp_doc = "Lifelines GEDCOM generic Record",
   .tp_basicsize = sizeof (LLINES_PY_RECORD),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
  };
