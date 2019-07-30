from setuptools import setup, Extension , find_packages

extension = Extension(
    'nuflux._NuFlux',
    ['src/pybindings/module.cxx',
     'src/lib/particleType.cpp',
     'src/lib/logging.cpp',
     'src/lib/Fluxes/SplineFlux.cpp',
     'src/lib/Fluxes/LegacyPromptFlux.cpp',
     'src/lib/Fluxes/LegacyConventionalFlux.cpp',
     'src/lib/Fluxes/ANFlux.cpp',
     'src/lib/Fluxes/LEFlux.cpp',
     'src/lib/Fluxes/IPLEFlux.cpp',
     'src/lib/FluxFunction.cpp',
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
    
