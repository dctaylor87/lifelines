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

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_nodeiter_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
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

#if 0				/* XXX */
static PyObject *llpy_nodeiter_iternext (PyObject *self)
{
  LLINES_PY_NODEITER *iter = (LLINES_PY_NODEITER *) self;
  NODE next_node = 0;

  /* if this is ever stored or returned, something is screwed up */
  int new_level = -2;

  if (llpy_debug)
    {
      fprintf (stderr, "llpy_nodeiter_iternext entry: type %c current %p refcnt %ld\n",
	       iter->ni_type, (void *)iter->ni_cur_node, Py_REFCNT (self));
    }

  if (iter->ni_level < 0)
    {
      /* trying to use an iter that was previously exhausted, raise an
	 exception */
      PyErr_SetObject (PyExc_StopIteration, Py_None);
      return NULL;
    }

  /* is it the first iteration?  Or have we been here beforfe? */
  if (! iter->ni_cur_node)
    {
      /* first time for this iterator */
      if (iter->ni_type == NODE_ITER_CHILDREN)
	{
	  /* simple case -- we only iterate over immediate children */
	  new_level = 1;
	  if (! iter->ni_tag)
	    /* no tag restriction */
	    next_node = nchild (iter->ni_top_node);
	  else
	    /* tag restriction present */
	    next_node = TAG (iter->ni_top_node, iter->ni_tag);
	  if (! next_node)
	    {
	      iter->ni_level = -1;
	      return NULL;	/* iter exhausted */
	    }
	}
      else if (iter->ni_type == NODE_ITER_TRAVERSE)
	{
	  /* traverse does parent before child */
	  if (! iter->ni_tag)
	    {
	      /* no tag restriction */
	      new_level = 0;
	      next_node = iter->ni_top_node;
	    }
	  else
	    {
	      /* XXX tag restriction present XXX */
	      abort ();
	    }
	}
      else
	{
	  /* bad value for ni_type -- should never happen */
	  PyErr_SetString (PyExc_TypeError, "llpy_nodeiter_iternext: iterator has bad type");
	  return NULL;
	}
      LLINES_PY_NODE *py_node = PyObject_New (LLINES_PY_NODE, &llines_node_type);
      if (! py_node)
	return NULL;

      iter->ni_cur_node = next_node;
      py_node->lnn_type = 0;
      py_node->lnn_node = next_node;
      return ((PyObject *) py_node);
    }
  /* this is not the first iteration */
  abort ();
  if (iter->ni_type == NODE_ITER_CHILDREN)
    {
      /* simple case -- we only iterate over immediate children */
      new_level = 1;
      if (! iter->ni_tag)
	/* no tag restriction */
	next_node = nchild (iter->ni_cur_node);
      else
	/* tag restrictions present */
	next_node = NEXT_TAG (iter->ni_cur_node, iter->ni_tag);
      if (! next_node)
	{
	  iter->ni_level = -1;
	  return NULL;		/* iter exhausted */
	}
    }
  else if (iter->ni_type == NODE_ITER_TRAVERSE)
    {
      if (! iter->ni_tag)
	{
	  /* no tag restriction */
	  next_node = iter->ni_cur_node;
	  for (cur_node = iter->ni_cur_node, next_node = nchild(cur_node);
	       ! next_node;
	       ???)
	    {
	      next_node = nchild (cur_node);
	      new_level++;

	      if (next_node)
		break;	      /* found one */

	      /* can't go down any further, do we have a sibling? */
	      new_level--;
	      next_node = nsibling(cur_node);
	      if (next_node)
		break;		/* found one */

	      /* no children, no more siblings, go up a level */
	      
	    }
	}
      else
	{
	  /* XXX tag restriction present XXX */
	  abort ();
	}
    }
}
#endif

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
#if 0
   .tp_iternext = llpy_nodeiter_iternext,
#endif
  };

void llpy_iter_init (void)
{
  int status;

  status = PyModule_AddFunctions (Lifelines_Module, Lifelines_Iter_Methods);

  if (status != 0)
    fprintf (stderr, "llpy_iter_init: attempt to add functions returns %d\n", status);
}
