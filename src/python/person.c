/* person.c -- person functions.

   These are the bulk of the functions that are documented in the
   'Person functions' section of the manual.  */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "lloptions.h"

#include "gedcom.h"		/* RECORD */
#include "indiseq.h"		/* INDISEQ */
#include "liflines.h"
#include "messages.h"

#include "python-to-c.h"
#include "types.h"
#include "person.h"
#include "set.h"

#define MAX_NAME_LENGTH	68 /* see comment in llrpt_fullname (interp/builtin.c) */

/* forward references */

static PyObject *llpy_name (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_fullname (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_surname (PyObject *self, PyObject *args);
static PyObject *llpy_givens (PyObject *self, PyObject *args);
static PyObject *llpy_trimname (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_birth (PyObject *self, PyObject *args);
static PyObject *llpy_death (PyObject *self, PyObject *args);
static PyObject *llpy_burial (PyObject *self, PyObject *args);
static PyObject *llpy_father (PyObject *self, PyObject *args);
static PyObject *llpy_mother (PyObject *self, PyObject *args);
static PyObject *llpy_nextsib (PyObject *self, PyObject *args);
static PyObject *llpy_prevsib (PyObject *self, PyObject *args);
static PyObject *llpy_sex (PyObject *self, PyObject *args);
static PyObject *llpy_male (PyObject *self, PyObject *args);
static PyObject *llpy_female (PyObject *self, PyObject *args);
static PyObject *llpy_pn (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_nspouses (PyObject *self, PyObject *args);
static PyObject *llpy_nfamilies (PyObject *self, PyObject *args);
static PyObject *llpy_parents (PyObject *self, PyObject *args);

static PyObject *llpy_title (PyObject *self, PyObject *args);

static PyObject *llpy_soundex (PyObject *self, PyObject *args);

static PyObject *llpy_nextindi (PyObject *self, PyObject *args);
static PyObject *llpy_previndi (PyObject *self, PyObject *args);
static PyObject *llpy_choosechild_i (PyObject *self, PyObject *args);
static PyObject *llpy_choosespouse_i (PyObject *self, PyObject *args);
static PyObject *llpy_choosefam (PyObject *self, PyObject *args);

static PyObject *llpy_spouses_i (PyObject *self, PyObject *args);


/* These four routines used to be in set.c, but got moved here because
   llpy_children has to be here and either add_children needs to be
   non-static or all four routines need to be in the same file.  */

static PyObject *llpy_children (PyObject *self, PyObject *args);
static PyObject *llpy_descendantset (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_childset (PyObject *self, PyObject *args, PyObject *kw);

/* helper routine for llpy_descendantset, llpy_childset, and llpy_children */
static int add_children (PyObject *obj, PyObject *working_set, PyObject *output_set);

static void llpy_individual_dealloc (PyObject *self);

/* NOTE on the function header comments that follow:

   The name shown is the C name.  The Python name, unless noted
   otherwise, is the same but with the llpy_ stripped off the front.
   The argument lists shown are the Python argument lists.  */

/* llpy_name (INDI, [CAPS]) --> NAME:

   Returns the default name of a person -- the name found on the first
   '1 NAME' line.  The slashes are removed.  If CAPS (optional) is
   True (default), the surname is made all capitals.  */

static PyObject *llpy_name (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  static char *keywords[] = { "caps", NULL };
  int caps = 1;
  NODE node_name = 0;
  STRING name = 0;
  
  if (! PyArg_ParseTupleAndKeywords (args, kw, "|p", keywords, &caps))
    return NULL;

  if (! (node_name = NAME (nztop (indi->llr_record))))
    {
      if (getlloptint("RequireNames", 0))
	/* XXX figure out how to issue exceptions XXX _("name: person does not have a name") */
	return NULL;

      Py_RETURN_NONE;
    }
  name = manip_name(nval(node_name), caps ? DOSURCAP : NOSURCAP, REGORDER, MAX_NAME_LENGTH);
  return (Py_BuildValue ("s", name));
}

/* llpy_fullname (INDI, [UPCASE], [KEEP_ORDER], [MAX_LENGTH]) --> STRING

   Returns the name of a person in a variety of formats.

   If UPCASE is True, the surname is shown in upper case, otherwise it
   is shown as in the record.  Default: False

   If KEEP_ORDER is True, the parts are shown in the order as found in
   the record, otherwise the surname is given first, followed by a
   comma, followed by the other name parts.  Default: True

   MAX_LENGTH specifies the maximum length that can be used to show
   the name.  Default: 0 (no maximum) */

static PyObject *llpy_fullname (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  static char *keywords[] = { "upcase", "keep_order", "max_length", NULL };
  int upcase = 0;
  int keep_order = 1;
  int max_length = 0;
  NODE node_name = 0;
  STRING name = 0;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|ppI", keywords, &upcase, &keep_order, &max_length))
    return NULL;

  if (! (node_name = NAME (nztop (indi->llr_record))) || ! nval(node_name))
    {
      if (getlloptint("RequireNames", 0))
	/* XXX figure out how to issue exceptions XXX _("fullname: person does not have a name") */
	return NULL;
      Py_RETURN_NONE;
    }
  if (max_length == 0)
    max_length = MAX_NAME_LENGTH;

  name = manip_name (nval (node_name), upcase ? DOSURCAP : NOSURCAP, keep_order, max_length);
  return (Py_BuildValue ("s", name));
}

/* llpy_surname (INDI) --> STRING

   Returns the surname as found in the first '1 NAME' line.  Slashes
   are removed. */

static PyObject *llpy_surname (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE node_name = 0;
  CNSTRING name = 0;

  node_name = nztop (indi->llr_record);
  if (! (node_name = NAME(node_name)) || ! nval(node_name))
    {
      if (getlloptint ("RequireNames", 0))
	/* XXX figure out how to issue exceptions XXX _("surname: person does not have a name" */
	return NULL;
      Py_RETURN_NONE;		/* XXX should it be an empty string instead? */
    }
  name = getasurname(nval(node_name));
  return (Py_BuildValue ("s", name));
}

/* llpy_givens (INDI) --> STRING

   Returns the given names of the person in the same order and format
   as found in the first '1 NAME' line of the record.  */

static PyObject *llpy_givens (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE name = 0;

  if (!(name = NAME(nztop (indi->llr_record))) || !nval(name))
    {
      if (getlloptint("RequireNames", 0))
	{
	  /* XXX throw error _("(givens) person does not have a name") XXX */
	  return NULL;
	}
      Py_RETURN_NONE;
    }
  return (Py_BuildValue ("s", givens(nval(name))));
}

/* llpy_trimname (INDI, MAX_LENGTH) --> STRING

   Returns the name trimmed to MAX_LENGTH.  */

static PyObject *llpy_trimname (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_RECORD *record_indi = (LLINES_PY_RECORD *) self;
  NODE indi = nztop(record_indi->llr_record);
  static char *keywords[] = { "max_length", NULL };
  int max_length;
  STRING str;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "I", keywords, &max_length))
    return NULL;

  if (!(indi = NAME(indi)) || ! nval(indi))
    {
      if (getlloptint("RequireNames", 0))
	/* XXX set exception -- _("(trimname) person does not have a name") XXX */
	return NULL;
    }
  str = name_string (trim_name (nval (indi), max_length));
  if (! str)
    str = "";

  return Py_BuildValue ("s", str);
}

/* llpy_birth (INDI) --> EVENT

   Returns the first birth event of INDI; None if no event is
   found.  */

static PyObject *llpy_birth (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop(indi->llr_record);
  NODE birth = BIRT(indi_node);
  LLINES_PY_EVEN_NODE *event;
  
  if (! birth)
    Py_RETURN_NONE;

  event = PyObject_New(LLINES_PY_EVEN_NODE, &llines_event_type);
  if (! event)
    return NULL;		/* PyObject_New failed */

  event->lne_node = birth;
  event->lne_type = LLINES_TYPE_EVEN;

  return (PyObject *)event;
}

/* llpy_death (INDI) --> EVENT

   Returns the first death event of INDI; None if no event is
   found.  */

static PyObject *llpy_death (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop(indi->llr_record);
  NODE death = DEAT(indi_node);
  LLINES_PY_EVEN_NODE *event;
  
  if (! death)
    Py_RETURN_NONE;

  event = PyObject_New(LLINES_PY_EVEN_NODE, &llines_event_type);
  if (! event)
    return NULL;		/* PyObject_New failed */

  event->lne_node = death;
  event->lne_type = LLINES_TYPE_EVEN;

  return (PyObject *)event;
}

/* llpy_burial (INDI) --> EVENT

   Returns the first burial event of INDI; None if no event is
   found.  */

static PyObject *llpy_burial (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop(indi->llr_record);
  NODE burial = BURI(indi_node);
  LLINES_PY_EVEN_NODE *event;
  
  if (! burial)
    Py_RETURN_NONE;

  event = PyObject_New(LLINES_PY_EVEN_NODE, &llines_event_type);
  if (! event)
    return NULL;		/* PyObject_New failed */

  event->lne_node = burial;
  event->lne_type = LLINES_TYPE_EVEN;

  return (PyObject *)event;
}

/* llpy_father (INDI) --> INDI

   Returns the first father of INDI; None if no person in the
   role.  */

static PyObject *llpy_father (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = indi_to_fath(nztop (indi->llr_record));
  LLINES_PY_RECORD *father;

  if (! indi_node)
    Py_RETURN_NONE;		/* no person in the role */

  father = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
  father->llr_record = node_to_record (indi_node);
  father->llr_type = LLINES_TYPE_INDI;

  return (PyObject *)father;
}

/* llpy_mother (INDI) --> INDI

   Returns the first mother of INDI; None if no person in the
   role.  */

static PyObject *llpy_mother (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = indi_to_moth(nztop (indi->llr_record));
  LLINES_PY_RECORD *mother;

  if (! indi_node)
    Py_RETURN_NONE;		/* no person in the role */

  mother = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
  mother->llr_record = node_to_record (indi_node);
  mother->llr_type = LLINES_TYPE_INDI;

  return (PyObject *)mother;
}

/* llpy_nextsib (INDI) --> INDI

   Returns the next (younger) sibling of INDI; None if no person in
   the role.  */

static PyObject *llpy_nextsib (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = indi_to_next_sib_old (nztop (indi->llr_record));
  LLINES_PY_RECORD *sibling;

  if (! indi_node)
    Py_RETURN_NONE;		/* no person in the role */

  sibling = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
  sibling->llr_record = node_to_record (indi_node);
  sibling->llr_type = LLINES_TYPE_INDI;

  return (PyObject *)sibling;
}

/* llpy_prevsib (INDI) --> INDI

   Returns the previous (older) sibling of INDI; None if no person in
   the role.  */

static PyObject *llpy_prevsib (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = indi_to_prev_sib_old (nztop (indi->llr_record));
  LLINES_PY_RECORD *sibling;

  if (! indi_node)
    Py_RETURN_NONE;		/* no person in the role */

  sibling = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
  sibling->llr_record = node_to_record (indi_node);
  sibling->llr_type = LLINES_TYPE_INDI;

  return (PyObject *)sibling;
}

/* llpy_sex (INDI) --> STRING

   Returns the sex of INDI (M, F, or U).  */

static PyObject *llpy_sex (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop (indi->llr_record);
  char *sex;

  if (SEX(indi_node) == SEX_FEMALE)
    sex = "F";
  else if (SEX(indi_node) == SEX_MALE)
    sex = "M";
  else
    sex = "U";

  return (Py_BuildValue ("s", sex));
}

/* llpy_male (INDI) --> BOOLEAN

   Returns True if male, False otherwise.  */

static PyObject *llpy_male (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop (indi->llr_record);
  BOOLEAN bool = (SEX(indi_node) == SEX_MALE);

  if (bool)
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

/* llpy_female (INDI) --> BOOLEAN

   Returns True if Female; False otherwise.  */

static PyObject *llpy_female (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop (indi->llr_record);
  BOOLEAN bool = (SEX(indi_node) == SEX_FEMALE);

  if (bool)
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

/* llpy_pn */

static PyObject *llpy_pn (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  static char *keywords[] = { "which", NULL };
  int which;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "i", keywords, &which))
    return NULL;

  abort ();
}

/* llpy_nspouses (INDI) --> INTEGER

   Returns the number of spouses of INDI.  */

static PyObject *llpy_nspouses (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE node = nztop (indi->llr_record);
  INT nspouses = 0;		/* throw away, used by macro */
  INT nactual = 0;

  FORSPOUSES(node, spouse, fam, nspouses)
    ++nactual;
  ENDSPOUSES

  return Py_BuildValue ("i", nactual);
}

/* llpy_nfamilies (INDI) --> INTEGER

   Returns the number of families (as spouse/parent) of INDI.

   NOTE: sensitive to Lifelines GEDCOM format which puts FAMS links
   *LAST*.  If this changes, this breaks.  */

static PyObject *llpy_nfamilies (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop (indi->llr_record);
  int count = length_nodes (FAMS (indi_node));

  return (Py_BuildValue ("I", count));
}

/* llpy_parents */

static PyObject *llpy_parents (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop (indi->llr_record);
  NODE fam_node = indi_to_famc (indi_node);
  LLINES_PY_RECORD *fam;

  if (! fam_node)
    Py_RETURN_NONE;		/* parents are unknown */

  fam = PyObject_New (LLINES_PY_RECORD, &llines_family_type);
  fam->llr_type = LLINES_TYPE_FAM;
  fam->llr_record = node_to_record (fam_node);

  return (PyObject *)fam;
}

/* llpy_title (INDI) --> STRING

   Returns the first '1 TITL' line in the record.  */

static PyObject *llpy_title (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE indi_node = nztop (indi->llr_record);
  NODE title = find_tag (nchild (indi_node), "TITL");

  if (! title)
    Py_RETURN_NONE;		/* no title found */

  return Py_BuildValue ("s", nval(title));
}

/* llpy_soundex (INDI) --> STRING

   Returns the SOUNDEX code of INDI.  */

static PyObject *llpy_soundex (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE name = 0;

  if (!(name = NAME(nztop (indi->llr_record))) || !nval(name))
    {
      if (getlloptint("RequireNames", 0))
	{
	  /* XXX throw error _("soundex: person does not have a name") XXX */
	  return NULL;
	}
      Py_RETURN_NONE;
    }
  return Py_BuildValue ("s", trad_soundex (getsxsurname (nval (name))));
}

/* llpy_root */

PyObject *llpy_root (PyObject *self, PyObject *args)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  abort ();
}

/* llpy_nextindi (INDI) --> INDI

   Returns the next INDI in the database (in key order).  */

static PyObject *llpy_nextindi (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  INT key_val = nzkeynum(indi->llr_record);

  if (key_val == 0)
    {
      /* unexpected internal error occurred -- raise an exception */
      PyErr_SetString(PyExc_SystemError, "nextindi: unable to determine RECORD's key");
      return NULL;
    }
  /* XXX xref_{next|prev}{i,f,s,e,x,} ultimately casts its argument to
     an INT32, so even if you are on a system where INT is INT64
     (e.g., x86_64), you are limited to INT_MAX keys -- despite the
     manual's assertion that keys can be as large as 9,999,999,999.
     Of course, iterating through that many keys would be painful...
     But, still...  Also, shouldn't the key value be *UNSIGNED*?  Keys
     are never negative.  Zero is reserved for *NOT FOUND / DOES NOT
     EXIST*.  XXX */
  key_val = (long)xref_nexti ((INT)key_val);

  if (key_val == 0)
    Py_RETURN_NONE;		/* no more -- we have reached the end */

  indi = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
  indi->llr_type = LLINES_TYPE_INDI;
  indi->llr_record = keynum_to_irecord (key_val);
  return (PyObject *)indi;
}

/* llpy_previndi (INDI) --> INDI

   Returns the previous INDI in the database (in key order).  */

static PyObject *llpy_previndi (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  INT key_val = nzkeynum (indi->llr_record);

  if (key_val == 0)
    {
      /* unexpected internal error occurred -- raise an exception */
      PyErr_SetString(PyExc_SystemError, "previndi: unable to determine RECORD's key");
      return NULL;
    }
  /* XXX xref_{next|prev}{i,f,s,e,x,} ultimately casts its argument to
     an INT32, so even if you are on a system where INT is INT64
     (e.g., x86_64), you are limited to INT_MAX keys -- despite the
     manual's assertion that keys can be as large as 9,999,999,999.
     Of course, iterating through that many keys would be painful...
     But, still...  Also, shouldn't the key value be *UNSIGNED*?  Keys
     are never negative.  Zero is reserved for *NOT FOUND / DOES NOT
     EXIST*.  XXX */
  key_val = (long)xref_previ ((INT)key_val);

  if (key_val == 0)
    Py_RETURN_NONE;		/* no more -- we have reached the end */

  indi = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
  indi->llr_type = LLINES_TYPE_INDI;
  indi->llr_record = keynum_to_irecord (key_val);
  return (PyObject *)indi;
}

/* llpy_choosechild_i (INDI) --> INDI

   Figures out INDI's set of children and asks the user to choose one.
   Returns None if INDI has no children or if the user cancelled the
   operation. */

static PyObject *llpy_choosechild_i (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE node = nztop (indi->llr_record);
  INDISEQ seq=0;
  RECORD record;

  if (! node)
    {
      /* unexpected internal error occurred -- raise an exception */
      PyErr_SetString(PyExc_SystemError, "choosechild: unable to find RECORD's top NODE");
      return NULL;
    }

  seq = indi_to_children (node);

  if (! seq || (length_indiseq (seq) < 1))
      Py_RETURN_NONE;	/* no children to choose from */

  record = choose_from_indiseq(seq, DOASK1, _(qSifonei), _(qSnotonei));
  remove_indiseq (seq);

  indi = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
  if (! indi)
    return NULL;

  indi->llr_type = LLINES_TYPE_INDI;
  indi->llr_record = record;

  return (PyObject *)indi;
}

static PyObject *llpy_choosespouse_i (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  NODE node = nztop (indi->llr_record);
  RECORD record;
  INDISEQ seq;
  LLINES_PY_RECORD *py_indi;

  seq = indi_to_spouses (node);
  if (! seq || (length_indiseq (seq) < 1))
    Py_RETURN_NONE;		/* no spouses for family */

  record = choose_from_indiseq (seq, DOASK1, _(qSifonei), _(qSnotonei));
  remove_indiseq (seq);
  if (! record)
    Py_RETURN_NONE;		/* user cancelled */

  py_indi = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
  if (! py_indi)
    return NULL;		/* no memory? */

  py_indi->llr_type = LLINES_TYPE_INDI;
  py_indi->llr_record = record;
  return ((PyObject *)py_indi);
}

/* llpy_choosefam(void) --> FAM */

static PyObject *llpy_choosefam (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  LLINES_PY_RECORD *fam;
  INDISEQ seq;
  RECORD record;

  seq = indi_to_families (nztop (indi->llr_record), TRUE);
  if (! seq || length_indiseq (seq) < 1)
    Py_RETURN_NONE;		/* person is not in any families */

  record = choose_from_indiseq(seq, DOASK1, _(qSifonei), _(qSnotonei));
  remove_indiseq (seq);
  if (! record)
    Py_RETURN_NONE;		/* user cancelled */

  fam = PyObject_New (LLINES_PY_RECORD, &llines_family_type);
  if (! fam)
    return NULL;		/* out of memory? */

  fam->llr_type = LLINES_TYPE_FAM;
  fam->llr_record = record;

  return (PyObject *) fam;
}

static PyObject *llpy_spouses_i (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  PyObject *output_set;	/* represents INDIs that are part of the return value */
  RECORD record = ((LLINES_PY_RECORD *)self)->llr_record;
  RECORD spouse;

  output_set = PySet_New (NULL);
  if (! output_set)
    return NULL;

  FORSPOUSES_RECORD(record,spouse)

    LLINES_PY_RECORD *new_indi = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
    if (! new_indi)
      {
	PySet_Clear (output_set);
	Py_DECREF (output_set);
	return NULL;
      }
    new_indi->llr_type = LLINES_TYPE_INDI;
    new_indi->llr_record = spouse;

    if (PySet_Add (output_set, (PyObject *)new_indi) < 0)
      {
	PySet_Clear (output_set);
	Py_DECREF (output_set);
	return NULL;
      }
  ENDSPOUSES_RECORD

  return (output_set);
}

/* llpy_descendantset(SET) -- given an input set of INDIs, produce an
   output set of those INDIs that are the descendants of the input
   INDIs.  */

static PyObject *llpy_descendantset (PyObject *self, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "set", NULL };
  PyObject *input_set;		/* set passed in */
  PyObject *working_set; /* represents INDIs that are not yet processed */
  PyObject *output_set;	/* represents INDIs that are part of the return value */
  PyObject *item;
  int status;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "O", keywords, &input_set))
    return NULL;

  output_set = PySet_New (NULL);
  if (! output_set)
    return NULL;

  working_set = PySet_New (NULL);
  if (! working_set)
    {
      Py_DECREF (output_set);
      return NULL;
    }

  /* propagate parents of the input set into working_set */
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_descendantset: processing input_set\n");
    }

  PyObject *iterator = PyObject_GetIter (input_set);
  if (! iterator)
    {
      Py_DECREF (working_set);
      Py_DECREF (output_set);
      return NULL;
    }
  while ((item = PyIter_Next (iterator)))
    {
      if ((status = add_children (item, working_set, NULL)) < 0)
	{
	  /* report status, cleanup, return NULL */
	  PySet_Clear (working_set);
	  Py_DECREF (working_set);
	  PySet_Clear (output_set);
	  Py_DECREF (output_set);
	  Py_DECREF (iterator);
	  return NULL;
	}
    }
  if (PyErr_Occurred())
    {
      /* clean up and return NULL */
      PySet_Clear (working_set);
      Py_DECREF (working_set);
      PySet_Clear (output_set);
      Py_DECREF (output_set);
      Py_DECREF (iterator);
      return NULL;
    }
  Py_DECREF (iterator);

  /* go through working_set, putting children into working_set, and
     item into output_set */
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_descendantset: processing working_set\n");
    }
  while ((PySet_GET_SIZE (working_set) > 0) && (item = PySet_Pop (working_set)))
    {
      int status = add_children (item, working_set, output_set);
      if (status < 0)
	{
	  /* report status, cleanup, return NULL */
	  PySet_Clear (working_set);
	  Py_DECREF (working_set);
	  PySet_Clear (output_set);
	  Py_DECREF (output_set);
	  return NULL;
	}
    }
  ASSERT (PySet_GET_SIZE (working_set) == 0);
  PySet_Clear (working_set);	/* should be empty! */
  Py_DECREF (working_set);

  return (output_set);
}

