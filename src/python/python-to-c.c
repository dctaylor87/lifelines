/* python-to-c.c -- this module provides the glue for Python code to call into Lifelines C routines */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "python-to-c.h"
#include "llpy-externs.h"

#include "llstdlib.h"
#include "gedcom.h"

#include "database.h"
#include "family.h"
#include "person.h"
#include "version.h"		/* get_lifelines_version */
#include "types.h"

/* variables defined here */

PyObject *Lifelines_Module;

int llpy_debug = 1;	      /* XXX TODO: make this ia bitmask XXX */

/* forward references */

static PyObject *llpy_version (PyObject *self, PyObject *args);

/* start of code */

static PyObject *llpy_version (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  return (Py_BuildValue ("s", get_lifelines_version(100)));
}

PyObject *_llpy_key (PyObject *self, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "strip_prefix", NULL };
  LLINES_PY_RECORD *record = (LLINES_PY_RECORD *)self;
  int strip_prefix = 0;		/* by default, leave the prefix on */
  CNSTRING key;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|p", keywords, &strip_prefix))
    return NULL;

  key = nzkey(record->llr_record);
  /* currently the prefix is always a single character.  if that
     changes, this needs modification */
  return Py_BuildValue ("s", (strip_prefix ? key + 1 : key));
}

PyObject *_llpy_top_node (PyObject *self, PyObject *args)
{
  LLINES_PY_RECORD *py_record = (LLINES_PY_RECORD *) self;
  RECORD record = py_record->llr_record;
  NODE top = nztop (record);
  LLINES_PY_NODE *py_node;

  if (! top)
    {
      /* something went wrong -- cannot find the top node for this record */
      PyErr_SetString(PyExc_SystemError, "top_node: unable to find RECORD's top NODE");
      return NULL;
    }
  py_node = PyObject_New (LLINES_PY_NODE, &llines_node_type);
  if (! py_node)
    return NULL;

  py_node->lnn_type = py_record->llr_type;
  py_node->lnn_node = top;
  return ((PyObject *) py_node);
}

