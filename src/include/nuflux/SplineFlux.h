#ifndef SPLINEFLUX_H
#define SPLINEFLUX_H

#include <photospline/splinetable.h>
#include <nuflux/nuflux.h>

namespace NewNuFlux{
  
  class SimpleSplineFlux : public FluxFunction{
  private:
    std::map<particleType,boost::shared_ptr<photospline::splinetable<>> > components;
  public:
    SimpleSplineFlux(const std::string& fluxName);
    static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);
    ///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
    virtual double getFlux(particleType type, double energy, double cosZenith) const;
  };
  
} //namespace NewNuFlux

#endif //SPLINEFLUX_H
