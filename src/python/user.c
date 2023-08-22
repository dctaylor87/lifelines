/* user.c -- user interaction */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "uiprompts.h"		/* DOASK1 */
#include "gedcom.h"		/* RECORD */
#include "indiseq.h"		/* INDISEQ (needed by rptui.h) */
#include "feedback.h"
#include "messages.h"
#include "../interp/rptui.h"

#include "python-to-c.h"
#include "types.h"

static PyObject *llpy_getindi (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_getfam (PyObject *self, PyObject *args);
static PyObject *llpy_getint (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_getstr (PyObject *self, PyObject *args, PyObject *kw);
static PyObject *llpy_menuchoose (PyObject *self, PyObject *args, PyObject *kw);

/* llpy_getindi (PROMPT) --> INDI: identify person through user interface */

static PyObject *llpy_getindi (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  STRING prompt = _("Identify person for program:");
  RECORD rec;
  static char *keywords[] = { "prompt", NULL };
  LLINES_PY_RECORD *indi;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|s", keywords, &prompt))
    return NULL;

  rec = rptui_ask_for_indi (prompt, DOASK1);

  indi = PyObject_New(LLINES_PY_RECORD, &llines_individual_type);
  indi->llr_type = LLINES_TYPE_INDI;
  indi->llr_record = rec;
  return (PyObject *)indi;
}

/* llpy_getfam (void) --> FAM; Identify family through user interface */

static PyObject *llpy_getfam (PyObject *self ATTRIBUTE_UNUSED, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_RECORD *family;
  RECORD record;

  record = rptui_ask_for_fam(_("Enter a spouse from family."),
			     _("Enter a sibling from family."));
  if (! record)
    Py_RETURN_NONE;		/* user cancelled */

  family = PyObject_New (LLINES_PY_RECORD, &llines_family_type);
  if (! family)
    return NULL;		/* PyObject_New failed and set the exception */

  family->llr_record = record;
  family->llr_type = LLINES_TYPE_FAM;
  return (PyObject *)family;
}

/* llpy_getint ([prompt]) --> INT; Get integer through user interface. */

static PyObject *llpy_getint (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  STRING prompt = _("Enter integer for program");
  static char *keywords[] = { "prompt", NULL };
  INT num;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|s", keywords, &prompt))
    return NULL;

  if (! rptui_ask_for_int (prompt, &num))
    {
      Py_RETURN_NONE;		/* user cancelled */
    }
  return Py_BuildValue ("i", num);
}

/* llpy_getstr ([prompt]) --> STRING; Get string through user interface. */

static PyObject *llpy_getstr (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "prompt", NULL };
  STRING prompt = _(qSchoostrttl);
  char buffer[MAXPATHLEN];

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|s", keywords, &prompt))
    return NULL;

  if (! ask_for_string (prompt, _(qSaskstr), buffer, sizeof(buffer)))
    /* cancelled -- return empty string */
    buffer[0] = 0;

  return Py_BuildValue ("s", buffer);
}

/* llpy_getindiset ([prompt]) --> SET */

static PyObject *llpy_getindiset (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "prompt", NULL };
  STRING prompt = _("Identify list of persons for program:");
  INDISEQ seq = 0;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|s", keywords, &prompt))
    return NULL;

  seq = rptui_ask_for_indi_list (prompt, TRUE);
  abort ();			/* XXX */
}

/* llpy_menuchoose (choices, [prompt]) --> INT; Select from a collection of choices. */

static PyObject *llpy_menuchoose (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "choices", "prompt", NULL };
  abort ();
}

static struct PyMethodDef Lifelines_User_Functions[] =
  {
   { "getindi",		(PyCFunction)llpy_getindi, METH_VARARGS | METH_KEYWORDS,
     "getindi([prompt]) --> INDI; Identify person through user interface." },
   { "getfam",		(PyCFunction)llpy_getfam, METH_NOARGS,
     "getfam(void) --> FAM; Identify family through user interface." },
   { "getint",		(PyCFunction)llpy_getint, METH_VARARGS | METH_KEYWORDS,
     "getint([prompt]) --> INT; Get integer through user interface." },
   { "getstr",		(PyCFunction)llpy_getstr, METH_VARARGS | METH_KEYWORDS,
     "getstr([prompt]) --> STRING; Get string through user interface." },
#if 0
   { "menuchoose",	(PyCFunction)llpy_menuchoose, METH_VARARGS | METH_KEYWORDS,
     "menuchoose(choices,[prompt]) --> INTEGER; Select from a collection of choices." },
#endif
   { NULL, 0, 0, NULL }		/* sentinel */
  };

void llpy_user_init (void)
{
  int status;

  status = PyModule_AddFunctions (Lifelines_Module, Lifelines_User_Functions);
  if (status != 0)
    fprintf (stderr, "llpy_user_init: attempt to add functions returned %d\n", status);
}
