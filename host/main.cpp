#include <nativelib.h>

int main(int argc, char *argv[])
{
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