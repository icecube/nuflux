#ifndef NEWSPLINEFLUX_H
#define NEWSPLINEFLUX_H

#include <photospline/splinetable.h>
#include <nuflux/nuflux.h>

namespace nuflux{

  class NewSplineFlux : public FluxFunction{
  private:
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> > components;
  public:
    NewSplineFlux(const std::string& fluxName);
    static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);
    ///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
    virtual double getFlux(ParticleType type, double energy, double cosZenith) const;
    double getMinEnergy() const;
    double getMaxEnergy() const;
    bool testExist(const std::string& name);
    bool taufail = false;
  };

} //namespace nuflux

#endif //NEWSPLINEFLUX_H