/* llpy_children(INDI) -- given an INDI, produce an output set of
   those INDIs that are children of the INDI */

static PyObject *llpy_children (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  PyObject *output_set = PySet_New (NULL);
  int status;

  if (! output_set)
    return NULL;

  status = add_children (self, output_set, NULL);
  if (status < 0)
    {
      PySet_Clear (output_set);
      Py_DECREF (output_set);
      return NULL;
    }

  return (output_set);
}

/* llpy_childset(SET) -- given an input set of INDIs, produce an
   output set of those INDIs that are the children of the input
   INDIs.  */

static PyObject *llpy_childset (PyObject *self, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "set", NULL };
  PyObject *input_set;		/* set passed in */
  PyObject *output_set;	/* represents INDIs that are part of the return value */
  PyObject *item;
  int status;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "O", keywords, &input_set))
    return NULL;

  output_set = PySet_New (NULL);
  if (! output_set)
    return NULL;

  /* propagate parents of the input set into output_set */
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_childset: processing input_set\n");
    }

  PyObject *iterator = PyObject_GetIter (input_set);
  if (! iterator)
    {
      Py_DECREF (output_set);
      return NULL;
    }
  while ((item = PyIter_Next (iterator)))
    {
      if ((status = add_children (item, output_set, NULL)) < 0)
	{
	  /* report status, cleanup, return NULL */
	  PySet_Clear (output_set);
	  Py_DECREF (output_set);
	  Py_DECREF (iterator);
	  return NULL;
	}
    }
  if (PyErr_Occurred())
    {
      /* clean up and return NULL */
      PySet_Clear (output_set);
      Py_DECREF (output_set);
      Py_DECREF (iterator);
      return NULL;
    }
  Py_DECREF (iterator);

  return (output_set);
}

