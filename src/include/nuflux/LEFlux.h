#ifndef LEFLUX_H
#define LEFLUX_H

#include <photospline/splinetable.h>
#include <nuflux/nuflux.h>

namespace nuflux{
  
  class LEFlux : public FluxFunction {
  public:
    LEFlux(const std::string& fluxName);
    static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);

    virtual double getFlux(ParticleType type, double energy, double cosZenith) const;
    
    // Computes the expected flux for neutrinos of the given type, energy, zenith angle and azimuth angle
    double getFlux(ParticleType type, double energy, double azimuth, double cosZenith) const;
    
  private:
    class Evaluator{
    private:
      photospline::splinetable<> data;
      Evaluator(const Evaluator&);
      Evaluator& operator=(const Evaluator&);
    public:
      Evaluator(const std::string&);
      ~Evaluator();
      size_t GetNDim() const { return data.get_ndim(); };
      std::pair<double, double> GetExtents(size_t dim) const;
      double operator()(double energy, double cosZenith) const;
      double operator()(double energy, double azimuth, double cosZenith) const;
    };
    
    std::map<ParticleType, boost::shared_ptr<Evaluator> > fluxes_;
  };

}

#endif // LEFLUX_H
