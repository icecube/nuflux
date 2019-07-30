import sys
import importlib

cmodule = importlib.import_module("._NuFlux","nuflux")
thismod = sys.modules[__name__]
for key,value in cmodule.__dict__.items():
    if not key.startswith("_"):
        thismod.__dict__[key] = value

del sys,importlib,cmodule,thismod,key,value



