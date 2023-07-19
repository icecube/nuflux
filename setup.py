import sys
import os
from setuptools import setup, Extension
import numpy

boost_python = 'boost_python'+str(sys.version_info.major)+str(sys.version_info.minor)
prefix = os.environ.get('PREFIX', '/usr/local')

extension = Extension(
    '_nuflux', [
     'src/library/logging.cpp',
     'src/library/SplineFlux.cpp',
     'src/library/SplineFlux2.cpp',
     'src/library/LegacyPromptFlux.cpp',
     'src/library/LegacyConventionalFlux.cpp',
     'src/library/ANFlux.cpp',
     'src/library/IPLEFlux.cpp',
     'src/library/FluxFunction.cpp',
     'src/pybindings/detail.cxx',
     'src/pybindings/module.cxx',
    ],
    include_dirs = ['src/include',numpy.get_include(),os.path.join(prefix,'include')],
    library_dirs = [os.path.join(prefix,'lib'),os.path.join(prefix,'lib64')],
    libraries = [boost_python,'photospline'],
    extra_compile_args = ['-std=c++11','-DUSE_NUMPY'],
    )

setup(
    ext_modules = [extension],
)
