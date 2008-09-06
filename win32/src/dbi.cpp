/*
  dbimodule.c

  Donated to the Python community by EShop, who can not
  support it!

  this is the general interface to copperman compliant databases.

  In particular, types and type numbers are defined
 */

#include <time.h>

#include "pywintypes.h"
#include "structmember.h"

/* Python 1.5.2 doesn't have PyObject_New
   PyObject_NEW is not *quite* as safe, but seem to work fine
   (as all win32all for 1.5.2 used it! */
#ifndef PyObject_New 
#define PyObject_New PyObject_NEW
#endif

#define DBI_EXPORT

#include "dbi.h"

//#ifdef _WIN64
//# define ctime _ctime32
//#endif

typedef struct
{
	PyObject_HEAD
	PyObject *objectOf;
} DbiContainer;

static struct PyMemberDef DbiContainer_members[] = {
	{"value", offsetof(DbiContainer, objectOf), T_OBJECT},
	{NULL},
};

PyObject *dbiValue(PyObject *o)
{
	return ((DbiContainer *) o)->objectOf;
}

static PyObject *makeDbiTypeTP(PyObject *args, PyTypeObject *ty)
{
	DbiContainer *dbt = 0;
	PyObject *ob;
  
	if (PyArg_ParseTuple(args, "O", &ob))
	{
		dbt = PyObject_New(DbiContainer, ty);
		dbt->objectOf = ob;
		Py_INCREF(ob);
	}
	return (PyObject*)dbt;
}

static void dbiDealloc(PyObject *self)
{
	Py_DECREF(((DbiContainer *) self)->objectOf);
#ifdef PyObject_Del /* see top of file for 1.5.2 comments */
	PyObject_Del(self);
#else
	PyMem_Free(self);
#endif
}

static PyMethodDef noMethods[] =
{
	{0, 0}
};

static PyObject *dbiRawStr(PyObject *self)
{
	PyObject *val = dbiValue(self);
	Py_INCREF(val);
	return val;
}

static PyObject *dateStr(PyObject *o)
{
	time_t t;
	if (!PyWinObject_Astime_t(dbiValue(o), &t))
		return NULL;
	return PyString_FromStringAndSize(ctime(&t), 24); /* less \n */
}

#define delg(a) dbiValue(a)->ob_type->tp_as_number

static PyObject* dt_nb_add(PyObject* a, PyObject* b)
{
	return delg(a)->nb_add(dbiValue(a),b);
}
static PyObject* dt_nb_subtract(PyObject* a, PyObject* b)
{
	return delg(a)->nb_subtract(dbiValue(a),b);
}
static PyObject* dt_nb_int(PyObject* a)
{
	return delg(a)->nb_int(dbiValue(a));
}
static PyObject* dt_nb_long(PyObject* a)
{
	return delg(a)->nb_long(dbiValue(a));
}
static PyObject* dt_nb_float(PyObject* a)
{
	return delg(a)->nb_float(dbiValue(a));
}

#if (PY_VERSION_HEX < 0x03000000)
static PyObject* dt_nb_oct(PyObject* a)
{
	return delg(a)->nb_oct(dbiValue(a));
}
static PyObject* dt_nb_hex(PyObject* a)
{
	return delg(a)->nb_hex(dbiValue(a));
}
#endif

static int dt_cmp(PyObject *a, PyObject *b)
{
	return dbiValue(a)->ob_type->tp_compare(a,b);
}
static int dt_nb_coerce(PyObject **, PyObject **);

