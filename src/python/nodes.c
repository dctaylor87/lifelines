#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "standard.h"		/* STRING */
#include "llstdlib.h"
#include "gedcom.h"
#include "../interp/interpi.h"	/* XXX */
#include "types.h"
#include "python-to-c.h"

/* forward references */

static PyObject *llpy_xref (PyObject *self, PyObject *args);
static PyObject *llpy_tag (PyObject *self, PyObject *args);
static PyObject *llpy_value (PyObject *self, PyObject *args);
static PyObject *llpy_parent_node (PyObject *self, PyObject *args);
static PyObject *llpy_child_node (PyObject *self, PyObject *args);
static PyObject *llpy_sibling_node (PyObject *self, PyObject *args);
static PyObject *llpy_copy_node_tree (PyObject *self, PyObject *args);
static PyObject *llpy_level (PyObject *self, PyObject *args);
static PyObject *llpy_nodeiter (PyObject *self, PyObject *args, PyObject *kw);

/* start of code */

static PyObject *llpy_xref (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;

  return Py_BuildValue ("s", nxref (node->lnn_node));
}

static PyObject *llpy_tag (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;

  return Py_BuildValue ("s", ntag (node->lnn_node));
}

static PyObject *llpy_value (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{

  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  STRING str = nval (node->lnn_node);

  if (! str)
    Py_RETURN_NONE;		/* node has no value -- probably has subnodes */

  return Py_BuildValue ("s", str);
}

static PyObject *llpy_parent_node (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  LLINES_PY_NODE *parent;
  NODE pnode = nparent (node->lnn_node);

  if (! pnode)
    Py_RETURN_NONE;		/* No parent -- already top of the tree */

  parent = PyObject_New (LLINES_PY_NODE, &llines_node_type);
  if (! parent)
    return NULL;		/* PyOBject_New failed and set an exception */

  parent->lnn_type = node->lnn_type; /* 'inherit' type from previous node */
  parent->lnn_node = pnode;

  return (PyObject *)parent;
}

static PyObject *llpy_child_node (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *py_node = (LLINES_PY_NODE *) self;
  NODE node = py_node->lnn_node;
  int type;

  node = nchild (node);
  if (! node)
    Py_RETURN_NONE;		/* node has no children */

  type = py_node->lnn_type;	/* save old type -- we are about to reuse py_node */
  py_node = PyObject_New (LLINES_PY_NODE, &llines_node_type);
  if (! py_node)
    return NULL;

  py_node->lnn_type = type;
  py_node->lnn_node = node;

  return (PyObject *)py_node;
}

static PyObject *llpy_sibling_node (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *py_node = (LLINES_PY_NODE *) self;
  NODE node = py_node->lnn_node;
  int type;

  node = nsibling (node);
  if (! node)
    Py_RETURN_NONE;		/* node has no siblings */

  type = py_node->lnn_type;	/* save old type -- we are about to reuse py_node */
  py_node = PyObject_New (LLINES_PY_NODE, &llines_node_type);
  if (! py_node)
    return NULL;

  py_node->lnn_type = type;
  py_node->lnn_node = node;

  return (PyObject *)py_node;
}

static PyObject *llpy_copy_node_tree (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  LLINES_PY_NODE *copy = PyObject_New (LLINES_PY_NODE, &llines_node_type);

  if (! copy)
    return NULL;

  copy->lnn_type = node->lnn_type;
  copy->lnn_node = copy_nodes (node->lnn_node, TRUE, TRUE);

  return (PyObject *)copy;
}

static PyObject *llpy_level (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  INT level = -1;

  for (NODE llnode = node->lnn_node; llnode; llnode = nparent (llnode))
    level++;

  return Py_BuildValue ("i", level);
}

/* llpy_add_node([parent], [prev] -- insert current node into GEDCOM
   tree with specified parent and previous sibling. */

static PyObject *llpy_add_node (PyObject *self, PyObject *args, PyObject *kw)
{
  LLINES_PY_NODE *orig = (LLINES_PY_NODE *) self;
  NODE orig_node = orig->lnn_node;
  static char *keywords[] = { "parent", "prev", NULL };
  PyObject *parent= Py_None;
  PyObject *prev = Py_None;
  NODE parent_node = NULL;
  NODE prev_node = NULL;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "|OO", keywords, &parent, &prev))
    return NULL;

  if (parent == Py_None)
    /* either we will become the top-node or we'll get our parent later... */
    parent_node = NULL;
  else if (Py_TYPE(parent) == &llines_node_type)
    parent_node = ((LLINES_PY_NODE *) parent)->lnn_node;
  else
    {
      /* error -- wrong type -- set exception and return NULL */
      PyErr_SetString (PyExc_TypeError, "add_node: parent must be a NODE or None");
      return NULL;
    }
  if (prev == Py_None)
    /* either we are first in line or our preceding sibling will be added later... */
    prev_node = NULL;
  else if (Py_TYPE(prev) == &llines_node_type)
    prev_node = ((LLINES_PY_NODE *) prev)->lnn_node;
  else
    {
      /* error -- wrong type -- set exception and return NULL */
      PyErr_SetString (PyExc_TypeError, "add_node: prev must be a NODE or None");
      return NULL;
    }

  if (prev_node && (nparent(prev_node) != parent_node))
    {
      /* prev_node has a different parent! -- set exception and return
	 NULL */
      PyErr_SetString (PyExc_ValueError, "add_node: prev is not a child of parent");
      return NULL;
    }

  /* XXX the following lines bracketed by dolock_node_in_cache need to
     be double and triple checked XXX */

  dolock_node_in_cache (orig_node, FALSE);
  nparent (orig_node) = parent_node;
  if (parent_node)
    orig_node->n_cel = parent_node->n_cel;
  else
    orig_node->n_cel = NULL;	/* XXX double check this! XXX */

  set_temp_node (orig_node, is_temp_node (parent_node));
  dolock_node_in_cache (orig_node, TRUE);

  /* if both prev_node and parent_node are NULL, then there is nothing to do */
  if (! prev_node)
    {
      if (! parent_node)
	;			/* neither a parent nor a sibling -- nothing to do */
      else
	{
	  /* parent, but no previous sibling -- we are first in line */
	  nsibling(orig_node) = nchild (parent_node);
	  nchild (parent_node) = orig_node;
	}
    }
  else
    {
      if (parent_node)
	{
	  /* have sibling and parent */
	  nparent (orig_node) = parent_node;
	  nsibling (orig_node) = nsibling (prev_node);
	  nsibling (prev_node) = orig_node;
	}
      else
	{
	  /* have sibling, but no parent -- extend the line */
	  nsibling (orig_node) = nsibling(prev_node);
	  nsibling(prev_node) = orig_node;
	}
    }
  return (self);
}

