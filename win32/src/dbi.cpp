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
	PyObject_Del(self);
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

#define delg(a) dbiValue(a)->ob_type->tp_as_number

static PyObject* dt_nb_add(PyObject* a, PyObject* b)
{
	return delg(a)->nb_add(dbiValue(a),b);
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

static PyObject *makeRaw(PyObject *self, PyObject *args)
{
  return makeDbiTypeTP(args, &DbiRaw_Type);
}

static PyObject *makeDbiType(PyObject *o, PyTypeObject *ty)
{
  DbiContainer *dbt = PyObject_New(DbiContainer, ty);
  dbt->objectOf = o;
  return (PyObject*) dbt;
}

PyObject *dbiMakeRaw(PyObject *contents)
{
  return makeDbiType(contents, &DbiRaw_Type);
}

static PyMethodDef globalMethods[] =
{
	// { "dbDate", makeDate, 1} ,
	// { "dbiDate", makeDate, 1} ,
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

	if (PyType_Ready(&DbiRaw_Type) == -1)
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

int dbiIsRaw(const PyObject *o)
{
  return o->ob_type == &DbiRaw_Type;
}
