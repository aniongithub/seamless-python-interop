#include <exception>

#include <nativelib.h>
#include <Python.h>

#include <cythoncallbacks.h>

#define MAGIC ((uint32_t)0xFFFF)

struct NativeLib
{
    uint32_t magic;
};

NativeResult Init(NativeLibHdl* libHdl)
{
    *libHdl = new NativeLib();
    (*libHdl)->magic = MAGIC;

    return RESULT_OK;
}

NativeResult Destroy(NativeLibHdl* libHdl)
{
    if (!libHdl ||
        (*libHdl)->magic != MAGIC)
        return RESULT_INVALID_ARGS;

    delete *libHdl;
    *libHdl = nullptr;
    
    return RESULT_OK;
}

NativeResult Get_Python_Callback(NativeLibHdl libHdl, CallbackParams params, NativeCallbacksHdl callbacksHdl)
{
    return Getcallbacks_Cython(libHdl, params, callbacksHdl);
}

NativeResult Get_Native_Callback(NativeLibHdl libHdl, CallbackParams params, NativeCallbacksHdl callbacksHdl)
{
    return RESULT_NOT_IMPLEMENTED;
}

NativeResult Get_Callback(NativeLibHdl libHdl, CallbackParams params, NativeCallbacksHdl callbacksHdl)
{
    if (!libHdl ||
        !callbacksHdl ||
        libHdl->magic != MAGIC)
        return RESULT_INVALID_ARGS;

    switch (params.callbackType)    {
    case PYTHON_CALLBACK:
        return Get_Python_Callback(libHdl, params, callbacksHdl);

    case NATIVE_CALLBACK:
        return Get_Native_Callback(libHdl, params, callbacksHdl);

    default:
        return RESULT_NOT_IMPLEMENTED;
    }
}

NativeResult Call_Callback(NativeLibHdl libHdl, NativeCallbacksHdl callbacksHdl)
{
    if (!libHdl ||
        !callbacksHdl ||
        !callbacksHdl->callback)
        return RESULT_INVALID_ARGS;
    CallbackFunc callback = *(callbacksHdl->callback);
    
    try
    {
        callback(libHdl);
        return RESULT_OK;
    }
    catch(const std::exception& e)
    {
        return RESULT_ERROR;
    }
}

static PyObject* cythoncallbacks;

__attribute__((constructor)) void libLoad()
{
    PyImport_AppendInittab("cythoncallbacks", PyInit_cythoncallbacks);
    Py_Initialize();
    cythoncallbacks = PyImport_ImportModule("cythoncallbacks");
}

__attribute__((destructor)) void libUnload()
{
    Py_Finalize();
}