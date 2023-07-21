[![Tests](https://github.com/icecube/nuflux/actions/workflows/tests.yml/badge.svg?branch=main)](https://github.com/icecube/nuflux/actions/workflows/tests.yml)
[![codecov](https://codecov.io/gh/icecube/nuflux/branch/main/graph/badge.svg?token=O6P20YHAXH)](https://codecov.io/gh/icecube/nuflux)
[![Conda-Forge](https://img.shields.io/conda/vn/conda-forge/nuflux?color=%2344A833&logo=condaforge)](https://anaconda.org/conda-forge/nuflux)
[![AUR](https://img.shields.io/aur/version/nuflux?color=%231793D1&logo=Arch%20Linux)](https://aur.archlinux.org/packages/nuflux/)
[![DOI](https://zenodo.org/badge/152797969.svg)](https://zenodo.org/badge/latestdoi/152797969)

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

     CMAKE_PREFIX_PATH=${SROOT} BOOST_ROOT=${SROOT} meson setup build . --prefix=/path/to/instal/to

the option -Ddata_path can be used to install to a directory outside of the PREFIX directory.

If you want to install to a read-only file system you can pass -Dinstall_data=False

You may also use the environment variable `NUFLUX_DATA` to specify the path of the nuflux data tables if they are in a
place other than where they were installed to.

## Documentation

You can build the documentation yourself using sphinx:

    pip3 install sphinx breathe sphinx_rtd_theme

Run ninja with docs/html target:

    ninja docs/html

The latest version of the documentation is automatically uploaded to [IceCube documentation server](https://docs.icecube.aq/nuflux/main)

## Usage

From python:

    >>> import nuflux
    >>> flux = nuflux.makeFlux('honda2006')
    >>> nu_type=nuflux.NuMu
    >>> nu_energy=1e3 # in GeV
    >>> nu_cos_zenith = 0.5
    >>> flux.getFlux(nu_type,nu_energy,nu_cos_zenith)
    2.2395566552347364e-11

from c++:

[comment]: # (BEGIN CODE)

    #include <nuflux/nuflux.h>

    int main(){  
      boost::shared_ptr<nuflux::FluxFunction> flux;
      flux=nuflux::makeFlux("honda2006");
      nuflux::ParticleType pdgid = nuflux::NuMu;
      double energy = 1e3; // in GeV
      double cos_zenith = 0;
      double f = flux->getFlux(pdgid, energy, cos_zenith);
      assert(f == 4.6476245267530432e-11);
    }

[comment]: # (END CODE)
