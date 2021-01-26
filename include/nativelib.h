#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Native headers have enums
enum NativeResult
{
    RESULT_INVALID_ARGS = -3,
    RESULT_NOT_IMPLEMENTED = -2,
    RESULT_ERROR = -1,
    RESULT_OK = 0,
};
typedef enum NativeResult NativeResult;

// Example of an opaque struct, used often for hiding internal
// native impleentation details from the consumer
struct NativeLib;
typedef struct NativeLib NativeLib;
typedef NativeLib* NativeLibHdl;

// Using a struct to contain our callback(s) as it's a 
// common pattern for (asynchronous) C interfaces
struct NativeCallbacks;
typedef struct NativeCallbacks NativeCallbacks;
typedef NativeCallbacks* NativeCallbacksHdl;

// Enum to let our library know how to find/populate
// the callback function pointer
enum CallbackType
{
    NATIVE_CALLBACK, // Uses a simple dlsym lookup to find a function pointer
    PYTHON_CALLBACK // Uses Cython to call into a Python callback
};
typedef enum CallbackType CallbackType;

struct CallbackParams
{
    const char* callbackName;
    CallbackType callbackType;
};
typedef struct CallbackParams CallbackParams;

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORT __attribute__((visibility("default")))

EXPORT NativeResult Init(NativeLibHdl* libHdl);
EXPORT NativeResult Destroy(NativeLibHdl* libHdl);

// Function to find a callback and store the native-callable function pointer in NativeCallbacksHdl
EXPORT NativeResult Get_Callback(NativeLibHdl libHandle, CallbackParams params, NativeCallbacksHdl callbacksHdl);

// Function that invokes the callback from the NativeCallbacksHdl provided
EXPORT NativeResult Call_Callback(NativeLibHdl libHandle, NativeCallbacksHdl callbacksHdl);

#ifdef __cplusplus
}
#endif

// Define our callback func here
typedef void (*CallbackFunc)(NativeLibHdl libHandle);
typedef CallbackFunc* CallbackFuncHdl;

// Declare our forward declared struct now
struct NativeCallbacks
{
    CallbackFuncHdl callback;
};