static struct PyMethodDef LifelinesMethods[] =
  {
   /* Person Functions -- moved to Lifelines_Person_Methods */

   /* GEDCOM Node Functions -- now in nodes.c */

   /* Event and Date Functions */

#if 0				/* XXX need serious thought due to existing implementation XXX */

   { "gettoday",	llpy_gettoday, METH_NOARGS,
     "doc string" },
   { "setdate",		llpy_setdate, METH_VARARGS,
     "doc string" },
   { "dayformat",	llpy_dayformat, METH_VARARGS,
     "dayformat(INT) --> VOID: set day format for stddate calls\n\
\t\t0 (leave space before single digit days)\n\
\t\t1 (use leading 0 before single digit days)\n\
\t\t2 (no space or leading 0 before single digit days)." },
   { "monthformat",	llpy_monthformat, METH_VARARGS,
     "monthformat(INT) --> VOID: set month format for stddate calls.\n\
\t\t0\tnumber with space before single digit months\n\
\t\t1\tnumber with leading zero before single digit months\n\
\t\t2\tnumber with no space for zero before single digit months\n\
\t\t3\tupper case abbreviation (eg.g. JAN, FEB) (localized)\n\
\t\t4\tcapitalized abbreviation (e.g., Jan, Feb) (localized)\n\
\t\t5\tupper case full word (e.g., JANUARY, FEBRUARY) (localized)\n\
\t\t6\tcapitalized full word (e.g., January, February) (localized)\n\
\t\t7\tlower case abbreviations (e.g., jan, feb) (localized)\n\
\t\t8\tlower case full word (e.g., january, february) (localized)\n\
\t\t9\tupper case abbreviation in English per GEDCOM (e.g., JAN, FEB)\n\
\t\t10\tlower case roman letter (e.g., i, ii)\n\
\t\t11\tupper case roman letter (e.g., I, II)\n" },
   { "yearformat",	llpy_yearformat, METH_VARARGS,
     "yearformat(INT) --> VOID: set year format for stddate calls\n\
\t\t0 (use leading spaces before years with less than four digits)\n\
\t\t1 (use leading 0 before years with less than four digits\n\
\t\t2 (no space or leading 0 before years)." },
   { "eraformat",	llpy_eraformat, METH_VARARGS,
     "eraformat(INT) --> VOID: set era format for stddate calls\n\
\t\t0 (no AD/BC markers)\n\
\t\t1 (trailing B.C. if appropriate)\n\
\t\t2 (trailing A.D. or B.C.)\n\
\t\t11 (trailing BC if appropriate)\n\
\t\t12 (trailing AD or BC)\n\
\t\t21 (trailing B.C.E. if appropriate)\n\
\t\t22 (trailing C.E. or B.C.E.)\n\
\t\t31 (trailing BC if appropriate)\n\
\t\t32 (trailing CE or BCE)." },
   { "dateformat",	llpy_dateformat, METH_VARARGS,
     "dateformat(INT) --> VOID: set date format for stddate calls." },
   { "datepic",		llpy_datepic, METH_VARARGS,
     "datepic(STRING) --> VOID: set custom date format for stddate calls (what about errors?)." },
   { "stddate",		llpy_stddate, METH_VARARGS,
     "doc string" },
   { "complexformat",	llpy_complexformat, METH_VARARGS,
     "doc string" },
   { "complexpic",	llpy_complexpic, METH_VARARGS,
     "doc string" },
   { "complexdate",	llpy_complexdate, METH_VARARGS,
     "doc string" },
   { "dayofweek",	llpy_dayofweek, METH_VARARGS,
     "doc string" },
   { "date2jd",		llpy_date2jd, METH_VARARGS,
     "doc string" },
   { "jd2date",		llpy_jd2date, METH_VARARGS,
     "doc string" },
#endif

   /* Value Extraction Functions */
#if 0
   { "extractdate",	llpy_extractdate, METH_VARARGS,
     "doc string" },
   { "extractnames",	llpy_extractnames, METH_VARARGS,
     "doc string" },
   { "extractplaces",	llpy_extractplaces, METH_VARARGS,
     "doc string" },

   { "extractdatestr",	llpy_extractdatestr, METH_VARARGS,
     "doc string" },
#endif

   /* User Interaction Functions */

#if 0				/* XXX until we figure out what to do about setx XXX */
   { "getindiset",	llpy_getindiset, METH_VARARGS,
     "identify set of persons through user interface" },
#endif
#if 0
   { "chooseindi",	llpy_chooseindi, METH_VARARGS,
     "doc string" },
   { "choosesubset",	llpy_choosesubset, METH_VARARGS,
     "doc string" },
#endif

   /* Person Set Functions and GEDCOM Extraction */

#if 0				/* XXX until we figure out what to do about sets XXX */
   { "indiset",		llpy_indiset, METH_VARARGS,
     "doc string" },
   { "spouseset",	llpy_spouseset, METH_VARARGS,
     "doc string" },

   { "gengedcomstrong",	llpy_gengedcomstrong, METH_VARARGS,
     "doc string" },
#endif

   /* Miscellaneous Functions */

#if 0
   { "program",		llpy_program, METH_NOARGS,
     "program(void) -> STRING: return name of the current program" },
#endif
   { "version",		llpy_version, METH_NOARGS,
     "version(void) -> STRING: return version of current program" },

   { NULL, 0, 0, NULL }		/* sentinel */
};

static PyModuleDef lifelines_module =
  {
   PyModuleDef_HEAD_INIT,
   .m_name = "llines",
   .m_doc = "Lifelines extension module",
   .m_size = -1,
   .m_methods = LifelinesMethods,
  };

