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

#include "python-to-c.h"
#include "types.h"
#include "person.h"

#define MAX_NAME_LENGTH	68 /* see comment in llrpt_fullname (interp/builtin.c) */

/* NOTE on the function header comments that follow:

   The name shown is the C name.  The Python name, unless noted
   otherwise, is the same but with the llpy_ stripped off the front.
   The argument lists shown are the Python argument lists.  */

/* llpy_name (INDI, [CAPS]) --> NAME:

   Returns the default name of a person -- the name found on the first
   '1 NAME' line.  The slashes are removed.  If CAPS (optional) is
   True (default), the surname is made all capitals.  */

PyObject *llpy_name (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  static char *keywords[] = { "caps", NULL };
  int caps = 1;
  NODE node_name = 0;
  STRING name = 0;
  
  if (! PyArg_ParseTupleAndKeywords (args, kw, "|p", keywords, &caps))
    /* XXX not reached - on failure, an exception is signalled by
       Python.  Do not currently know how to handle that.  XXX */
    return NULL;

  if (! (node_name = NAME (nztop (indi->lli_record))))
    {
      if (getlloptint("RequireNames", 0))
	/* XXX figure out how to issue exceptions XXX _("name: person does not have a name") */
	return NULL;
      Py_RETURN_NONE;
    }
  name = manip_name(nval(node_name), caps, REGORDER, MAX_NAME_LENGTH);
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

PyObject *llpy_fullname (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  static char *keywords[] = { "upcase", "keep_order", "max_length", NULL };
  int upcase = 0;
  int keep_order = 1;
  int max_length = 0;
  NODE node_name = 0;
  STRING name = 0;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|ppI", keywords, &upcase, &keep_order, &max_length))
    return NULL;

  if (! (node_name = NAME (nztop (indi->lli_record))) || ! nval(node_name))
    {
      if (getlloptint("RequireNames", 0))
	/* XXX figure out how to issue exceptions XXX _("fullname: person does not have a name") */
	return NULL;
      Py_RETURN_NONE;
    }
  if (max_length == 0)
    max_length = MAX_NAME_LENGTH;

  name = manip_name (nval (node_name), upcase, keep_order, max_length);
  return (Py_BuildValue ("s", name));
}

/* llpy_surname (INDI) --> STRING

   Returns the surname as found in the first '1 NAME' line.  Slashes
   are removed. */

PyObject *llpy_surname (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  NODE node_name = 0;
  STRING name = 0;

  node_name = nztop (indi->lli_record);
  if (! (node_name = NAME(node_name)) || ! nval(node_name))
    {
      if (getlloptint ("RequireNames", 0))
	/* XXX figure out how to issue exceptions XXX _("surname: person does not have a name" */
	return NULL;
      return None;		/* XXX should it be an empty string instead? */
    }
  name = getasurname(nval(node_name));
  return (Py_BuildValue ("s", name));
}

/* llpy_givens (INDI) --> STRING

   Returns the given names of the person in the same order and format
   as found in the first '1 NAME' line of the record.  */

PyObject *llpy_givens (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  NODE name = 0;

  if (!(name = NAME(nztop(indi->lli_record))) || !nval(name))
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

PyObject *llpy_trimname (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  static char *keywords[] = { "max_length", NULL };
  int max_length;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "I", keywords, &max_length))
    /* XXX not reached - on failure, an exception is signalled by
       Python.  Do not currently know how to handle that.  XXX */
    return NULL;
  abort ();
}

/* llpy_birth (INDI) --> EVENT

   Returns the first birth event of INDI; None if no event is
   found.  */

PyObject *llpy_birth (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  NODE indi = nztop (self->lli_record);
  NODE birth;

  
  abort ();
}

/* llpy_death (INDI) --> EVENT

   Returns the first death event of INDI; None if no event is
   found.  */

