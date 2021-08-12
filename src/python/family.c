/* family.c -- family functions.

   These are the bulk of the functions that are documented in the
   'Family functions' section of the manual.  */

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "lloptions.h"

#include "gedcom.h"		/* RECORD */
#include "indiseq.h"		/* INDISEQ */

#include "python-to-c.h"
#include "types.h"
#include "family.h"

/* forward references */

static PyObject *llpy_marriage (PyObject *self, PyObject *args);
static PyObject *llpy_husband (PyObject *self, PyObject *args);
static PyObject *llpy_wife (PyObject *self, PyObject *args);
static PyObject *llpy_nchildren (PyObject *self, PyObject *args);
static PyObject *llpy_nextfam (PyObject *self, PyObject *args);
static PyObject *llpy_prevfam (PyObject *self, PyObject *args);
static PyObject *llpy_firstchild (PyObject *self, PyObject *args);
static PyObject *llpy_lastchild (PyObject *self, PyObject *args);

static PyObject *llpy_marriage (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM *fam = (LLINES_PY_FAM *) self;
  NODE fam_node;
  NODE event = NULL;
  LLINES_PY_EVEN_NODE *marr;

  fam_node = nztop (fam->llf_record);
  event = MARR (fam_node);

  marr = PyObject_New (LLINES_PY_EVEN, &llines_event_type);
  if (! marr)
    return NULL;		/* PyObject_New failed? -- out of memory?  */

  marr->lne_node = event;
  marr->lne_type = LLINES_TYPE_EVEN;

  return ((PyObject *)marr);
}

/* llpy_husband (FAM) --> INDI

   Returns the first HUSB of the family.  None if there are none.  */

static PyObject *llpy_husband (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM *fam = (LLINES_PY_FAM *) self;
  NODE fam_node;
  NODE husb_node;
  LLINES_PY_INDI_NODE *husb;

  fam_node = nztop (fam->llf_record);
  husb_node = HUSB (fam_node);
  if (! husb_node)
    /* family doesn't have a recorded husband */
    Py_RETURN_NONE;

  husb = PyObject_New (LLINES_PY_INDI_NODE, &llines_individual_type);
  if (! husb)
    return NULL;		/* PyOBject_New failed -- out of memory? */

  husb->lni_node = husb_node;
  husb->lni_type = LLINES_TYPE_INDI;

  return (PyObject *)husb;
}

/* llpy_wife (FAM) --> INDI

   Returns the first WIFE of the family.  None if there are none.  */

static PyObject *llpy_wife (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM *fam = (LLINES_PY_FAM *) self;
  NODE fam_node;
  NODE wife_node;
  RECORD record;
  LLINES_PY_INDI_NODE *wife;

  fam_node = nztop (fam->llf_record);
  wife_node = WIFE (fam_node);
  if (! wife_node)
    /* family doesn't have a recorded wife */
    Py_RETURN_NONE;

  record  = node_to_record (wife_node);
  wife = PyObject_New (LLINES_PY_INDI_NODE, &llines_individual_type);
  if (! wife)
    return NULL;		/* PyOBject_New failed -- out of memory? */

  wife->lni_record = record;
  wife->lni_type = LLINES_TYPE_INDI;

  return (PyObject *)wife;
}

/* llpy_nchilden (FAM) --> INTEGER

   Returns the number of children in the family.  */

static PyObject *llpy_nchildren (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM *fam = (LLINES_PY_FAM *) self;
  RECORD fam_record = fam->llf_record;
  int count = length_nodes (FAMC (nztop (fam_record)));

  return (Py_BuildValue ("i", count));
}

/* llpy_firstchild (FAM) --> INDI

   Returns the first child of FAM.  */

static PyObject *llpy_firstchild (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM *fam = (LLINES_PY_FAM *) self;
  NODE indi_node;
  RECORD record;
  LLINES_PY_INDI_NODE *indi;

  indi_node = fam_to_first_chil (nztop (fam->llf_record));
  if (! indi_node)
    Py_RETURN_NONE;		/* no children */

  record = node_to_record (indi_node);

  indi = PyObject_New (LLINES_PY_INDI_NODE, &llines_individual_type);
  if (! indi)
    {
      /* PyObject_New failed -- presumably out of memory or corruption
	 detected.  It should have signalled an exception... */
      return NULL;
    }
  indi->lni_record = record;
  indi->lni_type = LLINES_TYPE_INDI;

  return (PyObject *)indi;
}

/* llpy_lastchild (FAM) --> INDI

   Returns the last child of FAM.  */

static PyObject *llpy_lastchild (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM *fam = (LLINES_PY_FAM *) self;
  NODE indi_node;
  RECORD record;
  LLINES_PY_INDI_NODE *indi;

  indi_node = fam_to_last_chil (nztop (fam->llf_record));
  if (! indi_node)
    Py_RETURN_NONE;		/* no children */

  record = node_to_record (indi_node);

  indi = PyObject_New (LLINES_PY_INDI_NODE, &llines_record_type);
  if (! indi)
    {
      /* PyObject_New failed -- presumably out of memory or corruption
	 detected.  It should have signalled an exception... */
      Py_RETURN_NONE;
    }
  indi->lni_record = record;
  indi->lni_type = LLINES_TYPE_INDI;

  return (PyObject *)indi;
}

/* llpy_nextfam (FAM) --> FAM

   Returns the next family (in key order) in the database.  */

static PyObject *llpy_nextfam (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM *fam = (LLINES_PY_FAM *) self;
  abort ();
}

/* llpy_prevfam (FAM) --> FAM

   Returns the previous family (in key order) in the database.  */

static PyObject *llpy_prevfam (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  abort ();
}

/* llpy_children (FAM) --> set of INDI

   Returns the set of children in the family.  */

static PyObject *llpy_children (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  abort ();
}

/* llpy_spouses (FAM) --> set of INDI

   Returns the set of spouses in the family.  */

static PyObject *llpy_spouses (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  abort ();
}

static struct PyMethodDef Lifelines_Family_Methods[] =
  {
   { "marriage",	llpy_marriage, METH_NOARGS,
     "(FAM).marriage --> EVENT: First marriage event of FAM; None if no event is found." },
   { "husband",		llpy_husband, METH_NOARGS,
     "(FAM).husband --> INDI: First husband of FAM; None if no person in the role." },
   { "wife",		llpy_wife, METH_NOARGS,
     "(FAM).wife --> INDI: FIrst wife of FAM; None if no person in the role." },
   { "nchildren",	llpy_nchildren, METH_NOARGS,
     "(FAM).nchildren -> INTEGER: number of children in FAM." },
   { "firstchild",	llpy_firstchild, METH_NOARGS,
     "(FAM).firstchild -> INDI: first child of FAM; None if no children." },
   { "lastchild",	llpy_lastchild, METH_NOARGS,
     "(FAM).lastchild -> INDI: last child of FAM; None if no children." },

   { "nextfam",		llpy_nextfam, METH_VARARGS,
     "(FAM).nextfam -> FAM: next family in database after FAM (in key order)" },
   { "prevfam",		llpy_prevfam, METH_VARARGS,
     "(FAM).prevfam -> FAM: previous family in database before FAM (in key order)" },
   { "children",	llpy_children, METH_NOARGS,
     "(FAM).children -> set of children in the family" },
   { "spouses",		llpy_spouses, METH_NOARGS,
     "(FAM).spouses -> set of spouses in the family" },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_family_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Family",
   .tp_doc = "Lifelines GEDCOM Family Record",
   .tp_basicsize = sizeof (LLINES_PY_FAM),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_methods = Lifelines_Family_Methods,
  };
