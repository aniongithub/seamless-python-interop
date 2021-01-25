import sys
sys.path.insert(0, '')

from ctypes import *
from nativelib import *
from callbacks import *

def main():

    libHdl = NativeLibHdl()
    result = Init(byref(libHdl))
    if result != RESULT_OK:
        print("Could not initialize native lib!")

    print("Initialized nativelib!")
    callbackParams = CallbackParams()
    callbackParams.callbackType = CallbackType(PYTHON_CALLBACK)
    callbackParams.callbackName = String(b"callbacks.python_callback")

    callbacks = NativeCallbacks()
    result = Get_Callback(libHdl, callbackParams, byref(callbacks))
    if result != RESULT_OK:
        print("Could not call Get_Callback successfully!")

    print("Obtained callback successfully")

    result = Destroy(byref(libHdl))
    if result != RESULT_OK:
        print("Could not destroy native lib!")

    print("Destroyed nativelib!")

if __name__ == "__main__":
    main()