PyNumberMethods dt_as_number =
{
	dt_nb_add ,   /* nb_add */
	dt_nb_subtract ,   /* nb_subtract */
	0,   /* nb_multiply */
#if (PY_VERSION_HEX < 0x03000000)
	0,   /* nb_divide (removed in Python 3)*/
#endif
	0,   /* nb_remainder */
	0,   /* nb_divmod */
	0,   /* nb_power */
	0,   /* nb_negative */
	0,   /* nb_positive */
	0,   /* nb_absolute */
	0,   /* nb_nonzero */
	0,   /* nb_invert */
	0,   /* nb_lshift */
	0,   /* nb_rshift */
	0,   /* nb_and */
	0,   /* nb_xor */
	0,   /* nb_or */
#if (PY_VERSION_HEX < 0x03000000)
	dt_nb_coerce,   /* nb_coerce (removed in Python 3) */
#endif
	dt_nb_int ,   /* nb_int */
	dt_nb_long ,   /* nb_long */
	dt_nb_float ,   /* nb_float */
#if (PY_VERSION_HEX < 0x03000000)
	dt_nb_oct ,   /* nb_oct */
	dt_nb_hex    /* nb_hex */
#endif
};

static PyTypeObject DbiDate_Type =
{
	PYWIN_OBJECT_HEAD
	"DbiDate",				/*tp_name */
	sizeof(DbiContainer),	/*tp_basicsize */
	0,						/*tp_itemsize */
	dbiDealloc,				/*tp_dealloc */
	0,						/*tp_print */
	0,						/*tp_getattr */
	0,						/*tp_setattr */
	dt_cmp,					/*tp_compare */
	0,                    /*tp_repr */
	&dt_as_number,        /**tp_as_number */
	0,                    /**tp_as_sequence */
	0,                    /**tp_as_mapping */
	0,                    /*tp_hash */
	0,                    /*tp_call */
	dateStr,               /*tp_str */
	PyObject_GenericGetAttr,	/* tp_getattro dbiGetAttr */
	PyObject_GenericSetAttr,	/* tp_setattro */
	0,						/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	0,						/* tp_doc */
	0,						/* tp_traverse */
	0,						/* tp_clear */
	0,						/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	0,						/* tp_iter */
	0,						/* tp_iternext */
	noMethods,				/* tp_methods */
	DbiContainer_members,	/* tp_members */
	0,						/* tp_getset */
	0,						/* tp_base */
	0,						/* tp_dict */
	0,						/* tp_descr_get */
	0,						/* tp_descr_set */
	0,						/* tp_dictoffset */
	0,						/* tp_init */
	0,						/* tp_alloc */
	0,						/* tp_new */
};

static int dt_nb_coerce(PyObject **pv, PyObject **pw)
{
	if (PyNumber_Check(*pw))
	{
		DbiContainer *dbt = PyObject_New(DbiContainer, &DbiDate_Type);
		dbt->objectOf = PyNumber_Int(*pw);
		Py_INCREF(*pv);
		*pw = (PyObject *)dbt;
		return 0;
	}
	return 1; /* Can't do it */
}

static PyTypeObject DbiRaw_Type =
{
	PYWIN_OBJECT_HEAD
	"DbiRaw",				/*tp_name */
	sizeof(DbiContainer),	/*tp_basicsize */
	0,						/*tp_itemsize */
	dbiDealloc,				/*tp_dealloc */
	0,						/*tp_print */
	0,						/*tp_getattr */
	0,                    /*tp_setattr */
	0,                    /*tp_compare */
	0,                    /*tp_repr */
	0,                    /**tp_as_number */
	0,                    /**tp_as_sequence */
	0,                    /**tp_as_mapping */
	0,                    /*tp_hash */
	0,						/*tp_call */
	dbiRawStr,				/*tp_str */
	PyObject_GenericGetAttr,	/* tp_getattro */
	PyObject_GenericSetAttr,	/* tp_setattro */
	0,						/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	0,						/* tp_doc */
	0,						/* tp_traverse */
	0,						/* tp_clear */
	0,						/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	0,						/* tp_iter */
	0,						/* tp_iternext */
	noMethods,				/* tp_methods */
	DbiContainer_members,	/* tp_members */
	0,						/* tp_getset */
	0,						/* tp_base */
	0,						/* tp_dict */
	0,						/* tp_descr_get */
	0,						/* tp_descr_set */
	0,						/* tp_dictoffset */
	0,						/* tp_init */
	0,						/* tp_alloc */
	0,						/* tp_new */
};

