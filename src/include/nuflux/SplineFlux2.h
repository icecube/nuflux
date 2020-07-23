#ifndef SPLINEFLUX2_H
#define SPLINEFLUX2_H

#include <photospline/splinetable.h>
#include <nuflux/nuflux.h>

namespace nuflux{

  class SplineFlux2 : public FluxFunction{
  private:
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> > components;
  public:
    SplineFlux2(const std::string& fluxName);
    static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);
    ///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
    virtual double getFlux(ParticleType type, double energy, double cosZenith) const;
    double readExtents(ParticleType type) const;
    bool PathExist(const std::string& name);
    double getMinEnergy() const;
    double getMaxEnergy() const;
  };

} //namespace nuflux

#endif //SPLINEFLUX2_H