PyObject *llpy_death (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_burial (INDI) --> EVENT

   Returns the first burial event of INDI; None if no event is
   found.  */

PyObject *llpy_burial (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_father (INDI) --> INDI

   Returns the first father of INDI; None if no person in the
   role.  */

PyObject *llpy_father (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_mother (INDI) --> INDI

   Returns the first mother of INDI; None if no person in the
   role.  */

PyObject *llpy_mother (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_nextsib (INDI) --> INDI

   Returns the next (younger) sibling of INDI; None if no person in
   the role.  */

PyObject *llpy_nextsib (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_prevsib (INDI) --> INDI

   Returns the previous (older) sibling of INDI; None if no person in
   the role.  */

PyObject *llpy_prevsib (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_sex (INDI) --> STRING

   Returns the sex of INDI (M, F, or U).  */

PyObject *llpy_sex (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  NODE indi_node = nztop (indi->lli_record);
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

PyObject *llpy_male (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  NODE indi_node = nztop (indi->lli_record);
  BOOLEAN bool = (SEX(indi_node) == SEX_MALE);
  if (bool)
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

/* llpy_female (INDI) --> BOOLEAN

   Returns True if Female; False otherwise.  */

PyObject *llpy_female (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  NODE indi_node = nztop (indi->lli_record);
  BOOLEAN bool = (SEX(indi_node) == SEX_FEMALE);
  if (bool)
    Py_RETURN_TRUE;
  else
    Py_RETURN_FALSE;
}

/* llpy_pn */

PyObject *llpy_pn (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  static char *keywords[] = { "which", NULL };
  int which;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "i", keywords, &which))
    return NULL;

  abort ();
}

/* llpy_nspouses (INDI) --> INTEGER

   Returns the number of spouses of INDI.  */

PyObject *llpy_nspouses (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_nfamilies (INDI) --> INTEGER

   Returns the number of families (as spouse/parent) of INDI.  */

PyObject *llpy_nfamilies (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  RECORD indi_record = indi->lli_record;
  int count = length_nodes (FAMS (nztop (indi_record)));

  return (Py_BuildValue ("I", count));
}

/* llpy_parents */

PyObject *llpy_parents (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_title (INDI) --> STRING

   Returns the first '1 TITL' line in the record.  */

PyObject *llpy_title (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_key */

PyObject *llpy_key (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  static char *keywords[] = { "num_only", NULL };
  int num_only = 0;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|p", keywords, &num_only))
    return NULL;

  abort ();
}

/* llpy_soundex (INDI) --> STRING

   Returns the SOUNDEX code of INDI.  */

PyObject *llpy_soundex (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_root */

PyObject *llpy_root (PyObject *self, PyObject *args)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_nextindi (INDI) --> INDI

   Returns the next INDI in the database (in key order).  */

PyObject *llpy_nextindi (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

/* llpy_previndi (INDI) --> INDI

   Returns the previous INDI in the database (in key order).  */

PyObject *llpy_previndi (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_INDI_NODE *indi = (LLINES_PY_INDI_NODE *) self;
  abort ();
}

static struct PyMethodDef Lifelines_Person_Methods[] =
  {
   /* Person Functions */

   { "name",		(PyCFunction)llpy_name, METH_VARARGS | METH_KEYWORDS,
     "(INDI).name([CAPS]) -->NAME; returns the name found on the first '1 NAME' line.\n\
If CAPS (optional) is True (default), the surname is made all capitals." },
   { "fullname",	(PyCFunction)llpy_fullname, METH_VARARGS | METH_KEYWORDS,
     "(INDI).fullname(bool1, bool2, int) -->" },
   { "surname",		(PyCFunction)llpy_surname, METH_NOARGS,
     "(INDI).surname(void) --> STRING: returns the surname as found in the first '1 NAME'\n\
line.  Slashes aree removed." },
   { "givens",		(PyCFunction)llpy_givens, METH_NOARGS,
     "(INDI).givens(void) --> STRING: returns the given names of the person in the\n\
same order and format as found in the first '1 NAME' line of the record." },
   { "trimname",	(PyCFunction)llpy_trimname, METH_VARARGS | METH_KEYWORDS,
     "(INDI).trimname(MAX_LENGTH) --> STRING; returns name trimmed to MAX_LENGTH." },
   { "birth",		(PyCFunction)llpy_birth, METH_NOARGS,
     "(INDI).birth(void) -> EVENT: First birth event of INDI; None if no event is found." },
   { "death",		(PyCFunction)llpy_death, METH_NOARGS,
     "(INDI).death(void) -> EVENT: First death event of INDI; None if no event is found." },
   { "burial",		(PyCFunction)llpy_burial, METH_NOARGS,
     "(INDI).burial(void) -> EVENT: First burial event of INDI; None if no event is found." },
   { "father",		(PyCFunction)llpy_father, METH_NOARGS,
     "(INDI).father(void) -> INDI: First father of INDI; None if no person in the role." },
   { "mother",		(PyCFunction)llpy_mother, METH_NOARGS,
     "(INDI).mother(void) -> INDI: First mother of INDI; None if no person in the role." },
   { "nextsib",		(PyCFunction)llpy_nextsib, METH_NOARGS,
     "(INDI).nextsib(void) -> INDI: next (younger) sibling of INDI. None if no person in the role." },
   { "prevsib",		(PyCFunction)llpy_prevsib, METH_NOARGS,
     "(INDI).prevsib(void) -> INDI: previous (older) sibling of INDI. None if no person in the role." },
   { "sex",		(PyCFunction)llpy_sex, METH_NOARGS,
     "(INDI).sex(void) -> STRING: sex of INDI (M, F, or U)." },
   { "male",		(PyCFunction)llpy_male, METH_NOARGS,
     "(INDI).male(void) --> boolean: True if male, False otherwise." },
   { "female",		(PyCFunction)llpy_female, METH_NOARGS,
     "(INDI).female(void) --> boolean: True if female, False otherwise." },
   { "pn",		(PyCFunction)llpy_pn, METH_VARARGS | METH_KEYWORDS,
     "(INDI).pn(integer) --> STRING: pronoun referring to INDI\n\
\t\t0 (He/She), 1 (he/she), 2 (His/Her), 3 (his/her), 4 (him/her)" },
   { "nspouses",	(PyCFunction)llpy_nspouses, METH_NOARGS,
     "(INDI).nspouses(void) -> INTEGER; Returns number of spouses of INDI." },
   { "nfamilies",	(PyCFunction)llpy_nfamilies, METH_NOARGS,
     "(INDI).nfamilies(void) -> INTEGER; Returns number of families (as spouse/parent) of INDI" },
   { "parents",		llpy_parents, METH_VARARGS,
     "doc string" },
   { "title",		(PyCFunction)llpy_title, METH_NOARGS,
     "(INDI).title(void) -> STRING; Returns the value of the first '1 TITL' line in the record." },
   { "key",		(PyCFunction)llpy_key, METH_NOARGS | METH_KEYWORDS,
     "(INDI).key([num_only]) --> : key; Returns the internal key.\n\
If boolean NUM_ONLY is True (default: False), omit the leading letter." },
   { "soundex",		(PyCFunction)llpy_soundex, METH_NOARGS,
     "(INDI).soundex(void) -> STRING: SOUNDEX code of INDI" },

   /* User Interaction Functions */

   { "choosechild",	llpy_choosechild, METH_VARARGS,
     "choosechild(INDI|FAM) -> INDI; Selects and returns child of person|family\n\
through user interface." },
   { "choosefam",	llpy_choosefam, METH_VARARGS,
     "choosefam(INDI) -> FAM; Selects and returns a family that INDI is in." },
   { "choosespouse",	llpy_choosespouse, METH_VARARGS,
     "choosespouse(INDI) -> INDI; Select and return a spouse of INDI." },

   { NULL, 0, 0, NULL }		/* sentinel */
};

PyTypeObject llines_individual_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Individual",
   .tp_doc = "Lifelines GEDCOM Individual Record",
   .tp_basicsize = sizeof (LLINES_PY_INDI_NODE),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_methods = Lifelines_Person_Methods,
 };