/* add_children -- for an individual, first we look at output_set (if
   non NULL).  If the individual is already present, we return success
   as we have already processed him/her.  Otherwise we we add him/her
   to output_set.

   Then, we look at each of the familes that he/she is a spouse in and
   for each family found, we add the children to the working set.

   NOTE: output_set is NULL when we are doing the first pass of the
   input arguments.  */

static int add_children (PyObject *obj, PyObject *working_set, PyObject *output_set)
{
  int status;
  RECORD record;

  if (output_set)
    {
      status = PySet_Contains (output_set, obj);
      if (status == 1)
	{
	  if (llpy_debug)
	    {
	      fprintf (stderr, "add_children: INDI %s already present\n",
		       nzkey (((LLINES_PY_RECORD *)obj)->llr_record));
	    }
	  return (0);		/* already present, not to do, success */
	}
      else if (status < 0)
	return (status);	/* failure, let caller cleanup... */

      /* obj is not in output_set *AND* just cam (in caller) from working set */
      if (llpy_debug)
	{
	  fprintf (stderr, "add_children: adding INDI %s to output set\n",
		   nzkey (((LLINES_PY_RECORD *)obj)->llr_record));
	}
      if (PySet_Add (output_set, (PyObject *)obj) < 0)
	return (-8);
    }

  /* the individual is NOT present in output_set, so add him/her to
     working_set */

  record = ((LLINES_PY_RECORD *)obj)->llr_record;

  FORFAMS_RECORD(record, fam)
    FORCHILDREN_RECORD(fam, child)
    /* wrap the child in a PyObject and put him/her into working_set */
      LLINES_PY_RECORD *indi_rec = PyObject_New (LLINES_PY_RECORD,
						      &llines_individual_type);
      if (! indi_rec)
	return (-1);		/* caller will clean up */

      indi_rec->llr_type = LLINES_TYPE_INDI;
      indi_rec->llr_record = child;
      if (PySet_Add (working_set, (PyObject *)indi_rec) < 0)
	return (-2);

    ENDCHILDREN_RECORD
  ENDFAMS_RECORD
    
  return (0);
}

