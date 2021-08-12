/* python-to-c -- private interfaces

   these are functions that the Python interpreter can call -- these
   functions extend the intrepreter.  These functions are currently
   non-static as the expectation is that they will be spread over
   multiple files.  If that changes, this header might be eliminated
   and static variants of the declarations folded into the appropriate
   C file.  */

PyObject *llpy_parents (PyObject *self, PyObject *args);
PyObject *llpy_key (PyObject *self, PyObject *args, PyObject *kw);
PyObject *llpy_inode (PyObject *self, PyObject *args);

PyObject *llpy_spouseset (PyObject *self, PyObject *args);
PyObject *llpy_fnode (PyObject *self, PyObject *args);
PyObject *llpy_root (PyObject *self, PyObject *args);
PyObject *llpy_fam (PyObject *self, PyObject *args);

PyObject *llpy_xref (PyObject *self, PyObject *args);
PyObject *llpy_tag (PyObject *self, PyObject *args);
PyObject *llpy_value (PyObject *self, PyObject *args);
PyObject *llpy_parent (PyObject *self, PyObject *args);
PyObject *llpy_child (PyObject *self, PyObject *args);
PyObject *llpy_sibling (PyObject *self, PyObject *args);
PyObject *llpy_savenode (PyObject *self, PyObject *args);
PyObject *llpy_level (PyObject *self, PyObject *args);
PyObject *llpy_date (PyObject *self, PyObject *args);
PyObject *llpy_place (PyObject *self, PyObject *args);
PyObject *llpy_year (PyObject *self, PyObject *args);
PyObject *llpy_long (PyObject *self, PyObject *args);
PyObject *llpy_short (PyObject *self, PyObject *args);
PyObject *llpy_gettoday (PyObject *self, PyObject *args);
PyObject *llpy_setdate (PyObject *self, PyObject *args);
PyObject *llpy_dayformat (PyObject *self, PyObject *args);
PyObject *llpy_monthformat (PyObject *self, PyObject *args);
PyObject *llpy_yearformat (PyObject *self, PyObject *args);
PyObject *llpy_eraformat (PyObject *self, PyObject *args);
PyObject *llpy_dateformat (PyObject *self, PyObject *args);
PyObject *llpy_datepic (PyObject *self, PyObject *args);
PyObject *llpy_stddate (PyObject *self, PyObject *args);
PyObject *llpy_complexformat (PyObject *self, PyObject *args);
PyObject *llpy_complexpic (PyObject *self, PyObject *args);
PyObject *llpy_complexdate (PyObject *self, PyObject *args);
PyObject *llpy_dayofweek (PyObject *self, PyObject *args);
PyObject *llpy_date2jd (PyObject *self, PyObject *args);
PyObject *llpy_jd2date (PyObject *self, PyObject *args);
PyObject *llpy_extractdate (PyObject *self, PyObject *args);
PyObject *llpy_extractnames (PyObject *self, PyObject *args);
PyObject *llpy_extractplaces (PyObject *self, PyObject *args);
PyObject *llpy_extracttokens (PyObject *self, PyObject *args);
PyObject *llpy_extractdatestr (PyObject *self, PyObject *args);
PyObject *llpy_getindi (PyObject *self, PyObject *args, PyObject *kw);
PyObject *llpy_getindiset (PyObject *self, PyObject *args);
PyObject *llpy_getfam (PyObject *self, PyObject *args);
PyObject *llpy_getint (PyObject *self, PyObject *args);
PyObject *llpy_getstr (PyObject *self, PyObject *args);
PyObject *llpy_choosechild (PyObject *self, PyObject *args);
PyObject *llpy_choosefam (PyObject *self, PyObject *args);
PyObject *llpy_chooseindi (PyObject *self, PyObject *args);
PyObject *llpy_choosespouse (PyObject *self, PyObject *args);
PyObject *llpy_choosesubset (PyObject *self, PyObject *args);
PyObject *llpy_menuchoose (PyObject *self, PyObject *args);
PyObject *llpy_indiset (PyObject *self, PyObject *args);
PyObject *llpy_parentset (PyObject *self, PyObject *args);
PyObject *llpy_childset (PyObject *self, PyObject *args);
PyObject *llpy_spouseset (PyObject *self, PyObject *args);
PyObject *llpy_siblingset (PyObject *self, PyObject *args);
PyObject *llpy_ancestorset (PyObject *self, PyObject *args);
PyObject *llpy_descendentset (PyObject *self, PyObject *args);
PyObject *llpy_descendentset (PyObject *self, PyObject *args);
PyObject *llpy_getindiset (PyObject *self, PyObject *args);
PyObject *llpy_gengedcomstrong (PyObject *self, PyObject *args);
PyObject *llpy_program (PyObject *self, PyObject *args);
PyObject *llpy_version (PyObject *self, PyObject *args);
PyObject *llpy_heapused (PyObject *self, PyObject *args);
