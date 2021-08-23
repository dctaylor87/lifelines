/* user.c -- user interaction */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
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


/* llpy_getindi (PROMPT) --> INDI: identify person through user interface */

PyObject *llpy_getindi (PyObject *self, PyObject *args, PyObject *kw)
{
  STRING prompt = _("Identify person for program:");
  RECORD rec;
  static char *keywords[] = { "prompt", NULL };
  LLINES_PY_INDI_RECORD *indi;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|s", keywords, &prompt))
    /* XXX actually not reached -- on failure an exception is
       signalled by Python.  Do not currently know how to handle that.
       XXX */
    return NULL;

  rec = rptui_ask_for_indi (prompt, DOASK1);

  indi = PyObject_New(LLINES_PY_INDI_RECORD, &llines_individual_type);
  indi->lri_type = LLINES_TYPE_INDI;
  indi->lri_record = rec;
  return (PyObject *)indi;
}

PyObject *llpy_getfam (PyObject *self, PyObject *args)
{
  abort ();
}

PyObject *llpy_getint (PyObject *self, PyObject *args, PyObject *kw)
{
  STRING prompt = _("Enter integer for program");
  static char *keywords[] = { "prompt", NULL };
  INT num;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|s", keywords, &prompt))
    return NULL;

  if (! rptui_ask_for_int (prompt, &num))
    {
      /* XXX figure out how to raise an exception XXX */
      return NULL;
    }
  return Py_BuildValue ("i", num);
}

PyObject *llpy_getstr (PyObject *self, PyObject *args, PyObject *kw)
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

PyObject *llpy_menuchoose (PyObject *self, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "choices", "prompt", NULL };
  abort ();
}

