import sys
sys.path.insert(0, '')

from ctypes import *
from nativelib import *

def python_callback(libHdl: NativeLibHdl) -> NativeResult:
    print("Hello from a python callback!")
    return NativeResult(RESULT_OK)