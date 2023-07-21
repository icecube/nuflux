import os
import sys

import numpy
from setuptools import Extension, setup

boost_python = "boost_python" + str(sys.version_info.major) + str(sys.version_info.minor)
prefix = os.environ.get("PREFIX", "/usr/local")

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
    include_dirs=["src/include", numpy.get_include(), os.path.join(prefix, "include")],
    library_dirs=[os.path.join(prefix, "lib"), os.path.join(prefix, "lib64")],
    libraries=[boost_python, "photospline"],
    extra_compile_args=["-std=c++20", "-DUSE_NUMPY"],
)

setup(
    packages=["nuflux"],
    ext_modules=[extension],
    package_data={"nuflux": ["data/*/*.fits", "data/*/*.dat"]},
)
