/* declarations of stuff in the python directory that is shared with
   the rest of Lifelines */

extern void llpy_init (void);
extern void llpy_python_terminate (void);

extern int llpy_register_script (char *script);
extern int llpy_execute_scripts (int continue_on_failure);
extern int llpy_python_interactive (void);
