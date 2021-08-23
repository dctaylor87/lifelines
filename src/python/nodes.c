#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */

#include "standard.h"		/* STRING */
#include "llstdlib.h"
#include "gedcom.h"
#include "types.h"

/* forward references */

static PyObject *llpy_xref (PyObject *self, PyObject *args);
static PyObject *llpy_tag (PyObject *self, PyObject *args);
static PyObject *llpy_value (PyObject *self, PyObject *args);
static PyObject *llpy_parent_node (PyObject *self, PyObject *args);
static PyObject *llpy_child_node (PyObject *self, PyObject *args);
static PyObject *llpy_sibling_node (PyObject *self, PyObject *args);
static PyObject *llpy_copy_node_tree (PyObject *self, PyObject *args);
static PyObject *llpy_level (PyObject *self, PyObject *args);

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
  LLINES_PY_NODE *parent = PyObject_New (LLINES_PY_NODE, &llines_node_type);

  if (! parent)
    return NULL;		/* PyOBject_New failed and set an exception */

  parent->lnn_type = 0;		/* XXX figure out what to put here XXX */
  parent->lnn_node = nparent (node->lnn_node);

  return (PyObject *)parent;
}

static PyObject *llpy_child_node (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  abort ();
}

static PyObject *llpy_sibling_node (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  abort ();
}

static PyObject *llpy_copy_node_tree (PyObject *self, PyObject *args ATTRIBUTE_UNUSED)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  LLINES_PY_NODE *copy = PyObject_New (LLINES_PY_NODE, &llines_node_type);

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


static void llpy_node_dealloc (PyObject *self)
{
  LLINES_PY_NODE *node = (LLINES_PY_NODE *) self;
  free_nodes (node->lnn_node);
  node->lnn_node = 0;
  node->lnn_type = 0;
  Py_TYPE(self)->tp_free (self);
  Py_DECREF (Py_TYPE(self));
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
     "(NODE).copy_node_tree --> NODE; returns a copy of the node structure" },
   { "level",		(PyCFunction)llpy_level, METH_NOARGS,
     "(NODE).level --> INT; returns the level of NODE" },

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