PyMODINIT_FUNC
PyInit_llines(void)
{
  PyObject *module;

  if (PyType_Ready(&llines_node_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_family_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_individual_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_event_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_source_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_other_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_record_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_database_type) < 0)
    return NULL;

  if (PyType_Ready(&llines_iter_type) < 0)
    return NULL;

  module = PyModule_Create (&lifelines_module);
  if (module == NULL)
    return NULL;

  Lifelines_Module = module;
  Py_INCREF (Lifelines_Module);	/* not sure if this is needed... */

  Py_INCREF (&llines_node_type);
  if (PyModule_AddObject (module, "Node", (PyObject *) &llines_node_type) < 0)
    {
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  Py_INCREF (&llines_family_type);
  if (PyModule_AddObject (module, "Family", (PyObject *) &llines_family_type) < 0)
    {
      Py_DECREF (&llines_family_type);
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  Py_INCREF (&llines_individual_type);
  if (PyModule_AddObject (module, "Individual", (PyObject *) &llines_individual_type) < 0)
    {
      Py_DECREF (&llines_individual_type);
      Py_DECREF (&llines_family_type);
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  Py_INCREF (&llines_event_type);
  if (PyModule_AddObject (module, "Event", (PyObject *) &llines_event_type) < 0)
    {
      Py_DECREF (&llines_event_type);
      Py_DECREF (&llines_individual_type);
      Py_DECREF (&llines_family_type);
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  Py_INCREF (&llines_source_type);
  if (PyModule_AddObject (module, "Source", (PyObject *) &llines_source_type) < 0)
    {
      Py_DECREF (&llines_source_type);
      Py_DECREF (&llines_event_type);
      Py_DECREF (&llines_individual_type);
      Py_DECREF (&llines_family_type);
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  Py_INCREF (&llines_other_type);
  if (PyModule_AddObject (module, "Other", (PyObject *) &llines_other_type) < 0)
    {
      Py_DECREF (&llines_other_type);
      Py_DECREF (&llines_source_type);
      Py_DECREF (&llines_event_type);
      Py_DECREF (&llines_individual_type);
      Py_DECREF (&llines_family_type);
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  Py_INCREF (&llines_record_type);
  if (PyModule_AddObject (module, "Record", (PyObject *) &llines_record_type) < 0)
    {
      Py_DECREF (&llines_record_type);
      Py_DECREF (&llines_other_type);
      Py_DECREF (&llines_source_type);
      Py_DECREF (&llines_event_type);
      Py_DECREF (&llines_individual_type);
      Py_DECREF (&llines_family_type);
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  Py_INCREF (&llines_iter_type);
  if (PyModule_AddObject (module, "Iter", (PyObject *) &llines_iter_type) < 0)
    {
      Py_DECREF (&llines_iter_type);
      Py_DECREF (&llines_record_type);
      Py_DECREF (&llines_other_type);
      Py_DECREF (&llines_source_type);
      Py_DECREF (&llines_event_type);
      Py_DECREF (&llines_individual_type);
      Py_DECREF (&llines_family_type);
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  Py_INCREF (&llines_database_type);
  if (PyModule_AddObject (module, "Database", (PyObject *) &llines_database_type) < 0)
    {
      Py_DECREF (&llines_database_type);
      Py_DECREF (&llines_iter_type);
      Py_DECREF (&llines_record_type);
      Py_DECREF (&llines_other_type);
      Py_DECREF (&llines_source_type);
      Py_DECREF (&llines_event_type);
      Py_DECREF (&llines_individual_type);
      Py_DECREF (&llines_family_type);
      Py_DECREF (&llines_node_type);
      Py_DECREF (module);
      return NULL;
    }

  llpy_user_init ();
  llpy_set_init ();
  llpy_iter_init ();
  llpy_database_init ();
  llpy_nodes_init ();
  llpy_records_init ();

  return (module);
}

/* XXX when Python support is compiled in, this routine must be called
   during the llines initialization phase. XXX */

void llpy_init (void)
{
  PyImport_AppendInittab ("llines", &PyInit_llines);
}
