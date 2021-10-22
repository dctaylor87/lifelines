/* iter.c -- iter types */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC */
#include "uiprompts.h"		/* DOASK1 */
#include "gedcom.h"		/* RECORD */

#include "python-to-c.h"
#include "types.h"

/* forward references */

/* these functions return iterators for all individuals, families, and
   sources, respectively, in the database.  */

static PyObject *llpy_events (PyObject *self, PyObject *args);
static PyObject *llpy_individuals (PyObject *self, PyObject *args);
static PyObject *llpy_families (PyObject *self, PyObject *args);
static PyObject *llpy_sources (PyObject *self, PyObject *args);
static PyObject *llpy_others (PyObject *self, PyObject *args);

static int nodeiter_next_child (LLINES_PY_NODEITER *iter);
static int nodeiter_next_traverse (LLINES_PY_NODEITER *iter);

static void llpy_iter_dealloc (PyObject *self);
static PyObject *llpy_iter_iter (PyObject *self);
static PyObject *llpy_iter_iternext (PyObject *self);

/* start of code */

/* llpy_events (void) --> Returns an iterator for the set of
   events in the database.  */

static PyObject *llpy_events (PyObject *self ATTRIBUTE_UNUSED,
			      PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_events entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_EVEN;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_individuals (void) --> Returns an iterator for the set of
   individuals in the database.  */

static PyObject *llpy_individuals (PyObject *self ATTRIBUTE_UNUSED,
				   PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_individuals entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_INDI;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_families (void) --> Returns an iterator for the set of
   families in the database.  */

static PyObject *llpy_families (PyObject *self ATTRIBUTE_UNUSED,
				PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_families entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_FAM;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_sources (void) --> Returns an iterator for the set of sources
   in the database.  */

static PyObject *llpy_sources (PyObject *self ATTRIBUTE_UNUSED,
			       PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_sources entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_SOUR;
  iter->li_current = 0;

  return (PyObject *)iter;
}

/* llpy_others (void) --> Returns an iterator for the set of other records
   in the database.  */

static PyObject *llpy_others (PyObject *self ATTRIBUTE_UNUSED,
			       PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_ITER *iter = PyObject_New (LLINES_PY_ITER, &llines_iter_type);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_others entry: self %p args %p\n",
	       (void *)self, (void *)args);
    }

  if (! iter)
    return NULL;		/* PyObject_New failed and set exception */

  iter->li_type = LLINES_TYPE_OTHR;
  iter->li_current = 0;

  return (PyObject *)iter;
}

static void llpy_iter_dealloc (PyObject *self)
{
  PyTypeObject *tp = Py_TYPE (self);

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_iter_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
#if 0
  PyObject_Del (self);
  Py_DECREF (tp);
#endif
}

static PyObject *llpy_iter_iter (PyObject *self)
{
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_iter_iter entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  /* XXX do we need to do a Py_INCREF here? XXX */
  Py_INCREF (self);

  return (self);
}

static PyObject *llpy_iter_iternext (PyObject *self)
{
  LLINES_PY_ITER *iter = (LLINES_PY_ITER *)self;
  RECORD record;

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_iter_iternext entry: type %c current %d refcnt %ld\n",
	       iter->li_type, iter->li_current, Py_REFCNT (self));
    }

  if (iter->li_current == -1)
    {
      /* trying to use an iter that was previously exhausted, raise an
	 exception */
      PyErr_SetObject (PyExc_StopIteration, Py_None);
      return NULL;
    }
  switch (iter->li_type)
    {
    case LLINES_TYPE_EVEN:
    case LLINES_TYPE_FAM:
    case LLINES_TYPE_INDI:
    case LLINES_TYPE_OTHR:
    case LLINES_TYPE_SOUR:
      iter->li_current = xref_next (iter->li_type, iter->li_current);
      break;
    default:      
      Py_UNREACHABLE ();	/* internal error */
    }
  if (iter->li_current == 0)
    {
      iter->li_current = -1;
      return NULL;		/* iter is now exhausted */
    }

  record = keynum_to_record (iter->li_type, iter->li_current);

  switch (iter->li_type)
    {
    case LLINES_TYPE_EVEN:
      {
	LLINES_PY_RECORD *obj = PyObject_New (LLINES_PY_RECORD, &llines_event_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->llr_type = LLINES_TYPE_EVEN;
	obj->llr_record = record;

	return (PyObject *)obj;
      }
    case LLINES_TYPE_INDI:
      {
	LLINES_PY_RECORD *obj = PyObject_New (LLINES_PY_RECORD, &llines_individual_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->llr_type = LLINES_TYPE_INDI;
	obj->llr_record = record;

	return (PyObject *)obj;
      }
    case LLINES_TYPE_FAM:
      {
	LLINES_PY_RECORD *obj = PyObject_New (LLINES_PY_RECORD, &llines_family_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->llr_type = LLINES_TYPE_FAM;
	obj->llr_record = record;

	return (PyObject *)obj;
      }
    case LLINES_TYPE_SOUR:
      {
	LLINES_PY_RECORD *obj = PyObject_New (LLINES_PY_RECORD, &llines_source_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->llr_type = LLINES_TYPE_SOUR;
	obj->llr_record = record;

	return (PyObject *)obj;
      }
    case LLINES_TYPE_OTHR:
      {
	LLINES_PY_RECORD *obj = PyObject_New (LLINES_PY_RECORD, &llines_other_type);
	if (! obj)
	  {
	    release_record (record);
	    return NULL;
	  }
	obj->llr_type = LLINES_TYPE_OTHR;
	obj->llr_record = record;

	return (PyObject *)obj;
      }
    default:
      /* Something mighty weird is happening. Not only was there
	 an internal error for us to be called with an unexpected
	 type, but it was not caught by the previous switch
	 statment.  */
      Py_UNREACHABLE ();
    }
}

static void llpy_nodeiter_dealloc (PyObject *self)
{
  PyTypeObject *tp = Py_TYPE (self);
  LLINES_PY_NODEITER *nodeiter = (LLINES_PY_NODEITER *) self;

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_nodeiter_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  nrefcnt(nodeiter->ni_top_node)--;
  nodeiter->ni_top_node = 0;

  nrefcnt(nodeiter->ni_cur_node)--;
  nodeiter->ni_cur_node = 0;

  if (nodeiter->ni_tag)
    free (nodeiter->ni_tag);
  nodeiter->ni_tag = 0;
#if 0
  PyObject_Del (self);
  Py_DECREF (tp);
#endif
}

static PyObject *llpy_nodeiter_iter (PyObject *self)
{
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_iter_iter entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  /* XXX do we need to do a Py_INCREF here? XXX */
  Py_INCREF (self);

  return (self);
}

static PyObject *llpy_nodeiter_iternext (PyObject *self)
{
  LLINES_PY_NODEITER *iter = (LLINES_PY_NODEITER *) self;
  LLINES_PY_NODE *py_node;
  int retval;

  /* if this is ever stored or returned, something is screwed up */
  int new_level = -2;

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_nodeiter_iternext entry: type %c current %p refcnt %ld\n",
	       iter->ni_type, (void *)iter->ni_cur_node, Py_REFCNT (self));
    }

  while (1)
    {
      if (iter->ni_level < 0)
	{
	  /* trying to use an iter that was previously exhausted, raise an
	     exception */
	  PyErr_SetObject (PyExc_StopIteration, Py_None);
	  return NULL;
	}

      if (iter->ni_type == ITER_CHILDREN)
	retval = nodeiter_next_child (iter);
      else if (iter->ni_type == ITER_TRAVERSE)
	retval = nodeiter_next_traverse (iter);
      else
	{
	  PyErr_SetString (PyExc_SystemError, "nodeiter_iternext: bad value for iter type");
	  return NULL;
	}
      if (retval < 0)
	{
	  /* previously exhausted */
	  PyErr_SetObject (PyExc_StopIteration, Py_None);
	  return NULL;
	}
      else if (retval == 0)
	return NULL;
      else
	{
	  /* we found a node, are we done? */
	  if ((iter->ni_tag == 0) || eqstr (ntag(iter->ni_cur_node), iter->ni_tag))
	    {
	      /* we are done */
	      py_node = PyObject_New (LLINES_PY_NODE, &llines_node_type);
	      if (! py_node)
		return NULL;
	      py_node->lnn_type = 0;
	      py_node->lnn_node = iter->ni_cur_node;
	      nrefcnt(py_node->lnn_node)++;
	      return (PyObject *) py_node;
	    }
	}
    }
}

/* nodeiter_next_child -- helper function for llpy_nodeiter_iternext
   for the case of CHILD iteration, determine the next NODE to return.

   This function is ONLY concerned with returning the next node, not
   with the tag constraint, if any.

   return value: 1 -- normal, iter has NODE to return
                 0 -- iterator exhausted
		-1 -- iterator previously exhausted */

static int nodeiter_next_child (LLINES_PY_NODEITER *iter)
{
  if (iter->ni_level < 0)
    {
      PyErr_SetObject (PyExc_StopIteration, Py_None);
      return (-1);
    }

  if (! iter->ni_cur_node)
    {
      /* first iteration */
      iter->ni_cur_node = nchild (iter->ni_top_node);

      if (iter->ni_cur_node)
	{
	  nrefcnt(iter->ni_cur_node)++;
	  return (1);		/* we have a node */
	}
      else
	{
	  iter->ni_level = -1;	/* mark that it is exhausted */
	  return (0);		/* exhausted -- no children */
	}
    }

  /* not the first call */
  if (! iter->ni_cur_node)
    {
      /* paranoia -- should not happen */
      PyErr_SetString (PyExc_SystemError, "nodeiter_next_child: inconsistent iter");
      iter->ni_level = -2;
      return (-1);
    }
  nrefcnt(iter->ni_cur_node)--;
  iter->ni_cur_node = nsibling (iter->ni_cur_node);
  if (iter->ni_cur_node)
    {
      nrefcnt(iter->ni_cur_node)++;
      return (1);		/* we have a node */
    }

  iter->ni_level = -1;		/* mark that it is exhausted */
  return (0);			/* exhausted -- no more children */
}

/* nodeiter_next_traverse -- helper function for llpy_nodeiter_iternext
   for the case of TRAVERSE iteration, determine the next NODE to
   return.

   This function is ONLY concerned with returning the next node,, not
   with the tag constraint, if any.

   return value: 1 -- normal, iter has NODE to return
                 0 -- iterator exhausted
		-1 -- iterator previously exhausted */

static int nodeiter_next_traverse (LLINES_PY_NODEITER *iter)
{
  NODE new_node;
  int new_level;
  NODE cur_node;

  if (iter->ni_level < 0)
    {
      PyErr_SetObject (PyExc_StopIteration, Py_None);
      return (-1);
    }

  if (! iter->ni_cur_node)
    {
      /* first iteration */
      iter->ni_cur_node = iter->ni_top_node;
      nrefcnt(iter->ni_cur_node)++;
      iter->ni_level = 0;
      return (1);		/* we have a node -- the top node */
    }

  /* not the first call */
  new_node = nchild (iter->ni_cur_node);
  new_level = iter->ni_level + 1;

  if (new_node)
    {
      nrefcnt(iter->ni_cur_node)--;
      iter->ni_cur_node = new_node;
      nrefcnt(iter->ni_cur_node)++;
      iter->ni_level = new_level;
      return (1);
    }

  /* no more desendants, try a sibling */
  new_node = nsibling (iter->ni_cur_node);
  new_level--;

  if (new_node)
    {
      nrefcnt(iter->ni_cur_node)--;
      iter->ni_cur_node = new_node;
      nrefcnt(iter->ni_cur_node)++;
      iter->ni_level = new_level;
      return (1);
    }
  /* nothing below, nothing to the size, so go up */

  /* start of loop */

  cur_node = iter->ni_cur_node;

  while (1)
    {
      cur_node = nparent (cur_node);
      new_level--;

      /* pararnoia -- these conditions should either both be true or
	 both be false */
      if ((cur_node == iter->ni_top_node) ||
	  (new_level <= 0))
	{
	  /* got back to the top so we are exhausted */
	  iter->ni_level = -1;
	  return (0);		/* newly exhausted */
	}
      new_node = nsibling (cur_node);
      if (new_node)
	{
	  /* found one, return it */
	  nrefcnt(iter->ni_cur_node)--;
	  iter->ni_cur_node = new_node;
	  nrefcnt(iter->ni_cur_node)++;
	  iter->ni_level = new_level;
	  return (1);
	}
    }
}

static struct PyMethodDef Lifelines_Iter_Methods[] =
  {
   /* RECORD functions */

   { "individuals",	llpy_individuals, METH_NOARGS,
     "individuals(void) -> iterator for the set of all INDI in the database" },
   { "families",	llpy_families, METH_NOARGS,
     "families(void) -> iterator for the set of all FAM in the database" },
   { "sources",		llpy_sources, METH_NOARGS,
     "sources(void) -> iterator for the set of all SOUR in the database" },
   { "events",		llpy_events, METH_NOARGS,
     "events(void) --> iterator for the set of all EVEN in the database" },
   { "others",		llpy_others, METH_NOARGS,
     "others(void) --> iterator for the set of all OTHR records in the database" },

   /* NODE functions */

   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_iter_type =
  {
   PyVarObject_HEAD_INIT (NULL, 0)
   .tp_name = "llines.Iter",
   .tp_doc = "Lifelines GEDCOM Iteration Type",
   .tp_basicsize = sizeof (LLINES_PY_ITER),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_iter_dealloc,
   .tp_methods = NULL,
   .tp_iter = llpy_iter_iter,
   .tp_iternext = llpy_iter_iternext,
  };

PyTypeObject llines_nodeiter_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.NodeIter",
   .tp_doc = "Lifelines GEDCOM NODE Iteration Type",
   .tp_basicsize = sizeof (LLINES_PY_NODEITER),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_nodeiter_dealloc,
   .tp_methods = NULL,
   .tp_iter = llpy_nodeiter_iter,
   .tp_iternext = llpy_nodeiter_iternext,
  };

void llpy_iter_init (void)
{
  int status;

  status = PyModule_AddFunctions (Lifelines_Module, Lifelines_Iter_Methods);

  if (status != 0)
    fprintf (stderr, "llpy_iter_init: attempt to add functions returned %d\n", status);

  status = PyModule_AddIntMacro (Lifelines_Module, ITER_CHILDREN);

  if (status != 0)
    fprintf (stderr, "llpy_iter_init: attempt to add macro ITER_CHILDREN returned %d\n", status);

  status = PyModule_AddIntMacro (Lifelines_Module, ITER_TRAVERSE);

  if (status != 0)
    fprintf (stderr, "llpy_iter_init: attempt to add macro ITER_TRAVERSE returned %d\n", status);
}