static PyTypeObject DbiRowId_Type =
{
	PYWIN_OBJECT_HEAD
	"DbiRowId",				/*tp_name */
	sizeof(DbiContainer),	/*tp_basicsize */
	0,						/*tp_itemsize */
	dbiDealloc,				/*tp_dealloc */
	0,						/*tp_print */
	0,						/*tp_getattr */
	0,                    /*tp_setattr */
	0,                    /*tp_compare */
	0,                    /*tp_repr */
	0,                    /**tp_as_number */
	0,                    /**tp_as_sequence */
	0,                    /**tp_as_mapping */
	0,                    /*tp_hash */
	0,						/*tp_call */
	dbiRawStr,				/*tp_str */
	PyObject_GenericGetAttr,	/* tp_getattro */
	PyObject_GenericSetAttr,	/* tp_setattro */
	0,						/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	0,						/* tp_doc */
	0,						/* tp_traverse */
	0,						/* tp_clear */
	0,						/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	0,						/* tp_iter */
	0,						/* tp_iternext */
	noMethods,				/* tp_methods */
	DbiContainer_members,	/* tp_members */
	0,						/* tp_getset */
	0,						/* tp_base */
	0,						/* tp_dict */
	0,						/* tp_descr_get */
	0,						/* tp_descr_set */
	0,						/* tp_dictoffset */
	0,						/* tp_init */
	0,						/* tp_alloc */
	0,						/* tp_new */
};

static PyObject *makeDate(PyObject *self, PyObject *args)
{
	return makeDbiTypeTP(args, &DbiDate_Type);
}

static PyObject *makeRaw(PyObject *self, PyObject *args)
{
  return makeDbiTypeTP(args, &DbiRaw_Type);
}

static PyObject *makeRowId(PyObject *self, PyObject *args)
{
  return makeDbiTypeTP(args, &DbiRowId_Type);
}

static PyObject *makeDbiType(PyObject *o, PyTypeObject *ty)
{
  DbiContainer *dbt = PyObject_New(DbiContainer, ty);
  dbt->objectOf = o;
  return (PyObject*) dbt;
}

PyObject *dbiMakeDate(PyObject *contents)
{
  return makeDbiType(contents, &DbiDate_Type);
}

PyObject *dbiMakeRaw(PyObject *contents)
{
  return makeDbiType(contents, &DbiRaw_Type);
}

PyObject *dbiMakeRowId(PyObject *contents)
{
  return makeDbiType(contents, &DbiRowId_Type);
}



static PyMethodDef globalMethods[] =
{
	{ "dbDate", makeDate, 1} ,
	{ "dbiDate", makeDate, 1} ,
	{ "dbRaw", makeRaw, 1} ,
	{ "dbiRaw", makeRaw, 1} ,
	{0,     0}        /* Sentinel */
};

