#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Native headers have enums
enum NativeResult_t
{
    RESULT_NOT_IMPLEMENTED = -2,
    RESULT_ERROR = -1,
    RESULT_OK = 0,
};
typedef enum NativeResult_t NativeResult;

// Example of an opaque struct, used often for hiding internal
// native implementation details from the consumer
typedef struct NativeLib* NativeLibHdl;

// Using a struct to contain our callback(s) as it's a 
// common pattern for (asynchronous) C interfaces
struct NativeCallbacks_t;
typedef struct NativeCallbacks_t NativeCallbacks;
typedef NativeCallbacks* NativeCallbacksHdl;

// Enum to let our library know how to find/populate
// the callback function pointer
enum CallbackType_t
{
    NATIVE, // Uses a simple dlsym lookup to find a function pointer
    PYTHON // Uses Cython to call into a Python callback
};
typedef enum CallbackType_t CallbackType;

struct CallbackParams_t
{
    const char* callbackName;
    CallbackType callbackType;
};
typedef struct CallbackParams_t CallbackParams;

#define EXPORT __attribute__((visibility("default")))

// Function to find a callback and store the native-callable function pointer in NativeCallbacksHdl
EXPORT NativeResult Get_Callback(NativeLibHdl libHandle, CallbackParams params, NativeCallbacksHdl callbacksHdl);

// Function that invokes the callback from the NativeCallbacksHdl provided
EXPORT NativeResult Call_Callback(NativeLibHdl libHandle, NativeCallbacksHdl callbacksHdl);

// Define our callback func here and use our opaque struct here
typedef NativeResult (*CallbackFunc)(NativeLibHdl libHandle);
struct NativeStruct
{
    CallbackFunc callback;    
};