/* user.c -- user interaction */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "uiprompts.h"		/* DOASK1 */
#include "gedcom.h"		/* RECORD */
#include "indiseq.h"		/* INDISEQ */
#include "../interp/rptui.h"
#include "python-to-c.h"

/* llpy_getindi (PROMPT) --> INDI: identify person through user interface */

PyObject *llpy_getindi (PyObject *self, PyObject *args, PyObject *kw)
{
  STRING prompt = _("Identify person for program:");
  STRING key = 0;
  static char *keywords[] = { "prompt", NULL };

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|s", keywords, &prompt))
    /* XXX actually not reached -- on failure an exception is
       signalled by Python.  Do not currently know how to handle that.
       XXX */
    return NULL;

  key = rptui_ask_for_indi_key(prompt, DOASK1);

  return Py_BuildValue ("s", key);
}
