[![Tests](https://github.com/icecube/nuflux/actions/workflows/tests.yml/badge.svg?branch=main)](https://github.com/icecube/nuflux/actions/workflows/tests.yml)
[![codecov](https://codecov.io/gh/icecube/nuflux/branch/main/graph/badge.svg?token=O6P20YHAXH)](https://codecov.io/gh/icecube/nuflux)

# nuflux

Nuflux is a library for calculating atmospheric neutrino fluxes.

## Installation

### Packages

nuflux is packaged [on conda-forge](https://anaconda.org/conda-forge/nuflux). Install with `conda install -c conda-forge nuflux`.

### From source

nuflux uses two different build systems: one based on `setuptools` and one based on `meson`. Which one you should use depends on whether you need to build C++ libraries against nuflux, or are content to use the Python bindings.

#### Prerequisites

* cfitsio
* photospline
* boost_python

#### setuptools

If you only need to use python you can install it directly through pip:

    pip install --user git+https://github.com/icecube/nuflux

Note that this may fail if photospline and boost_python are installed in nonstandard locations, e.g. headers in /opt/toolsets/mystuff/include, libraries in /opt/toolsets/mystuff/lib. In this case, set the `PREFIX` environment variable to the root of your toolset installation:

    PREFIX=/opt/toolsets/mystuff pip install --user git+https://github.com/icecube/nuflux

If you are running it in IceCube's cvmfs environment you will want to set `PREFIX` to the base of the cvmfs envionment which is stored in the envionment variable `SROOT`:

    eval `/cvmfs/icecube.opensciencegrid.org/py3-v4.1.1/setup.sh`
    PREFIX=${SROOT} pip install --user git+https://github.com/icecube/nuflux

#### Meson

If you want to use nuflux from a c++ program you need to install it with meson

    git clone https://github.com/icecube/nuflux
    cd nuflux
    meson build
    ninja -C build
    sudo ninja -C build install

If boost and photospline are installed in nonstandard locations, you might need to let meson know with environment variables. Since photospline is detected with cmake you need to set `CMAKE_PREFIX_PATH=/path/to/photospline`. Boost uses `BOOST_ROOT`.

For example, in IceCube's cvmfs envionment you should do:

     CMAKE_PREFIX_PATH=${SROOT} BOOST_ROOT=${SROOT}//spack/opt/spack/linux-centos7-x86_64/gcc-9.2.0spack/boost-1.71.0-gcw4axdhsqe44hdnbqiwxgfiinffreka/ meson setup . --prefix=/path/to/instal/to

the option -Ddata_path can be used to install to a directory outside of the PREFIX directory.

If you want to install to a read-only file system you can pass -Dinstall_data=False

## Documentation

You can build the documentation yourself using sphinx:
    
    pip3 install sphinx

In the /nuflux/docs/ directory, run make with the kind of documentation you'd like, e.g.

    make html

Run "make help" to see all available documentation formats.

## Usage

From python:

    import nuflux
    flux = nuflux.makeFlux('honda2006')
    nu_type=nuflux.NuMu
    nu_energy=1e3 # in GeV
    nu_cos_zenith = 0.5
    print(flux.getFlux(nu_type,nu_energy,nu_cos_zenith))

from c++:

    #include <nuflux/nuflux.h>

    int main(){  
      boost::shared_ptr<nuflux::FluxFunction> flux;
      flux=nuflux::makeFlux("honda2006");
      double f=flux->getFlux(nuflux::NuMu,1e3,0);
      std::cout << f << '\n';
    }


