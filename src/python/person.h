/* person.h -- private interfaces

   these are functions that the Python interpreter can call -- these
   functions extend the intrepreter.  These functions are currently
   non-static.  That might change -- this header might be eliminated
   and static variants of the declarations folded into the appropriate
   C file (probably person.c).  */

PyObject *llpy_name (PyObject *self, PyObject *args, PyObject *kw);
PyObject *llpy_fullname (PyObject *self, PyObject *args, PyObject *kw);
PyObject *llpy_surname (PyObject *self, PyObject *args);
PyObject *llpy_givens (PyObject *self, PyObject *args);
PyObject *llpy_trimname (PyObject *self, PyObject *args, PyObject *kw);
PyObject *llpy_birth (PyObject *self, PyObject *args);
PyObject *llpy_death (PyObject *self, PyObject *args);
PyObject *llpy_burial (PyObject *self, PyObject *args);
PyObject *llpy_father (PyObject *self, PyObject *args);
PyObject *llpy_mother (PyObject *self, PyObject *args);
PyObject *llpy_nextsib (PyObject *self, PyObject *args);
PyObject *llpy_prevsib (PyObject *self, PyObject *args);
PyObject *llpy_sex (PyObject *self, PyObject *args);
PyObject *llpy_male (PyObject *self, PyObject *args);
PyObject *llpy_female (PyObject *self, PyObject *args);
PyObject *llpy_pn (PyObject *self, PyObject *args, PyObject *kw);
PyObject *llpy_nspouses (PyObject *self, PyObject *args);
PyObject *llpy_nfamilies (PyObject *self, PyObject *args);

PyObject *llpy_title (PyObject *self, PyObject *args);

PyObject *llpy_soundex (PyObject *self, PyObject *args);

PyObject *llpy_nextindi (PyObject *self, PyObject *args);
PyObject *llpy_previndi (PyObject *self, PyObject *args);

/* returns set of spouses */
PyObject *llpy_spouses (PyObject *self, PyObject *args);

/* returns set of families */
PyObject *llpy_indi_families (PyObject *self, PyObject *args);
