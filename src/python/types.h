/* requires PY_SSIZE_T_CLEAN to be defined and <Python.h> to be
   included first */

#if 0
typedef struct {
  PyObject_HEAD
  PVALUE llp_pvalue;		/* Lifelines PVALUE -- do we need this? */
} LLINES_PY_PVALUE;
#endif

typedef struct {
  PyObject_HEAD
  /* XXX place Lifelines member fields here XXX */
} Lifelines_Object;

typedef struct {
  PyObject_HEAD
  int lnn_type;
  NODE lnn_node;		/* Lifelines GEDCOM generic NODE */
} LLINES_PY_NODE;

typedef struct {
  PyObject_HEAD
  int lni_type;
  NODE lni_node;		/* Lifelines GEDCOM INDI NODE */
} LLINES_PY_INDI_NODE;

typedef struct {
  PyObject_HEAD
  int lnf_type;
  NODE lnf_node;		/* Lifelines GEDCOM FAM NODE */
} LLINES_PY_FAM_NODE;

typedef struct {
  PyObject_HEAD
  int lne_type;
  NODE lne_node;		/* Lifelines GEDCOM EVEN NODE */
} LLINES_PY_EVEN_NODE;

typedef struct {
  PyObject_HEAD
  int lns_type;
  NODE lns_node;		/* Lifelines GEDCOM SOUR NODE */
} LLINES_PY_SOUR_NODE;

typedef struct {
  PyObject_HEAD
  int lno_type;
  NODE lno_node;		/* Lifelines GEDCOM OTHR NODE */
} LLINES_PY_OTHR_NODE;

/* type values for records and nodes -- will likely be augmented in
   the future.  And might become numerical rather than mnemonic.
   Another possibility is uppercase for RECORDs and lowercase for
   NODEs... with corresponding name changes. */

#define LLINES_TYPE_INDI	'I' /* individual */
#define LLINES_TYPE_FAM		'F' /* family */
#define LLINES_TYPE_SOUR	'S' /* source */
#define LLINES_TYPE_EVEN	'E' /* event */
#define LLINES_TYPE_OTHR	'X' /* other */

typedef struct {
  PyObject_HEAD
  int llr_type;
  RECORD llr_record;		/* Lifelines GEDCOM generic RECORD */
} LLINES_PY_RECORD;

typedef struct {
  PyObject_HEAD
  int lri_type;
  RECORD lri_record;		/* Lifelines GEDCOM Individual RECORD */
} LLINES_PY_INDI_RECORD;

typedef struct {
  PyObject_HEAD
  int lrf_type;
  RECORD lrf_record;		/* Lifelines GEDCOM Family RECORD */
} LLINES_PY_FAM_RECORD;

typedef struct {
  PyObject_HEAD
  int lre_type;
  RECORD lre_record;		/* Lifelines GEDCOM Event RECORD */
} LLINES_PY_EVEN_RECORD;

typedef struct {
  PyObject_HEAD
  int lrs_type;
  RECORD lrs_record;		/* Lifelines GEDCOM Source RECORD */
} LLINES_PY_SOUR_RECORD;

typedef struct {
  PyObject_HEAD
  int lro_type;
  RECORD lro_record;		/* Lifelines GEDCOM Other RECORD */
} LLINES_PY_OTHR_RECORD;

typedef struct {
  PyObject_HEAD
  /* XXX insert fields here that are useful for database 'types' XXX */
} LLINES_PY_DATABASE;

/* the 'iter' functions for each RECORD type (INDI, FAM...) provide an
   instance of this type with the li_type field set to the RECORD type
   and the li_current field set to zero.

   The tp_iternext function for this type then examines the li_type
   field to know which xref_next* function to call, supplying the
   li_current value.  The returned value is then both stored in
   li_current and converted to a RECORD that is returned to the
   caller.  */

typedef struct {
  PyObject_HEAD
  int li_type;			/* the type being iterated over, NOT *our* type */
  int li_current;		/* argument to supply to xref_next* function */
} LLINES_PY_ITER;

/* various GEDCOM record types */

extern PyTypeObject llines_database_type;
extern PyTypeObject llines_event_type;

extern PyTypeObject llines_family_type;
extern PyTypeObject llines_individual_type;
extern PyTypeObject llines_other_type;
extern PyTypeObject llines_source_type;

extern PyTypeObject llines_iter_type;

/* generic GEDCOM record type -- not sure if we need this */

extern PyTypeObject llines_record_type;

/* GEDCOM node */
extern PyTypeObject llines_node_type;
