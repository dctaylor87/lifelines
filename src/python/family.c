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
#include "liflines.h"		/* choose_from_indiseq */
#include "messages.h"

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
static PyObject *llpy_nextfam (PyObject *self, PyObject *args);
static PyObject *llpy_prevfam (PyObject *self, PyObject *args);
static PyObject *llpy_children (PyObject *self, PyObject *args);
static PyObject *llpy_spouses (PyObject *self, PyObject *args);
static PyObject *llpy_choosechild_f (PyObject *self, PyObject *args);

static void llpy_family_dealloc (PyObject *self);

static PyObject *llpy_marriage (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  NODE fam_node;
  NODE event = NULL;
  LLINES_PY_EVEN_NODE *marr;

  fam_node = nztop (fam->lrf_record);
  event = MARR (fam_node);

  if (! event)
    Py_RETURN_NONE;

  marr = PyObject_New (LLINES_PY_EVEN_NODE, &llines_event_type);
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
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  NODE fam_node;
  NODE husb_node;
  LLINES_PY_INDI_RECORD *husb;
  CNSTRING key;

  fam_node = nztop (fam->lrf_record);
  husb_node = HUSB (fam_node);
  if (! husb_node)
    /* family doesn't have a recorded husband */
    Py_RETURN_NONE;

  key = rmvat (nval (husb_node));
  if (! key)
    Py_RETURN_NONE;		/* it has a HUSB line, but no value -- no husband */

  husb = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
  if (! husb)
    return NULL;		/* PyOBject_New failed -- out of memory? */

  husb->lri_record = key_to_irecord (key);
  husb->lri_type = LLINES_TYPE_INDI;

  return (PyObject *)husb;
}

/* llpy_wife (FAM) --> INDI

   Returns the first WIFE of the family.  None if there are none.  */

static PyObject *llpy_wife (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  NODE fam_node;
  NODE wife_node;
  LLINES_PY_INDI_RECORD *wife;
  CNSTRING key;

  fam_node = nztop (fam->lrf_record);
  wife_node = WIFE (fam_node);
  if (! wife_node)
    /* family doesn't have a recorded wife */
    Py_RETURN_NONE;

  key = rmvat (nval (wife_node));
  if (! key)
    Py_RETURN_NONE;		/* it has a WIFE line, but not value -- no wife */

  wife = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
  if (! wife)
    return NULL;		/* PyOBject_New failed -- out of memory? */

  wife->lri_record = key_to_irecord (key);
  wife->lri_type = LLINES_TYPE_INDI;

  return (PyObject *)wife;
}

/* llpy_nchilden (FAM) --> INTEGER

   Returns the number of children in the family.  */

static PyObject *llpy_nchildren (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  RECORD fam_record = fam->lrf_record;
  int count = length_nodes (CHIL (nztop (fam_record)));

  return (Py_BuildValue ("i", count));
}

/* llpy_firstchild (FAM) --> INDI

   Returns the first child of FAM.  */

static PyObject *llpy_firstchild (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  NODE indi_node;
  LLINES_PY_INDI_RECORD *indi;
  CNSTRING key;

  indi_node = CHIL(nztop (fam->lrf_record));

  if (! indi_node)
    Py_RETURN_NONE;		/* no children */

  key = rmvat (nval (indi_node));
  if (! key)
    Py_RETURN_NONE;		/* malformed CHIL line -- no value, no  children */

  indi = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
  if (! indi)
    {
      /* PyObject_New failed -- presumably out of memory or corruption
	 detected.  It should have signalled an exception... */
      return NULL;
    }
  indi->lri_record = key_to_irecord (key);
  indi->lri_type = LLINES_TYPE_INDI;

  return (PyObject *)indi;
}

/* llpy_lastchild (FAM) --> INDI

   Returns the last child of FAM.  */

static PyObject *llpy_lastchild (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  NODE indi_node;
  NODE prev_node;
  LLINES_PY_INDI_RECORD *indi;
  CNSTRING key;

  indi_node = CHIL (nztop (fam->lrf_record));

  if (! indi_node)
    Py_RETURN_NONE;		/* no children */

  /* cycle through all remaining nodes, keeping most recent CHIL node */
  while (indi_node)
    {
      if (eqstr (ntag (indi_node), "CHIL"))
	prev_node = indi_node;
      indi_node = nsibling (indi_node);
    }
  key = rmvat (nval (prev_node));
  if (! key)
    Py_RETURN_NONE;		/* malformed CHIL line -- no value, no children */

  indi = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
  if (! indi)
    {
      /* PyObject_New failed -- presumably out of memory or corruption
	 detected.  It should have signalled an exception... */
      return NULL;
    }
  indi->lri_record = key_to_irecord (key);
  indi->lri_type = LLINES_TYPE_INDI;

  return (PyObject *)indi;
}

