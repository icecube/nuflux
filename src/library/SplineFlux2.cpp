#include <fstream>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <nuflux/SplineFlux2.h>

namespace nuflux{

  bool tauexist = false;
  inline bool SplineFlux2::PathExist(const std::string& name){
    // Test whether a file at the fiven path exists or not.
    if (FILE *file = fopen(name.c_str(), "r")) { fclose(file); return true; }
    else { return false; }
  }

  double SplineFlux2::readExtents(ParticleType type)const{
    if ((type == NuTau || type == NuTauBar) && !tauexist){
        std::cout << "physics extents not available for this particle type" << std::endl;
        return 0;
    }
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >
     ::const_iterator it=components.find(type);
    double le=it->second->lower_extent(0), ue=it->second->upper_extent(0);
    double lc=it->second->lower_extent(1), uc=it->second->upper_extent(1);
    std::cout << "Extents for dim energy:\t\t" << pow(10,le) << "\t" << pow(10,ue) << std::endl;
    // Because data is symmetrical around cos(zenith):
    // std::cout << "Extents for dim cos(zenith):\t" << lc << "\t" << uc << std::endl;
    std::cout << "Extents for dim cos(zenith):\t" << -uc << "\t" << uc << std::endl;
    return 0;
  }


  SplineFlux2::SplineFlux2(const std::string& fluxName):
    FluxFunction(fluxName){

        components[NuMu] =boost::make_shared<photospline::splinetable<>>(
            detail::getDataPath("SplineFlux2/"+fluxName+"_numu.fits"));
        components[NuMuBar] =boost::make_shared<photospline::splinetable<>>(
            detail::getDataPath("SplineFlux2/"+fluxName+"_numubar.fits"));
        components[NuE] =boost::make_shared<photospline::splinetable<>>(
            detail::getDataPath("SplineFlux2/"+fluxName+"_nue.fits"));
        components[NuEBar] =boost::make_shared<photospline::splinetable<>>(
            detail::getDataPath("SplineFlux2/"+fluxName+"_nuebar.fits"));

        // Check if tau files are available for the requested flux, and if so, load them.
        // bool taufail = false;
        std::string taufile = detail::getDataPath("SplineFlux2/"+fluxName+"_nutau.fits");
        if (PathExist(taufile)) {
            components[NuTau]    =boost::make_shared<photospline::splinetable<>>(
                detail::getDataPath("SplineFlux2/"+fluxName+"_nutau.fits"));
            components[NuTauBar] =boost::make_shared<photospline::splinetable<>>(
                detail::getDataPath("SplineFlux2/"+fluxName+"_nutaubar.fits"));
            tauexist = true;
        }
        else {
            tauexist = false;
        }
    }

  boost::shared_ptr<FluxFunction> SplineFlux2::makeFlux(const std::string& fluxName){
      return(boost::dynamic_pointer_cast<FluxFunction>(
          boost::make_shared<SplineFlux2>(fluxName)));
  }

  double SplineFlux2::getFlux(ParticleType type, double energy, double cosZenith) const{
    // If tau flavor is not supported (e.g. for conventional fluxes), return 0.
    if ((type == NuTau || type == NuTauBar) && !tauexist){ return(0); }

    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >
        ::const_iterator it=components.find(type);
    if(it==components.end())
      throw std::runtime_error(
        name+" does not support particle type "
        +boost::lexical_cast<std::string>(type)
      );

    // Warn the user about coordinates outside of physics extents:
    double le=it->second->lower_extent(0), ue=it->second->upper_extent(0);
    double lc=it->second->lower_extent(1), uc=it->second->upper_extent(1);
    if (!((energy-pow(10, ue)) * (energy-pow(10, le)) <= 0 )){
        std::cerr << "Warning: Chosen energy not within physics extents" << '\n';
    }
    if (!( (-uc <= cosZenith) && (cosZenith <= uc) )){
        std::cerr << "Warning: Chosen cos(zenith) not within physics extents" << '\n';
    }

    double coords[2]={log10(energy),std::abs(cosZenith)};
    int centers[2];
    it->second->searchcenters(coords,centers);
    double lf=it->second->ndsplineeval(coords,centers,0);
    return(pow(10,lf));
  }



} //namespace nuflux

NNF_REGISTER_FLUX("H3a_SIBYLL21",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_total",&nuflux::SplineFlux2::makeFlux);
// Note that H3a_SIBYLL21 and H3a_SIBYLL21_total are identical.
NNF_REGISTER_FLUX("H3a_SIBYLL21_prompt",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_conv",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_k",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_K0",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_pi",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_mu",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_total",&nuflux::SplineFlux2::makeFlux);
// Note that H3a_SIBYLL23C and H3a_SIBYLL23C_total are identical.
NNF_REGISTER_FLUX("H3a_SIBYLL23C_prompt",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_conv",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_k",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_K0",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_pi",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_mu",&nuflux::SplineFlux2::makeFlux);
//
