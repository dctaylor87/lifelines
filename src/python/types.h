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
  int lln_type;
  NODE lln_node;		/* Lifelines GEDCOM generic NODE */
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

/* llr_type values for records and nodes -- will likely be augmented
   in the future.  And might become numerical rather than mnemonic */

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
  int lli_type;
  RECORD lli_record;		/* Lifelines GEDCOM Individual RECORD */
} LLINES_PY_INDI;

typedef struct {
  PyObject_HEAD
  int llf_type;
  RECORD llf_record;		/* Lifelines GEDCOM Family RECORD */
} LLINES_PY_FAM;

typedef struct {
  PyObject_HEAD
  int lle_type;
  RECORD lle_record;		/* Lifelines GEDCOM Event RECORD */
} LLINES_PY_EVEN;

typedef struct {
  PyObject_HEAD
  int lls_type;
  RECORD lls_record;		/* Lifelines GEDCOM Source RECORD */
} LLINES_PY_SOUR;

typedef struct {
  PyObject_HEAD
  int llo_type;
  RECORD llo_record;		/* Lifelines GEDCOM Other RECORD */
} LLINES_PY_OTHR;

typedef struct {
  PyObject_HEAD
  /* XXX insert fields here that are useful for database 'types' XXX */
} LLINES_PY_DATABASE;

/* various GEDCOM record types */

extern PyTypeObject llines_database_type;
extern PyTypeObject llines_event_type;
extern PyTypeObject llines_family_type;
extern PyTypeObject llines_individual_type;
extern PyTypeObject llines_other_type;
extern PyTypeObject llines_source_type;

/* generic GEDCOM record type -- not sure if we need this */

extern PyTypeObject llines_record_type;

/* GEDCOM node */
extern PyTypeObject llines_node_type;
