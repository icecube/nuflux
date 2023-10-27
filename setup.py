import os
import sys

import numpy
from setuptools import Extension, setup

boost_python = "boost_python" + str(sys.version_info.major) + str(sys.version_info.minor)
prefix = os.environ.get("PREFIX", "/usr/local")
prefixes = prefix.split(":")

extension = Extension(
    "_nuflux",
    [
        "src/library/logging.cpp",
        "src/library/SplineFlux.cpp",
        "src/library/SplineFlux2.cpp",
        "src/library/LegacyPromptFlux.cpp",
        "src/library/LegacyConventionalFlux.cpp",
        "src/library/ANFlux.cpp",
        "src/library/IPLEFlux.cpp",
        "src/library/FluxFunction.cpp",
        "src/pybindings/detail.cxx",
        "src/pybindings/module.cxx",
    ],
    include_dirs=["src/include", numpy.get_include()] + [os.path.join(p, "include") for p in prefixes],
    library_dirs=[os.path.join(p, "lib") for p in prefixes] + [os.path.join(p, "lib64") for p in prefixes],
    libraries=[boost_python, "photospline"],
    extra_compile_args=["-std=c++17", "-DUSE_NUMPY"],
)

headers = [
    "src/include/nuflux/ANFlux.h",
    "src/include/nuflux/FluxFunction.h",
    "src/include/nuflux/Fluxes.h",
    "src/include/nuflux/IPLEFlux.h",
    "src/include/nuflux/Interfaces.h",
    "src/include/nuflux/LegacyConventionalFlux.h",
    "src/include/nuflux/LegacyPromptFlux.h",
    "src/include/nuflux/SplineFlux.h",
    "src/include/nuflux/SplineFlux2.h",
    "src/include/nuflux/logging.h",
    "src/include/nuflux/nuflux.h",
]

setup(
    packages=["nuflux"],
    package_dir={"": "src"},
    ext_modules=[extension],
    package_data={"nuflux": ["data/*/*.fits", "data/*/*.dat"]},
)
