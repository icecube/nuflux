# nuflux

Nuflux is a library for calculating atmospheric neutrino fluxes.

## Prerequisites

* cfitsio
* phosospline
* boost_python

## Installation

If you only need to use python you can install it directly through pip:

    pip install --user git+https://github.com/IceCubeOpenSource/nuflux

If you want to use nuflux from a c++ program you need to install it with meson

    git clone https://github.com/IceCubeOpenSource/nuflux
    cd nuflux
    meson build
    ninja -C build
    sudo ninja -C build install

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


