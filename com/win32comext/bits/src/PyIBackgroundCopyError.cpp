// This file implements the IBackgroundCopyError Interface for Python.
// Generated by makegw.py

#include "bits_pch.h"
#include "PyIBackgroundCopyError.h"

// @doc - This file contains autoduck documentation
// ---------------------------------------------------
//
// Interface Implementation

PyIBackgroundCopyError::PyIBackgroundCopyError(IUnknown *pdisp):
	PyIUnknown(pdisp)
{
	ob_type = &type;
}

PyIBackgroundCopyError::~PyIBackgroundCopyError()
{
}

/* static */ IBackgroundCopyError *PyIBackgroundCopyError::GetI(PyObject *self)
{
	return (IBackgroundCopyError *)PyIUnknown::GetI(self);
}

// @pymethod |PyIBackgroundCopyError|GetError|Description of GetError.
PyObject *PyIBackgroundCopyError::GetError(PyObject *self, PyObject *args)
{
	IBackgroundCopyError *pIBCE = GetI(self);
	if ( pIBCE == NULL )
		return NULL;
	if ( !PyArg_ParseTuple(args, ":GetError") )
		return NULL;
	HRESULT hr;
	BG_ERROR_CONTEXT context;
	HRESULT hr_err;
	PY_INTERFACE_PRECALL;
	hr = pIBCE->GetError( &context, &hr_err);
	PY_INTERFACE_POSTCALL;
	if ( FAILED(hr) )
		return PyCom_BuildPyException(hr, pIBCE, IID_IBackgroundCopyError );
	return Py_BuildValue("lk", context, hr_err);
}

// @pymethod |PyIBackgroundCopyError|GetFile|Description of GetFile.
PyObject *PyIBackgroundCopyError::GetFile(PyObject *self, PyObject *args)
{
	IBackgroundCopyError *pIBCE = GetI(self);
	if ( pIBCE == NULL )
		return NULL;
	IBackgroundCopyFile *pVal;
	if ( !PyArg_ParseTuple(args, ":GetFile") )
		return NULL;
	HRESULT hr;
	PY_INTERFACE_PRECALL;
	hr = pIBCE->GetFile( &pVal );
	PY_INTERFACE_POSTCALL;
	if ( FAILED(hr) )
		return PyCom_BuildPyException(hr, pIBCE, IID_IBackgroundCopyError);
	return PyCom_PyObjectFromIUnknown(pVal, IID_IBackgroundCopyFile, FALSE);
}

// @pymethod |PyIBackgroundCopyError|GetErrorDescription|Description of GetErrorDescription.
PyObject *PyIBackgroundCopyError::GetErrorDescription(PyObject *self, PyObject *args)
{
	IBackgroundCopyError *pIBCE = GetI(self);
	if ( pIBCE == NULL )
		return NULL;
	// @pyparm int|LanguageId||Description for LanguageId
	LPWSTR pErrorDescription;
	DWORD LanguageId = 0;
	if ( !PyArg_ParseTuple(args, "|l:GetErrorDescription", &LanguageId) )
		return NULL;
	HRESULT hr;
	PY_INTERFACE_PRECALL;
	hr = pIBCE->GetErrorDescription( LanguageId, &pErrorDescription );
	PY_INTERFACE_POSTCALL;
	if ( FAILED(hr) )
		return PyCom_BuildPyException(hr, pIBCE, IID_IBackgroundCopyError );
	PyObject *ret = PyWinObject_FromWCHAR(pErrorDescription);
	CoTaskMemFree(pErrorDescription);
	return ret;
}

// @pymethod |PyIBackgroundCopyError|GetErrorContextDescription|Description of GetErrorContextDescription.
PyObject *PyIBackgroundCopyError::GetErrorContextDescription(PyObject *self, PyObject *args)
{
	IBackgroundCopyError *pIBCE = GetI(self);
	if ( pIBCE == NULL )
		return NULL;
	// @pyparm int|LanguageId||Description for LanguageId
	LPWSTR pContextDescription;
	DWORD LanguageId;
	if ( !PyArg_ParseTuple(args, "l:GetErrorContextDescription", &LanguageId) )
		return NULL;
	HRESULT hr;
	PY_INTERFACE_PRECALL;
	hr = pIBCE->GetErrorContextDescription( LanguageId, &pContextDescription );
	PY_INTERFACE_POSTCALL;
	if ( FAILED(hr) )
		return PyCom_BuildPyException(hr, pIBCE, IID_IBackgroundCopyError );
	PyObject *ret = PyWinObject_FromWCHAR(pContextDescription);
	CoTaskMemFree(pContextDescription);
	return ret;
}

// @pymethod |PyIBackgroundCopyError|GetProtocol|Description of GetProtocol.
PyObject *PyIBackgroundCopyError::GetProtocol(PyObject *self, PyObject *args)
{
	IBackgroundCopyError *pIBCE = GetI(self);
	if ( pIBCE == NULL )
		return NULL;
	LPWSTR pProtocol;
	if ( !PyArg_ParseTuple(args, ":GetProtocol") )
		return NULL;
	HRESULT hr;
	PY_INTERFACE_PRECALL;
	hr = pIBCE->GetProtocol( &pProtocol );
	PY_INTERFACE_POSTCALL;
	if ( FAILED(hr) )
		return PyCom_BuildPyException(hr, pIBCE, IID_IBackgroundCopyError );
	PyObject *ret = PyWinObject_FromWCHAR(pProtocol);
	CoTaskMemFree(pProtocol);
	return ret;
}

// @object PyIBackgroundCopyError|Description of the interface
static struct PyMethodDef PyIBackgroundCopyError_methods[] =
{
	{ "GetError", PyIBackgroundCopyError::GetError, 1 }, // @pymeth GetError|Description of GetError
	{ "GetFile", PyIBackgroundCopyError::GetFile, 1 }, // @pymeth GetFile|Description of GetFile
	{ "GetErrorDescription", PyIBackgroundCopyError::GetErrorDescription, 1 }, // @pymeth GetErrorDescription|Description of GetErrorDescription
	{ "GetErrorContextDescription", PyIBackgroundCopyError::GetErrorContextDescription, 1 }, // @pymeth GetErrorContextDescription|Description of GetErrorContextDescription
	{ "GetProtocol", PyIBackgroundCopyError::GetProtocol, 1 }, // @pymeth GetProtocol|Description of GetProtocol
	{ NULL }
};

PyComTypeObject PyIBackgroundCopyError::type("PyIBackgroundCopyError",
		&PyIUnknown::type,
		sizeof(PyIBackgroundCopyError),
		PyIBackgroundCopyError_methods,
		GET_PYCOM_CTOR(PyIBackgroundCopyError));
