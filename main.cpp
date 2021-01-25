#include <nativelib.h>

#include <cstdlib>
#include <string>
#include <regex>

#include <boost/python.hpp>
#include <boost/filesystem.hpp>

// Declare our forward declared struct now
struct NativeCallbacks
{
    std::function<NativeResult(NativeLibHdl)> callback = nullptr;

    NativeResult Invoke(NativeLibHdl libHdl)
    {
        if (!callback)
            return RESULT_INVALID_ARGS;
        return callback(libHdl);
    }
};

namespace python = boost::python;

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

std::vector<std::string> split(const std::string & s, std::string rgx_str = "\\s+") 
{
    std::vector<std::string> elems;
    std::regex rgx (rgx_str);

    std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
    std::sregex_token_iterator end;

    while (iter != end)  {
        //std::cout << "S43:" << *iter << std::endl;
        elems.push_back(*iter);
        ++iter;
    }

    return elems;
}

NativeResult Get_Python_Callback(CallbackParams params, NativeCallbacksHdl callbacksHdl)
{   
    // We assume that the last part of the name is the module function name
    auto parts = split(params.callbackName, "\\.");
    
    // We need at least the module name and a function name
    if (parts.size() < 2)
        return RESULT_INVALID_ARGS;
    
    auto moduleFuncName = parts[parts.size() - 1];

    // Assemble the module name
    std::string moduleName = "";
    for (auto i = 0; i < parts.size() - 1; i++)
    {
        moduleName += parts[i];
        if (i < parts.size() - 2)
            moduleName += ".";
    }

    // Do we need this?
    std::string moduleFileName = "";
    for (auto i = 0; i < parts.size() - 1; i++)
    {
        moduleFileName += parts[i];
        if (i < parts.size() - 2)
            moduleFileName += "/";
    }
    moduleFileName += ".py";

    printf("Module name: %s, Module filename: %s, Function name: %s\n", 
        moduleName.c_str(), 
        moduleFileName.c_str(),
        moduleFuncName.c_str());
    
    // Try to load the module
    try
    {
        python::object python_module = python::import(moduleName.c_str());        
        std::stringstream ss;
        auto cmd = "from " + moduleName + " import " + moduleFuncName;
        auto python_callback = python::exec(cmd.c_str());
        
        // Copy the std::function for invocation when Call_Callback is called
        callbacksHdl->callback = [=](NativeLibHdl libHdl) -> NativeResult
        {
            try
            {
                // Instantiate the module method
                auto result = python_callback(libHdl);
                // python_module.attr(moduleFuncName.c_str())(libHdl);
                return python::extract<NativeResult>(result);
            }
            catch(const python::error_already_set&)
            {
                PyErr_Print();
                return RESULT_ERROR;
            }
        };
    }
    catch(const python::error_already_set&)
    {
        PyErr_Print();
        return RESULT_ERROR;
    }

    // Everything went OK
    return RESULT_OK;
}

NativeResult Get_Native_Callback(CallbackParams params, NativeCallbacksHdl callbacksHdl)
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
        return Get_Python_Callback(params, callbacksHdl);

    case NATIVE_CALLBACK:
        return Get_Native_Callback(params, callbacksHdl);

    default:
        return RESULT_NOT_IMPLEMENTED;
    }
}

NativeResult Call_Callback(NativeLibHdl libHdl, NativeCallbacksHdl callbacksHdl)
{
    if (!libHdl ||
        !callbacksHdl)
        return RESULT_INVALID_ARGS;

    return callbacksHdl->Invoke(libHdl);
}

int main(int argc, char *argv[])
{
    Py_Initialize();

    NativeLibHdl libHdl;
    auto result = Init(&libHdl);
    if (result != RESULT_OK)
        return result;

    CallbackParams params;
    params.callbackType = PYTHON_CALLBACK;
    params.callbackName = "callbacks.python_callback";

    NativeCallbacks callbacks;
    result = Get_Callback(libHdl, params, &callbacks);
    if (result != RESULT_OK)
        return result;

    result = Call_Callback(libHdl, &callbacks);
    if (result != RESULT_OK)
        return result;

    result = Destroy(&libHdl);
    if (result != RESULT_OK)
        return result;

    return RESULT_OK;
}