#include <fstream>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <nuflux/NewSplineFlux.h>

namespace nuflux{

  double NewSplineFlux::getMinEnergy()const{
    return 9.0e-2;
  }

  double NewSplineFlux::getMaxEnergy()const{
    return 8.9e10;
  }

  inline bool NewSplineFlux::testExist(const std::string& name){
    if (FILE *file = fopen(name.c_str(), "r")) { fclose(file); return true; }
    else { return false; }
  }

  NewSplineFlux::NewSplineFlux(const std::string& fluxName):
    FluxFunction(fluxName){

        components[NuMu] =boost::make_shared<photospline::splinetable<>>(
            detail::getDataPath("NewSplineFlux/"+fluxName+"_numu.fits"));
        components[NuMuBar] =boost::make_shared<photospline::splinetable<>>(
            detail::getDataPath("NewSplineFlux/"+fluxName+"_numubar.fits"));
        components[NuE] =boost::make_shared<photospline::splinetable<>>(
            detail::getDataPath("NewSplineFlux/"+fluxName+"_nue.fits"));
        components[NuEBar] =boost::make_shared<photospline::splinetable<>>(
            detail::getDataPath("NewSplineFlux/"+fluxName+"_nuebar.fits"));

        // Check if tau files are available for the requested flux, and if so, load them.
        std::string taufile = detail::getDataPath("NewSplineFlux/"+fluxName+"_nutau.fits");
        if (testExist(taufile)) {
            components[NuTau]    =boost::make_shared<photospline::splinetable<>>(
                detail::getDataPath("NewSplineFlux/"+fluxName+"_nutau.fits"));
            components[NuTauBar] =boost::make_shared<photospline::splinetable<>>(
                detail::getDataPath("NewSplineFlux/"+fluxName+"_nutaubar.fits"));
            taufail = false;
        }
        else {
            taufail = true;
        }
    }

  boost::shared_ptr<FluxFunction> NewSplineFlux::makeFlux(const std::string& fluxName){
      return(boost::dynamic_pointer_cast<FluxFunction>(
          boost::make_shared<NewSplineFlux>(fluxName)));
  }

  double NewSplineFlux::getFlux(ParticleType type, double energy, double cosZenith) const{
    // If tau flavor is not supported (e.g. for conventional fluxes), return 0.
    if ((type == NuTau || type == NuTauBar) && taufail){ return(0); }

    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >
        ::const_iterator it=components.find(type);
    if(it==components.end())
      throw std::runtime_error(
        name+" does not support particle type "
        +boost::lexical_cast<std::string>(type)
      );
    double coords[2]={log10(energy),cosZenith};
    int centers[2];
    it->second->searchcenters(coords,centers);
    double lf=it->second->ndsplineeval(coords,centers,0);
    return(pow(10,lf));
  }

} //namespace nuflux

NNF_REGISTER_FLUX("H3a_SIBYLL21",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_total",&nuflux::NewSplineFlux::makeFlux);
// Note that H3a_SIBYLL21 and H3a_SIBYLL21_total are identical.
NNF_REGISTER_FLUX("H3a_SIBYLL21_prompt",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_conv",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_k",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_pi",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_total",&nuflux::NewSplineFlux::makeFlux);
// Note that H3a_SIBYLL23C and H3a_SIBYLL23C_total are identical.
NNF_REGISTER_FLUX("H3a_SIBYLL23C_prompt",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_conv",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_k",&nuflux::NewSplineFlux::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_pi",&nuflux::NewSplineFlux::makeFlux);
//
