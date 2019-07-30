from setuptools import setup, Extension , find_packages

extension = Extension(
    'nuflux._NuFlux',
    ['src/pybindings/module.cxx',
     'src/library/particleType.cpp',
     'src/library/logging.cpp',
     'src/library/Fluxes/SplineFlux.cpp',
     'src/library/Fluxes/LegacyPromptFlux.cpp',
     'src/library/Fluxes/LegacyConventionalFlux.cpp',
     'src/library/Fluxes/ANFlux.cpp',
     'src/library/Fluxes/LEFlux.cpp',
     'src/library/Fluxes/IPLEFlux.cpp',
     'src/library/FluxFunction.cpp',
    ],
    include_dirs=['src/include'],
    libraries=['hdf5','boost_python27','photospline'],
    extra_compile_args=['-std=c++11'],
    )

setup(
    name="NuFlux",
    version = "0.0.1",
    package_data={"nuflux":["data/*.fits",'data/*.dat']},
    packages=find_packages(),
    ext_modules=[extension],

    zip_safe=False
    )
    