extern "C" __declspec(dllexport)
#if (PY_VERSION_HEX < 0x03000000)
void initdbi(void)
#else
PyObject *PyInit_dbi(void)
#endif
{
	PyObject *dict, *module;
	PyWinGlobals_Ensure();

#if (PY_VERSION_HEX < 0x03000000)
#define RETURN_ERROR return;
	module = Py_InitModule("dbi", globalMethods);

#else
#define RETURN_ERROR return NULL;
	static PyModuleDef dbi_def = {
		PyModuleDef_HEAD_INIT,
		"dbi",
		"Wraps ODBC datatypes",
		-1,
		globalMethods
		};
	module = PyModule_Create(&dbi_def);
#endif

	if (!module)
		RETURN_ERROR;
	dict = PyModule_GetDict(module);
	if (!dict)
		RETURN_ERROR;

	if (PyType_Ready(&DbiDate_Type) == -1
		||PyType_Ready(&DbiRaw_Type) == -1
		||PyType_Ready(&DbiRowId_Type) == -1)
		RETURN_ERROR;

	/* These need to be unicode strings in Py3k and char strings in 2.x regardless if UNICODE
		is defined. Py_BuildValue with 's' format is a convenient way to do so.
		??? Why expose them in multiple ways ???
	*/
	char *szDbiString = "STRING";
	char *szDbiRaw = "RAW";
	char *szDbiNumber = "NUMBER";
	char *szDbiDate = "DATE";
	char *szDbiRowId = "ROWID";
	PyObject *obtypes=Py_BuildValue("(sssss)",
			szDbiString,
			szDbiRaw,
			szDbiNumber,
			szDbiDate,
			szDbiRowId);
	// Steals a ref to obtypes, so it doesn't need to be DECREF'ed.
	if (obtypes==NULL || PyModule_AddObject(module, "TYPES", obtypes) == -1)
		RETURN_ERROR;
	// These are exported and used in odbc.cpp, so keep our own ref
	DbiString = PyTuple_GET_ITEM(obtypes, 0);
	Py_INCREF(DbiString);
	DbiRaw = PyTuple_GET_ITEM(obtypes, 1);
	Py_INCREF(DbiRaw);
	DbiNumber = PyTuple_GET_ITEM(obtypes, 2);
	Py_INCREF(DbiNumber);
	DbiDate = PyTuple_GET_ITEM(obtypes, 3);
	Py_INCREF(DbiDate);
	DbiRowId = PyTuple_GET_ITEM(obtypes, 4);
	Py_INCREF(DbiRowId);
	/* ??? These are also added to the module with attribute name same as value,
			not sure what the point of this is ???
	*/
	if (PyDict_SetItem(dict, DbiString, DbiString) == -1
		||PyDict_SetItem(dict, DbiRaw, DbiRaw) == -1
		||PyDict_SetItem(dict, DbiNumber, DbiNumber) == -1
		||PyDict_SetItem(dict, DbiDate, DbiDate) == -1
		||PyDict_SetItem(dict, DbiRowId, DbiRowId) == -1)
		RETURN_ERROR;

	/* Establish errors
		The class names have been changed to agree with the name with which they are added to the module.
		Formerly they were actually invalid identifiers.
	*/
	DbiNoError = PyErr_NewException("dbi.noError", NULL, NULL);
	if (DbiNoError == NULL || PyDict_SetItemString(dict, "noError", DbiNoError) == -1)
		RETURN_ERROR;
	DbiOpError = PyErr_NewException("dbi.opError", NULL, NULL);
	if (DbiOpError == NULL || PyDict_SetItemString(dict, "opError", DbiOpError) == -1)
		RETURN_ERROR;
	DbiProgError = PyErr_NewException("dbi.progError", NULL, NULL);
	if (DbiProgError == NULL || PyDict_SetItemString(dict, "progError", DbiProgError) == -1)
		RETURN_ERROR;
	DbiIntegrityError = PyErr_NewException("dbi.integrityError", NULL, NULL);
	if (DbiIntegrityError == NULL || PyDict_SetItemString(dict, "integrityError", DbiIntegrityError) == -1)
		RETURN_ERROR;
	DbiDataError = PyErr_NewException("dbi.dataError", NULL, NULL);
	if (DbiDataError == NULL || PyDict_SetItemString(dict, "dataError", DbiDataError) == -1)
		RETURN_ERROR;
	DbiInternalError = PyErr_NewException("dbi.internalError", NULL, NULL);
	if (DbiInternalError == NULL || PyDict_SetItemString(dict, "internalError", DbiInternalError) == -1)
		RETURN_ERROR;

#if (PY_VERSION_HEX >= 0x03000000)
	return module;
#endif
}

int dbiIsDate(const PyObject *o)
{
  return o->ob_type == &DbiDate_Type;
}

int dbiIsRaw(const PyObject *o)
{
  return o->ob_type == &DbiRaw_Type;
}

int dbiIsRowId(const PyObject *o)
{
	return o->ob_type == &DbiRowId_Type;
}
