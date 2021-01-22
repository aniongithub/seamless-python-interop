import sys
sys.path.insert(0, '')

from ctypes import *
import nativelib as n

libHdl = n.NativeLibHdl()
result = n.Init(byref(libHdl))
if result != n.RESULT_OK:
    print("Could not initialize native lib!")

print("Initialized nativelib!")

result = n.Destroy(byref(libHdl))
if result != n.RESULT_OK:
    print("Could not destroy native lib!")

print("Destroyed nativelib!")