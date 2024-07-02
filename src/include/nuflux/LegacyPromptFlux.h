#ifndef LEGACYPROMPTFLUX_H
#define LEGACYPROMPTFLUX_H

#include <nuflux/nuflux.h>
#include <iosfwd>
#include <map>

namespace nuflux{

  class LegacyPromptFlux : public FluxFunction{
  private:
    class component{
    private:
      ///These are the coefficients for the degree 5 polynomial in cos(zenith) and log10(energy)
      double coeffs[15];
      friend std::istream& operator>>(std::istream&,component&);
    public:
      //The minimum energy for which this parameterization is considered valid
      double eMin;

      //energy should be in GeV
      double getFlux(double energy, double cosZenith) const;
    };
    friend std::istream& operator>>(std::istream&,component&);
    friend component readPromptComponent(const std::string& path);

    class kneeFunction{
    private:
      double c0, c1, x0, e0;
      friend std::istream& operator>>(std::istream&, kneeFunction&);
    public:
      kneeFunction();
      double operator()(double energy) const;
    };
    friend std::istream& operator>>(std::istream&, kneeFunction&);

    std::map<ParticleType,component> components;
    kneeFunction kneeCorrection;
  public:

    double getMinEnergy() const;
    double getMaxEnergy() const;

    LegacyPromptFlux(const std::string& fluxName);
    static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);

    ///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
    virtual double getFlux(ParticleType type, double energy, double cosZenith) const;

    virtual void setKneeReweightingModel(std::string reweightModel);
  };

} //namespace nuflux

#endif //LEGACYPROMPTFLUX_H
