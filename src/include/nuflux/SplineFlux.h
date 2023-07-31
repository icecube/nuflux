#ifndef SPLINEFLUX_H
#define SPLINEFLUX_H

#include <photospline/splinetable.h>
#include <nuflux/nuflux.h>

namespace nuflux{

  class SimpleSplineFlux : public FluxFunction{
  private:
    std::map<ParticleType,std::shared_ptr<photospline::splinetable<>> > components;
  public:
    SimpleSplineFlux(const std::string& fluxName);
    static std::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);
    ///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
    virtual double getFlux(ParticleType type, double energy, double cosZenith) const;
    double getMinEnergy() const;
    double getMaxEnergy() const;
    double readExtents(ParticleType type) const;

  };

} //namespace nuflux

#endif //SPLINEFLUX_H