/* llpy_create_node (tag,[value]) --> NODE Returns the newly created
   node having specified TAG.  If VALUE is omitted, None, or empty,
   then the value is empty.*/
static PyObject *llpy_create_node (PyObject *self ATTRIBUTE_UNUSED, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "tag", "value", NULL };
  STRING xref = 0;
  STRING tag = 0;
  STRING value = 0;
  NODE parent = 0;
  NODE node;
  LLINES_PY_NODE *py_node;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "s|z", keywords, &tag, &value))
    return NULL;

  node = create_temp_node (xref, tag, value, parent);
  if (! (py_node = PyObject_New(LLINES_PY_NODE, &llines_node_type)))
    return NULL;

  py_node->lnn_node = node;
  py_node->lnn_type = 0;	/* unknown */

  return ((PyObject *)py_node);
}

/* llpy_nodeiter -- returns an iterator for the input node tree.

   The 'type' of iterator depends on the input arguments.

   If TYPE is CHILD, we iterate over the immediate children of the
   input node.

   If TYPE is TRAVERSE, we iterate over all the nodes, depth first,
   parent before child.

   TAG is either None or a string.  If it is None, then we behave as
   above.  If it is a string, then we behave as above *BUT* before
   returning a node, we compare its tag against the string.  If they
   match we return it, if they do not match, we continue the iteration
   until we find a match or we exhaust the iteration.  */

