#include "qterm.h"

#ifdef HAVE_PYTHON
#include <Python.h>
extern QString getException();
extern QString getErrOutputFile(QTermWindow *);

extern PyMethodDef qterm_methods[];
#endif
