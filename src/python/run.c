/* run.c -- functions for running the Python interpreter -- if it is
   compiled in */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ansidecl.h>		/* ATTRIBUTE_UNUSED */
#include <stdio.h>

#include "standard.h"		/* STRING */
#include "llstdlib.h"		/* CALLBACK_FNC (needed by gedcom.h) */
#include "lloptions.h"

#include "python-to-c.h"
#include "llpy-externs.h"

/* we have three things to handle:

. calling Python interactively if the user specified -P

. 'registering' each script specified on the command line

. calling the scripts */

/* forward references */

static int execute_script (CNSTRING script);

/* start of code */

/* array of scripts seen on the command line, in the order seen */
static char **script_array;

/* how many scripts we've seen */
int nscripts = 0;

/* so that one time initialization is only run once */
static int initialized = 0;

/* llpy_register_script -- we record or register the sciripts seen on
   the command line.  We don't try to be fancy and optimize the calls
   to allocate space for the script names as typically the user will
   only specify one script or possibly two.

   TODO: figure out how to do script specific arguments and options. */

int
llpy_register_script (char *script)
{
  char **new_array;

  if (nscripts)
    new_array = (char **)realloc ((void *)script_array, (nscripts + 1) * sizeof (char *));
  else
    new_array = (char **)malloc (sizeof (char *));

  if (! new_array)
    return (-1);		/* malloc / realloc failed! */

  new_array[nscripts] = script;
  script_array = new_array;
  nscripts++;
  return (0);
}

/* llpy_execute_scripts -- execute the scripts in the order specified
   on the command line.

   continue_on_failure -- if true and a script fails, we continue to
   the next script, otherwise we return with the failure status.

   TODO: make continue on failure script specific.

   FUTURE TODO: figure out how to pass arguments and options to the
   script -- something better than reading stdin for arguments. */

int
llpy_execute_scripts (int continue_on_failure)
{
  int saved_status = 0;

  if (! initialized)
    {
      llpy_init ();
      Py_Initialize ();
      initialized = 1;
    }

  for (int cur_script = 0; cur_script < nscripts; cur_script++)
    {
      int status = execute_script (script_array[cur_script]);
      if (status < 0)
	{
	  if (! continue_on_failure)
	    return (status);
	  else
	    saved_status = status;
	}
    }
  return (saved_status);	/* most recent failure, if any, otherwise zero */
}

static int
execute_script (CNSTRING report_name)
{
  FILE *report_fp;
  /* XXX might want to open in binary mode -- LLREADBINARY XXX */
  STRING mode = LLREADTEXT;	/* "r" on POSIX, on M$ systems this is "rt" */
  STRING ext = ".py";
  INT utf8 = uu8;
  STRING report_pathname;
  char *path;
  int status;
  PyGILState_STATE py_gil;	/* python's GIL */

  path = getlloptstr ("LLPROGRAMS", ".");
  report_fp = fopenpath (report_name, mode, path, ext, utf8, &report_pathname);

  if (! report_fp)
    {
      /* XXX give the bad news XXX */
      fprintf (stderr, "unable to open %s\n", report_name);
      return (-1);
    }
  py_gil = PyGILState_Ensure();
  status = PyRun_SimpleFile (report_fp, report_pathname);
  PyGILState_Release(py_gil);

  fclose (report_fp);
  return (status);
}

int
llpy_python_interactive (void)
{
  int status;

  if (! initialized)
    {
      llpy_init ();
      Py_Initialize ();
      initialized = 1;
    }
  status = PyRun_InteractiveLoop (stdin, "<stdin>");
  return (status);
}
