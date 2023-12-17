#ifndef LEGACYCONVENTIONALFLUX_H
#define LEGACYCONVENTIONALFLUX_H

#include <nuflux/nuflux.h>
#include <iosfwd>
#include <map>

namespace nuflux{

  class LegacyConventionalFlux : public FluxFunction{
  public:
    class component{
    public:
      ///A component for an equation like Gaisser 6.7 or 7.5
      struct progenitorComponent{
        double A,B,E_c;
        bool kaonic;
        //energy should be in GeV
        double operator()(double energy, double inclination) const;
      };
    private:
      ///These are the coefficients for the degree 5 polynomial in cos(zenith) and log10(energy)
      ///used for low energies
      double lowECoeffs[15];
      ///These are the maximum energies for which the low energy polynomial is to be used
      //for 4 predefined abs(cos(zenith)) bands: (.5,1], (.3,.5], (.1,.3], [0,.1]
      double lowECutoffs[4];
      ///These are (usually?) the contributions form pions and kaons
      progenitorComponent directComponents[2];
      ///Like the 'direct' components, but multiplied by the 'modification'
      progenitorComponent modifiedComponents[3];
      ///Base powerlaw index at high energies
      double highEIndex;
      ///Overall normalization factor
      double norm;
      ///The coefficients for the inclination correction to the cos(zenith)
      double inclinationCoeffs[5];
      ///Slight modifications to the normalization of the high energy calculation
      ///These correspond to the predefined cos(zenith) ranges:
      ///[0,.1), [.1,.2), [.2,.4), [.4,.8), [.8,1)
      double normTweaks[6];
      ///The normalization for the 'modification' term
      double modificationNorm;
      ///The scale log10(energy) for the 'modification' term
      double modificationScale;

      ///the factor by which to boost the pion decay contribution
      double pionAdjust;
      ///the factor by which to boost the kaon decay contribution
      double kaonAdjust;
      ///the additional normalization factor necessary to maintain the same
      ///total flux over all angles and energies after adjusting the pion/kaon ratio
      double pionKaonNorm;

      double inclination(double cosZenith) const;
      double transitionEnergy(double cosZenith) const;

      friend std::istream& operator>>(std::istream&,component&);
    public:
      //energy should be in GeV
      double getFlux(double energy, double cosZenith) const;

      void adjustPionKaonRatio(double pionAdjust, double kaonAdjust);
    };
    friend std::istream& operator>>(std::istream&,component&);
    friend component readConvComponent(const std::string& path);

    class kneeSpline{
    private:
      //knot locations and coefficients for a cubic spline
      //where the segment from x[i] to x[i+1], with x'=x-x[i] is
      // y = d[i]*x'^3 + c[i]*x'^2 + b[i]*x' + y[i]
      //knots correspond to log10 of neutrino energy in GeV;
      std::vector<double> x, y, b, c, d;
      friend std::istream& operator>>(std::istream&,kneeSpline&);
    public:
      ///Constructs a kneeSpline whose value is 1 for all energies
      kneeSpline();
      ///Computes the correction factor for this knee model, relative to the base model
      ///\param energy the neutrino energy in GeV
      double operator()(double energy) const;
    };
  private:
    std::map<ParticleType,component> components;

    kneeSpline kneeCorrection;
    ///the factor by which to boost the pion decay contribution
    double pionAdjust;
    ///the factor by which to boost the kaon decay contribution
    double kaonAdjust;
  public:
    LegacyConventionalFlux(const std::string& fluxName);
    static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);

    ///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
    virtual double getFlux(ParticleType type, double energy, double cosZenith) const;
    virtual double getMinEnergy() const;
    virtual double getMaxEnergy() const;

    ///boosts the contribution from pion decays
    ///\param adjust the factor by which to increase the pion contribution
    void setRelativePionContribution(double adjust);
    double getRelativePionContribution() const{ return(pionAdjust); }

    ///boosts the contribution from kaon decays
    ///\param adjust the factor by which to increase the kaon contribution
    void setRelativeKaonContribution(double adjust);
    double getRelativeKaonContribution() const{ return(kaonAdjust); }

    virtual void setKneeReweightingModel(std::string reweightModel);
  };

} //namespace nuflux

#endif //LEGACYCONVENTIONALFLUX_H
