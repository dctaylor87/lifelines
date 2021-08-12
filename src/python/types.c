#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "llpy-externs.h"

#include "llstdlib.h"
#include "gedcom.h"
#include "pvalue.h"

#include "types.h"

PyTypeObject llines_node_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Node",
   .tp_doc = "Lifelines GEDCOM node",
   .tp_basicsize = sizeof (LLINES_PY_NODE),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
  };

PyTypeObject llines_event_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Event",
   .tp_doc = "Lifelines GEDCOM Event Record",
   .tp_basicsize = sizeof (LLINES_PY_EVEN),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
  };

PyTypeObject llines_source_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Source",
   .tp_doc = "Lifelines GEDCOM Source Record",
   .tp_basicsize = sizeof (LLINES_PY_SOUR),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
  };

PyTypeObject llines_other_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Other",
   .tp_doc = "Lifelines GEDCOM Other Record",
   .tp_basicsize = sizeof (LLINES_PY_OTHR),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
  };

PyTypeObject llines_record_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Record",
   .tp_doc = "Lifelines GEDCOM generic record",
   .tp_basicsize = sizeof (LLINES_PY_RECORD),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
  };

PyModuleDef lifelines_module =
  {
   PyModuleDef_HEAD_INIT,
   .m_name = "llines",
   .m_doc = "Lifelines extensions",
   .m_size = -1,
  };

PyMODINIT_FUNC
PyInit_llines(void)
{
  PyObject *module;

  if (PyType_Ready(&llines_node_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_event_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_source_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_node_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_record_type) < 0)
    return NULL;

  module = PyModule_Create (&lifelines_module);
  if (module == NULL)
    return NULL;

  Py_INCREF (&llines_node_type);
  if (PyModule_AddObject (module, "Node", (PyObject *) &llines_node_type) < 0) {
    Py_DECREF (&llines_node_type);
    Py_DECREF (module);
    return NULL;
  }

  Py_INCREF (&llines_event_type);
  if (PyModule_AddObject (module, "Event", (PyObject *) &llines_event_type) < 0) {
    Py_DECREF (&llines_event_type);
    Py_DECREF (module);
    return NULL;
  }

  Py_INCREF (&llines_source_type);
  if (PyModule_AddObject (module, "Source", (PyObject *) &llines_source_type) < 0) {
    Py_DECREF (&llines_source_type);
    Py_DECREF (module);
    return NULL;
  }

  Py_INCREF (&llines_other_type);
  if (PyModule_AddObject (module, "Other", (PyObject *) &llines_other_type) < 0) {
    Py_DECREF (&llines_other_type);
    Py_DECREF (module);
    return NULL;
  }

  Py_INCREF (&llines_record);
  if (PyModule_AddObject (module, "Record", (PyObject *) &llines_record_type) < 0) {
    Py_DECREF (&llines_record_type);
    Py_DECREF (module);
    return NULL;
  }

  return (module);
}