static void llpy_individual_dealloc (PyObject *self)
{
  LLINES_PY_RECORD *indi = (LLINES_PY_RECORD *) self;
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_family_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  release_record (indi->llr_record);
  indi->llr_record = 0;
  indi->llr_type = 0;
  Py_TYPE(self)->tp_free (self);
#if 0
  Py_DECREF (Py_TYPE(self));
#endif
}

static PyObject *llpy_individual_iter(PyObject *self ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (! iter)
    return NULL;

  iter->li_current = 0;
  iter->li_type = LLINES_TYPE_INDI;

  return (PyObject *)iter;
}

static struct PyMethodDef Lifelines_Person_Methods[] =
  {
   /* Person Functions */

   { "name",		(PyCFunction)llpy_name, METH_VARARGS | METH_KEYWORDS,
     "name([CAPS]) -->NAME; returns the name found on the first '1 NAME' line.\n\
If CAPS (optional) is True (default), the surname is made all capitals." },
   { "fullname",	(PyCFunction)llpy_fullname, METH_VARARGS | METH_KEYWORDS,
     "fullname(bool1, bool2, int) -->" },
   { "surname",		(PyCFunction)llpy_surname, METH_NOARGS,
     "surname(void) --> STRING: returns the surname as found in the first '1 NAME'\n\
line.  Slashes aree removed." },
   { "givens",		(PyCFunction)llpy_givens, METH_NOARGS,
     "givens(void) --> STRING: returns the given names of the person in the\n\
same order and format as found in the first '1 NAME' line of the record." },
   { "trimname",	(PyCFunction)llpy_trimname, METH_VARARGS | METH_KEYWORDS,
     "trimname(MAX_LENGTH) --> STRING; returns name trimmed to MAX_LENGTH." },
   { "birth",		(PyCFunction)llpy_birth, METH_NOARGS,
     "birth(void) -> EVENT: First birth event of INDI; None if no event is found." },
   { "death",		(PyCFunction)llpy_death, METH_NOARGS,
     "death(void) -> EVENT: First death event of INDI; None if no event is found." },
   { "burial",		(PyCFunction)llpy_burial, METH_NOARGS,
     "burial(void) -> EVENT: First burial event of INDI; None if no event is found." },
   { "father",		(PyCFunction)llpy_father, METH_NOARGS,
     "father(void) -> INDI: First father of INDI; None if no person in the role." },
   { "mother",		(PyCFunction)llpy_mother, METH_NOARGS,
     "mother(void) -> INDI: First mother of INDI; None if no person in the role." },
   { "nextsib",		(PyCFunction)llpy_nextsib, METH_NOARGS,
     "nextsib(void) -> INDI: next (younger) sibling of INDI. None if no person in the role." },
   { "prevsib",		(PyCFunction)llpy_prevsib, METH_NOARGS,
     "prevsib(void) -> INDI: previous (older) sibling of INDI. None if no person in the role." },
   { "sex",		(PyCFunction)llpy_sex, METH_NOARGS,
     "sex(void) -> STRING: sex of INDI (M, F, or U)." },
   { "male",		(PyCFunction)llpy_male, METH_NOARGS,
     "male(void) --> boolean: True if male, False otherwise." },
   { "female",		(PyCFunction)llpy_female, METH_NOARGS,
     "female(void) --> boolean: True if female, False otherwise." },
   { "pn",		(PyCFunction)llpy_pn, METH_VARARGS | METH_KEYWORDS,
     "pn(integer) --> STRING: pronoun referring to INDI\n\
\t\t0 (He/She), 1 (he/she), 2 (His/Her), 3 (his/her), 4 (him/her)" },
   { "nspouses",	(PyCFunction)llpy_nspouses, METH_NOARGS,
     "nspouses(void) -> INTEGER; Returns number of spouses of INDI." },
   { "nfamilies",	(PyCFunction)llpy_nfamilies, METH_NOARGS,
     "nfamilies(void) -> INTEGER; Returns number of families (as spouse/parent) of INDI" },
   { "parents",		llpy_parents, METH_VARARGS,
     "parents(void) --> FAM.  Returns the first FAM in which INDI is a child." },
   { "title",		(PyCFunction)llpy_title, METH_NOARGS,
     "title(void) -> STRING; Returns the value of the first '1 TITL' line in the record." },
   { "key", (PyCFunction)_llpy_key, METH_VARARGS | METH_KEYWORDS,
     "key([strip_prefix]) --> STRING.  Returns the database key of the record.\n\
If STRIP_PREFIX is True (default: False), the non numeric prefix is stripped." },
   { "soundex",		(PyCFunction)llpy_soundex, METH_NOARGS,
     "soundex(void) -> STRING: SOUNDEX code of INDI" },
   { "nextindi",	(PyCFunction)llpy_nextindi, METH_NOARGS,
     "nextindi(void) -> INDI: Returns next INDI (in database order)." },
   { "previndi",	(PyCFunction)llpy_previndi, METH_NOARGS,
     "previndi(void) -> INDI: Returns previous INDI (in database order)." },

   { "children",	(PyCFunction)llpy_children, METH_NOARGS,
     "children(void) --> SET.  Returns the set of children of INDI." },
   /* User Interaction Functions */

   { "choosechild",	llpy_choosechild_i, METH_NOARGS,
     "choosechild(void) -> INDI; Selects and returns child of person\n\
through user interface.  Returns None if INDI has no children." },
   { "choosepouse",	llpy_choosespouse_i, METH_NOARGS,
     "choosespouse(void) --> INDI.  Select and return a spouse of individual\n\
through user interface.  Returns None if individual has no spouses or user cancels." },
   { "choosefam",	llpy_choosefam, METH_NOARGS,
     "choosefam(void) -> FAM; Selects and returns a family that INDI is in." },

   /* this was in set.c, but there is no documented way to add methods
      to a type after it has been created.  And, researching how
      Python does it and then looking at the documentation for the
      functions involved, it says that it is not safe...  Which I take
      to mean that it is subject to change without notice... And that
      there might be other caveats as well. */

   { "spouses",		llpy_spouses_i, METH_NOARGS,
     "spouses(void) --> SET.  Returns set of spouses of INDI." },

   { "top_node", (PyCFunction)_llpy_top_node, METH_NOARGS,
     "top_nodevoid) --> NODE.  Returns the top of the NODE tree associated with the RECORD." },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

static struct PyMethodDef Lifelines_Person_Functions[] =
  {
   { "descendantset",	(PyCFunction)llpy_descendantset, METH_VARARGS | METH_KEYWORDS,
     "descendantset(SET) --> SET.  Returns the set of descendants of the input set." },
   { "childset",	(PyCFunction)llpy_childset, METH_VARARGS | METH_KEYWORDS,
     "childset(SET) --> SET.  Returns the set of INDIs that are children of the input INDIs." },
   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_individual_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Individual",
   .tp_doc = "Lifelines GEDCOM Individual Record",
   .tp_basicsize = sizeof (LLINES_PY_RECORD),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_individual_dealloc,
   .tp_iter = llpy_individual_iter,
   .tp_hash = llines_record_hash,
   .tp_richcompare = llines_record_richcompare,
   .tp_methods = Lifelines_Person_Methods,
 };

void llpy_person_init (void)
{
  int status;

  status = PyModule_AddFunctions (Lifelines_Module, Lifelines_Person_Functions);
  if (status != 0)
    fprintf (stderr, "llpy_person_init: attempt to add functions returned %d\n", status);
}
