.. _Usage:

Usage
############

Here are two very short examples of how to use nuflux in your own project.
The below code snippets will calculate a flux value for a given model,
particle type, neutrino energy and cosine zenith angle.


In Python
---------

::

  import nuflux
  flux = nuflux.makeFlux('honda2006')
  nu_type=nuflux.NuMu
  nu_energy=1e3 # in GeV
  nu_cos_zenith = 0.5
  print(flux.getFlux(nu_type,nu_energy,nu_cos_zenith))

In C++
------

::

  #include <nuflux/nuflux.h>

  int main(){
    boost::shared_ptr<nuflux::FluxFunction> flux;
    flux=nuflux::makeFlux("honda2006");
    double f=flux->getFlux(nuflux::NuMu,1e3,0);
    std::cout << f << std::endl;
  }
