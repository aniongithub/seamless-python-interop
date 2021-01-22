#include <nativelib.h>

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

NativeResult Get_Callback(NativeLibHdl libHandle, CallbackParams params, NativeCallbacksHdl callbacksHdl)
{
    return RESULT_NOT_IMPLEMENTED;
}

NativeResult Call_Callback(NativeLibHdl libHandle, NativeCallbacksHdl callbacksHdl)
{
    return RESULT_NOT_IMPLEMENTED;
}