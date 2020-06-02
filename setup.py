import sys
import os
from setuptools import setup, Extension ,find_packages
import numpy

boost_python = 'boost_python'+str(sys.version_info.major)+str(sys.version_info.minor)
prefix = os.environ.get('PREFIX', '/usr/local')

extension = Extension(
    'nuflux._nuflux',
    ['src/library/logging.cpp',
     'src/library/SplineFlux.cpp',
     'src/library/LegacyPromptFlux.cpp',
     'src/library/LegacyConventionalFlux.cpp',
     'src/library/ANFlux.cpp',
     'src/library/IPLEFlux.cpp',
     'src/library/FluxFunction.cpp',
     'src/pybindings/detail.cxx',     
     'src/pybindings/module.cxx',
    ],
    include_dirs=['src/include',numpy.get_include(),os.path.join(prefix,'include')],
    library_dirs=[os.path.join(prefix,'lib'),os.path.join(prefix,'lib64')],
    libraries=[boost_python,'photospline'],
    extra_compile_args=['-std=c++11','-DUSE_NUMPY'],
    )

setup(
    name="nuflux",
    version = "0.0.1",
    packages=find_packages(),
    ext_modules=[extension],    
    package_data={"nuflux":["data/*/*.fits",'data/*/*.dat']},
    test_suite="tests.test_fluxes",
    zip_safe=False,
    keywords = [
        'neutrino', 'cosmic rays','atmospheric neutrinos',
    ],
    classifiers = [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'Operating System :: MacOS :: MacOS X',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: C++',
        'Programming Language :: Python',
        'Topic :: Scientific/Engineering :: Astronomy',
        'Topic :: Scientific/Engineering :: Physics',
    ],   
)
    
