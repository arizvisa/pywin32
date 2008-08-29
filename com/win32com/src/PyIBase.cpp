#include "stdafx.h"
#include "PythonCOM.h"

PyIBase::PyIBase()
{
	_Py_NewReference(this);
}
PyIBase::~PyIBase()
{
}

/*static*/BOOL PyIBase::is_object(PyObject *ob, PyComTypeObject *which)
{
	return PyObject_IsInstance(ob, (PyObject *)which);
}

BOOL PyIBase::is_object(PyComTypeObject *which)
{
	return is_object(this,which);
}

/*static*/PyObject *
PyIBase::getattro(PyObject *self, PyObject *name)
{
	/*
	if (PyString_Check(name)) {
		PyObject *rc = ((PyIBase *)self)->getattr(PyString_AsString(name));
		if (rc)
			return rc;
		PyErr_Clear();
	}
	*/
	// Using PyObject_GenericGetAttr allows some special type magic
	// (ie, 
	return PyObject_GenericGetAttr(self, name);
}

PyObject *
PyIBase::getattr(char *name)
{
	return PyObject_GetAttrString(this, name);
}

/*static*/int PyIBase::setattro(PyObject *op,PyObject *obname, PyObject *v)
{
	char *name=PYWIN_ATTR_CONVERT(obname);
	if (name==NULL)
		return -1;
	PyIBase* bc = (PyIBase *)op;
	return bc->setattr(name,v);
}

int PyIBase::setattr(char *name, PyObject *v)
{
	char buf[128];
	sprintf(buf, "%s has read-only attributes", ob_type->tp_name );
	PyErr_SetString(PyExc_TypeError, buf);
	return -1;
}

/*static*/ PyObject *
PyIBase::repr( PyObject *ob )
{
	return ((PyIBase *)ob)->repr();
}
PyObject * PyIBase::repr()
{
	TCHAR buf[80];
	wsprintf(buf, _T("<%hs at %p>"),ob_type->tp_name, (PyObject *)this);
	return PyWinObject_FromTCHAR(buf);
}

/*static*/ void PyIBase::dealloc(PyObject *ob)
{
	delete (PyIBase *)ob;
}

/*static*/ int PyIBase::cmp(PyObject *ob1, PyObject *ob2)
{
	return ((PyIBase *)ob1)->compare(ob2);
}