/* llpy_nextfam (FAM) --> FAM

   Returns the next family (in key order) in the database.  */

static PyObject *llpy_nextfam (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  NODE fam_node = nztop (fam->lrf_record);
  STRING key = fam_to_key(fam_node);
  char *endptr = 0;
  long key_val;

  errno = 0;
  key_val = strtol (&key[1], &endptr, 10);
  if (errno != 0)
    {
      /* XXX error occurred -- figure out how to raise an exception XXX */
      return NULL;
    }
  if (*endptr != '@')
    {
      /* XXX should not happen -- raise an error XXX */
      return NULL;
    }
  key_val = (long)xref_nexti ((INT)key_val);

  if (key_val == 0)
    Py_RETURN_NONE;		/* no more -- we have reached the end */

  fam = PyObject_New (LLINES_PY_FAM_RECORD, &llines_family_type);
  fam->lrf_type = LLINES_TYPE_FAM;
  fam->lrf_record = keynum_to_irecord (key_val);
  return (PyObject *)fam;
}

/* llpy_prevfam (FAM) --> FAM

   Returns the previous family (in key order) in the database.  */

static PyObject *llpy_prevfam (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  NODE fam_node = nztop (fam->lrf_record);
  STRING key = fam_to_key(fam_node);
  char *endptr = 0;
  long key_val;

  errno = 0;
  key_val = strtol (&key[1], &endptr, 10);
  if (errno != 0)
    {
      /* XXX error occurred -- figure out how to raise an exception XXX */
      return NULL;
    }
  if (*endptr != '@')
    {
      /* XXX should not happen -- raise an error XXX */
      return NULL;
    }
  key_val = (long)xref_previ ((INT)key_val);

  if (key_val == 0)
    Py_RETURN_NONE;		/* no more -- we have reached the end */

  fam = PyObject_New (LLINES_PY_FAM_RECORD, &llines_family_type);
  fam->lrf_type = LLINES_TYPE_FAM;
  fam->lrf_record = keynum_to_irecord (key_val);
  return (PyObject *)fam;
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

/* llpy_choosechild (FAM) --> INDI

   Figures out FAM's set of children and asks the user to choose one.
   Returns None if FAM has no children or if the user cancelled the
   operation. */

static PyObject *llpy_choosechild_f (PyObject *self, PyObject *args)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  NODE node = nztop (fam->lrf_record);
  INDISEQ seq=0;
  RECORD record;
  LLINES_PY_INDI_RECORD *indi;

  if (! node)
    Py_RETURN_NONE;		/* XXX should this be an exception? XXX */

  seq = fam_to_children (node);

  if (! seq || (length_indiseq (seq) < 1))
      Py_RETURN_NONE;	/* no children to choose from */

  record = choose_from_indiseq(seq, DOASK1, _(qSifonei), _(qSnotonei));
  remove_indiseq (seq);

  indi = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
  if (! indi)
    return NULL;

  indi->lri_type = LLINES_TYPE_INDI;
  indi->lri_record = record;

  return (PyObject *)indi;
}

static void llpy_family_dealloc (PyObject *self)
{
  LLINES_PY_FAM_RECORD *fam = (LLINES_PY_FAM_RECORD *) self;
  release_record (fam->lrf_record);
  fam->lrf_record = 0;
  fam->lrf_type = 0;
  Py_TYPE(self)->tp_free (self);
  Py_DECREF (Py_TYPE(self));
}

static PyObject *llpy_family_iter(PyObject *self)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (! iter)
    return NULL;

  iter->li_current = 0;
  iter->li_type = LLINES_TYPE_FAM;

  return (PyObject *)iter;
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

   { "nextfam",		llpy_nextfam, METH_NOARGS,
     "(FAM).nextfam -> FAM: next family in database after FAM (in key order)" },
   { "prevfam",		llpy_prevfam, METH_NOARGS,
     "(FAM).prevfam -> FAM: previous family in database before FAM (in key order)" },
   { "children",	llpy_children, METH_NOARGS,
     "(FAM).children -> set of children in the family" },
   { "spouses",		llpy_spouses, METH_NOARGS,
     "(FAM).spouses -> set of spouses in the family" },

   { "choosechild",	llpy_choosechild_f, METH_NOARGS,
     "(FAM).choosechild --> INDI.  Select and return child of family\n\
 through user interface. Returns None if family has no children." },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_family_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Family",
   .tp_doc = "Lifelines GEDCOM Family Record",
   .tp_basicsize = sizeof (LLINES_PY_FAM_RECORD),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_family_dealloc,
   .tp_iter = llpy_family_iter,
   .tp_hash = llines_record_hash,
   .tp_richcompare = llines_record_richcompare,
   .tp_methods = Lifelines_Family_Methods,
  };
