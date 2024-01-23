#ifndef ANFLUX_H
#define ANFLUX_H

#include <photospline/splinetable.h>
#include <nuflux/nuflux.h>

namespace nuflux{

  class ANFlux : public FluxFunction {
  public:
    ANFlux(const std::string& fluxName);
    static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);

    ///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
    virtual double getFlux(ParticleType type, double energy, double cosZenith) const;
    double getMinEnergy() const;
    double getMaxEnergy() const;

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
    };

    double emin_,emax_;
    std::map<ParticleType, boost::shared_ptr<Evaluator> > fluxes_;
  };

}

#endif // ANFLUX_H
