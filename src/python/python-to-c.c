/* python-to-c.c -- this module provides the glue for Python code to call into Lifelines C routines */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "python-to-c.h"
#include "llpy-externs.h"

#include "llstdlib.h"
#include "gedcom.h"

#include "database.h"
#include "family.h"
#include "person.h"

static struct PyMethodDef LifelinesMethods[] =
  {
   /* Person Functions -- moved to Lifelines_Person_Methods */

   { "parents",		llpy_parents, METH_VARARGS,
     "doc string" },
   { "key",		(PyCFunction)llpy_key, METH_VARARGS | METH_KEYWORDS,
     "key(INDI) --> : internal key" },
   { "inode",		llpy_inode, METH_VARARGS,
     "Root GEDCOM node of INDI" },
   { "firstindi",	(PyCFunction)llpy_firstindi, METH_NOARGS,
     "firstindi(void) -> INDI: First person in database in key order" },
   { "lastindi",	(PyCFunction)llpy_lastindi, METH_NOARGS,
     "lastindi(void) -> INDI: Last person in database in key order" },
   { "nextindi",	(PyCFunction)llpy_nextindi, METH_VARARGS | METH_KEYWORDS,
     "nextindi(INDI) -> INDI: next person in database after INDI (in key order)" },
   { "previndi",	(PyCFunction)llpy_previndi, METH_VARARGS | METH_KEYWORDS,
     "previndi(INDI) -> INDI: previous person in database after INDI (in key order)" },
   { "spouseset",	llpy_spouseset, METH_VARARGS,
     "doc string" },

   /* Family Functions -- moved to Lifelines_Family_Methods */

   { "fnode",		llpy_fnode, METH_VARARGS,
     "doc string" },
   { "root",		llpy_root, METH_VARARGS,
     "doc string" },
   { "fam",		llpy_fam, METH_VARARGS,
     "doc string" },

   /* GEDCOM Node Functions */

   { "xref",		llpy_xref, METH_VARARGS,
     "doc string" },
   { "tag",		llpy_tag, METH_VARARGS,
     "doc string" },
   { "value",		llpy_value, METH_VARARGS,
     "doc string" },
   { "parent",		llpy_parent, METH_VARARGS,
     "doc string" },
   { "child",		llpy_child, METH_VARARGS,
     "doc string" },
   { "sibling",		llpy_sibling, METH_VARARGS,
     "doc string" },
   { "savenode",	llpy_savenode, METH_VARARGS,
     "doc string" },
   { "level",		llpy_level, METH_VARARGS,
     "doc string" },

   /* Event and Date Functions */

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

   /* Value Extraction Functions */

   { "extractdate",	llpy_extractdate, METH_VARARGS,
     "doc string" },
   { "extractnames",	llpy_extractnames, METH_VARARGS,
     "doc string" },
   { "extractplaces",	llpy_extractplaces, METH_VARARGS,
     "doc string" },
   { "extracttokens",	llpy_extracttokens, METH_VARARGS,
     "doc string" },
   { "extractdatestr",	llpy_extractdatestr, METH_VARARGS,
     "doc string" },

   /* User Interaction Functions */

   { "getindi",		(PyCFunction)llpy_getindi, METH_VARARGS | METH_KEYWORDS,
     "getindi(PROMPT) -> INDI: identify person through user interface" },
   { "getindiset",	llpy_getindiset, METH_VARARGS,
     "identify set of persons through user interface" },
   { "getfam",		llpy_getfam, METH_VARARGS,
     "getfam(PROMPT) -> FAM: identify family through user interface" },
   { "getint",		llpy_getint, METH_VARARGS,
     "doc string" },
   { "getstr",		llpy_getstr, METH_VARARGS,
     "doc string" },
   { "choosechild",	llpy_choosechild, METH_VARARGS,
     "choosechild(INDI|FAM) -> INDI; Selects and returns child of person|family\n\
through user interface." },
   { "choosefam",	llpy_choosefam, METH_VARARGS,
     "choosefam(INDI) -> FAM; Selects and returns a family that INDI is in." },
   { "chooseindi",	llpy_chooseindi, METH_VARARGS,
     "doc string" },
   { "choosespouse",	llpy_choosespouse, METH_VARARGS,
     "choosespouse(INDI) -> INDI; Select and return a spouse of INDI." },
   { "choosesubset",	llpy_choosesubset, METH_VARARGS,
     "doc string" },
   { "menuchoose",	llpy_menuchoose, METH_VARARGS,
     "doc string" },

   /* Person Set Functions and GEDCOM Extraction */

   { "indiset",		llpy_indiset, METH_VARARGS,
     "doc string" },
   { "parentset",	llpy_parentset, METH_VARARGS,
     "doc string" },
   { "childset",	llpy_childset, METH_VARARGS,
     "doc string" },
   { "spouseset",	llpy_spouseset, METH_VARARGS,
     "doc string" },
   { "siblingset",	llpy_siblingset, METH_VARARGS,
     "doc string" },
   { "ancestorset",	llpy_ancestorset, METH_VARARGS,
     "doc string" },
   { "descendantset",	llpy_descendentset, METH_VARARGS,
     "doc string" },
   { "descendentset",	llpy_descendentset, METH_VARARGS,
     "doc string" },
   { "getindiset",	llpy_getindiset, METH_VARARGS,
     "doc string" },
   { "gengedcomstrong",	llpy_gengedcomstrong, METH_VARARGS,
     "doc string" },

   /* Miscellaneous Functions */

   { "program",		llpy_program, METH_NOARGS,
     "program(void) -> STRING: return name of the current program" },
   { "version",		llpy_version, METH_NOARGS,
     "version(void) -> STRING: return version of current program" },
   { "heapused",	llpy_heapused, METH_NOARGS,
     "doc string" },

   { NULL, 0, 0, NULL }		/* sentinel */
};

static struct PyMethodDef Lifelines_Source_Methods[] =
  {

   { NULL, 0, 0, NULL }		/* sentinel */
  };

static struct PyMethodDef Lifelines_Other_Methods[] =
  {

   { NULL, 0, 0, NULL }		/* sentinel */
  };

static PyModuleDef LifelinesModule =
  {
   PyModuleDef_HEAD_INIT,
   .m_name = "llines",
   .m_doc = "Lifelines extension module",
   .m_size = -1,		/* XXX for now -- might want to revisit this XXX */
   .m_methods = LifelinesMethods,
  };

static PyObject *
PyInit_llines (void)
{
  return PyModule_Create (&LifelinesModule);
}

/* XXX when Python support is compiled in, this routine must be called
   during the llines initialization phase. XXX */

void llpy_init (void)
{
  PyImport_AppendInittab ("llines", &PyInit_llines);
}
