from setuptools import setup, Extension ,find_packages
import numpy

extension = Extension(
    'nuflux._nuflux',
    ['src/library/logging.cpp',
     'src/library/SplineFlux.cpp',
     'src/library/LegacyPromptFlux.cpp',
     'src/library/LegacyConventionalFlux.cpp',
     'src/library/ANFlux.cpp',
     'src/library/LEFlux.cpp',
     'src/library/IPLEFlux.cpp',
     'src/library/FluxFunction.cpp',
     'src/pybindings/module.cxx',
    ],
    include_dirs=['src/include',numpy.get_include()],
    libraries=['boost_python27','photospline'],
    extra_compile_args=['-std=c++11','-DUSE_NUMPY'],
    )

setup(
    name="nuflux",
    version = "0.0.1",
    packages=find_packages(),
    ext_modules=[extension],    
    package_data={"nuflux":["data/*/*.fits",'data/*/*.dat']},
    test_suite="tests.test_fluxes",
    zip_safe=False
    )
    
