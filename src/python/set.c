/* set.c -- SET functions.

   These are the bulk of the functions that take and/or return a SET.

   While these functions return a SET, most (all?) allow any input
   that is iterable. */


#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC (needed by gedcom.h) */
#include "gedcom.h"		/* RECORD */

#include "llpy-externs.h"

#include "python-to-c.h"
#include "types.h"

/* forward references */

static PyObject *llpy_siblingset (PyObject *self, PyObject *args, PyObject *kw);

/* start of code */

static PyObject *llpy_siblingset (PyObject *self, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "set", "include_self", NULL };
  PyObject *set;	/* collection of input INDIs */
  int include_self = 1;	/* whether to include input iNDIs in output */
  PyObject *iterator;
  PyObject *py_indi;
  PyObject *output;
  int status;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "O|p", keywords, &set, &include_self))
    return NULL;

  iterator = PyObject_GetIter (set);
  if (! iterator)
    return NULL;		/* non-iterable argument supplied */

  output = PySet_New (NULL);	/* output is an empty set */
  if (! output)
    {
      /* unable to create a new set, clean up and propagate the error */
      Py_DECREF (iterator);
      return NULL;
    }

  while (py_indi = PyIter_Next (iterator))
    {
      RECORD indi;
      NODE node;

      if (py_indi->ob_type != &llines_individual_type)
	{
	  /* XXX set an exception -- wrong type (non INDI) was found
	     input set.  Return NULL after cleaning up.  XXX */
	  return NULL;
	}
      indi = ((LLINES_PY_INDI_RECORD *)py_indi)->lri_record;
      node = FAMC (nztop (indi));
      if (! node)
	{
	  /* Not in a family as a child, hence no known siblings,
	     add indi to output set if include_self is True.

	     And, regardless, continue to the next element of the
	     input set */
	  if (include_self)
	    {
	      status = PySet_Add (output, py_indi);
	      if (status < 0)
		{
		  /* error -- clean up and propagate error */
		  Py_DECREF (iterator);
		  PySet_Clear (output);
		  Py_DECREF (output);

		  return NULL;
		}
	    }
	  continue;
	}
      else
	{
	  node = qkey_to_fam (rmvat(nval(node)));
	  for (NODE child = CHIL (node); child; child = nsibling(child))
	    {
	      LLINES_PY_INDI_RECORD *new_indi;
	      CNSTRING key = rmvat (nval (child));
	      INT keynum = atoi(key);

	      if (include_self || (keynum != nzkeynum(indi)))
		{
		  RECORD sibling = key_to_irecord (key);

		  new_indi = PyObject_New (LLINES_PY_INDI_RECORD, &llines_individual_type);
		  if (! new_indi)
		    {
		      /* clean up and return */
		      release_record (sibling);

		      Py_DECREF (iterator);
		      PySet_Clear (output);
		      Py_DECREF (output);

		      return NULL;
		    }
		  new_indi->lri_type = LLINES_TYPE_INDI;
		  new_indi->lri_record = sibling;
		  status = PySet_Add (output, (PyObject *)new_indi);
		  if (status < 0)
		    {
		      /* clean up and return */
		      Py_DECREF ((PyObject *)new_indi);

		      Py_DECREF (iterator);
		      PySet_Clear (output);
		      Py_DECREF (output);

		      return NULL;
		    }
		}
	    }
	}
    }
  if (PyErr_Occurred())
    {
      /* iteration terminated due to error -- clean up and return */
      Py_DECREF (iterator);
      PySet_Clear (output);
      Py_DECREF (output);

      return NULL;
    }
  /* no errors and we are done! */
  return (output);
}

static struct PyMethodDef Lifelines_Set_Methods[] =
  {
   { "siblingset", (PyCFunction)llpy_siblingset, METH_VARARGS | METH_KEYWORDS,
     "siblingset(set, [include_self]) --> SET of INDIs.\n\
Returns the set of all siblings of the INDIs in the input SET.\n\
If INCLUDE_SELF is True (default: True), then the input INDIs are included." },
   { NULL, 0, 0, NULL }		/* sentinel */
  };

void llpy_set_init (void)
{
  int status;

  status = PyModule_AddFunctions (Lifelines_Module, Lifelines_Set_Methods);

  if (status != 0)
    fprintf (stderr, "llpy_set_init: attempt to add functions returns %d\n", status);
}