static PyObject *llpy_nodeiter (PyObject *self, PyObject *args, PyObject *kw)
{
  static char *keywords[] = { "type", "tag", NULL };
  int type = 0;
  char *tag = 0;
  LLINES_PY_NODEITER *iter;

  if (! PyArg_ParseTupleAndKeywords (args, kw, "i|z", keywords, &type, &tag))
    return NULL;

  if ((type != ITER_CHILDREN) && (type != ITER_TRAVERSE))
    {
      PyErr_SetString (PyExc_ValueError, "nodeiter: type must be either ITER_CHILDREN or ITER_TRAVERSE");
      return NULL;
    }
  iter = PyObject_New (LLINES_PY_NODEITER, &llines_nodeiter_type);
  if (! iter)
    return NULL;

  Py_INCREF (self);
  iter->ni_top_node = ((LLINES_PY_NODE *)self)->lnn_node;
  iter->ni_cur_node = NULL;
  iter->ni_type = type;
  iter->ni_level = 0;
  if ((tag == 0) || (*tag == '\0'))
    iter->ni_tag = 0;
  else
    iter->ni_tag = strdup (tag);

  return (PyObject *) iter;
}

static void llpy_node_dealloc (PyObject *self)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  if (llpy_debug)
    {
      fprintf (stderr, "llpy_family_dealloc entry: self %p refcnt %ld\n",
	       (void *)self, Py_REFCNT (self));
    }
  free_nodes (node->lnn_node);
  node->lnn_node = 0;
  node->lnn_type = 0;
  Py_TYPE(self)->tp_free (self);
#if 0
  Py_DECREF (Py_TYPE(self));
#endif
}

static struct PyMethodDef Lifelines_Node_Methods[] =
  {
   /* NODE Functions */

   { "xref",		(PyCFunction)llpy_xref, METH_NOARGS,
     "(NODE).xref(void) --> STRING; returns cross reference index of NODE" },
   { "tag",		(PyCFunction)llpy_tag, METH_NOARGS,
     "(NODE).tag(void) --> STRING; returns tag of NODE" },
   { "value",		(PyCFunction)llpy_value, METH_NOARGS,
     "(NODE).value(void) --> STRING; returns value of NODE" },
   { "parent_node",	(PyCFunction)llpy_parent_node, METH_NOARGS,
     "(NODE).parent_node(void) --> NODE; returns parent node of NODE" },
   { "child_node",	(PyCFunction)llpy_child_node, METH_NOARGS,
     "(NODE).child_node(void) --> NODE; returns first child of NODE" },
   { "sibling_node",	(PyCFunction)llpy_sibling_node, METH_NOARGS,
     "(NODE).sibling_node(void) --> NODE; returns next sibling of NODE" },
   { "copy_node_tree",	(PyCFunction)llpy_copy_node_tree, METH_NOARGS,
     "(NODE).copy_node_tree(void) --> NODE; returns a copy of the node structure" },
   { "level",		(PyCFunction)llpy_level, METH_NOARGS,
     "(NODE).level(void) --> INT; returns the level of NODE" },

   { "add_node",	(PyCFunction)llpy_add_node, METH_VARARGS | METH_KEYWORDS,
     "(NODE).add_node([parent],[prev]) --> NODE.  Modifies node to have\n\
parent PARENT and previous sibling PREV.  Returns modified NODE" },
   { "nodeiter",	(PyCFunction)llpy_nodeiter, METH_VARARGS | METH_KEYWORDS,
     "nodeiter(type, [tag]) --> Iterator over node tree.\n\
TYPE is an int -- one of ITER_CHILDREN or ITER_TRAVERSE.\n\
TAG, if specified, restricts the iterator to just those nodes having that tag." },
   { NULL, 0, 0, NULL }		/* sentinel */
  };

static struct PyMethodDef Lifelines_Node_Functions[] =
  {
   { "create_node",	(PyCFunction)llpy_create_node, METH_VARARGS | METH_KEYWORDS,
     "create_node(tag,[value]) --> NODE.  Creates node having tag and value." },

   { NULL, 0, 0, NULL }		/* sentinel */
  };

PyTypeObject llines_node_type =
  {
   PyVarObject_HEAD_INIT(NULL, 0)
   .tp_name = "llines.Node",
   .tp_doc = "Lifelines GEDCOM generic Node",
   .tp_basicsize = sizeof (LLINES_PY_NODE),
   .tp_itemsize = 0,
   .tp_flags = Py_TPFLAGS_DEFAULT,
   .tp_new = PyType_GenericNew,
   .tp_dealloc = llpy_node_dealloc,
   .tp_methods = Lifelines_Node_Methods,
  };

void llpy_nodes_init (void)
{
  int status;

  status = PyModule_AddFunctions (Lifelines_Module, Lifelines_Node_Functions);
  if (status != 0)
    fprintf (stderr, "llpy_nodes_initj: attempt to add functions returned %d\n", status);
}
