#include <fstream>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <nuflux/SplineFlux2.h>

namespace nuflux{

  inline bool SplineFlux2::PathExist(const std::string& name){
    // Test whether a file at the fiven path exists or not.
    if (FILE *file = fopen(name.c_str(), "r")) { fclose(file); return true; }
    else { return false; }
  }

// These two functions are only needed for the Python API. If you want
// to get the physics extents of each individual flux, please use
// the readExtents() function.
  double SplineFlux2::getMinEnergy()const{
    return 8e-2;
  }
  double SplineFlux2::getMaxEnergy()const{
    return 1e10;
  }

  double SplineFlux2::readExtents(ParticleType type)const{
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >
     ::const_iterator it=components.find(type);
    double le=it->second->lower_extent(0), ue=it->second->upper_extent(0);
    double lc=it->second->lower_extent(1), uc=it->second->upper_extent(1);
    std::cout << "Extents for dim energy:\t\t" << pow(10,le) << "\t" << pow(10,ue) << std::endl;
    lc = -uc;    // because data is symmetrical around cos(zenith)
    std::cout << "Extents for dim cos(zenith):\t" << lc << "\t" << uc << std::endl;
    return 0;
  }


  SplineFlux2::SplineFlux2(const std::string& fluxName):
    FluxFunction(fluxName){

        std::vector<ParticleType> particles{ NuE, NuEBar, NuMu, NuMuBar, NuTau, NuTauBar};
        std::vector<std::string> files{
            detail::getDataPath("SplineFlux2/"+fluxName+"_nue.fits"),
            detail::getDataPath("SplineFlux2/"+fluxName+"_nuebar.fits"),
            detail::getDataPath("SplineFlux2/"+fluxName+"_numu.fits"),
            detail::getDataPath("SplineFlux2/"+fluxName+"_numubar.fits"),
            detail::getDataPath("SplineFlux2/"+fluxName+"_nutau.fits"),
            detail::getDataPath("SplineFlux2/"+fluxName+"_nutaubar.fits")
        };

        for(int f=0; f<files.size(); f++){
            if (PathExist(files.at(f))){
                components[particles.at(f)] = boost::make_shared<photospline::splinetable<>>(files.at(f));
            }
        }
    }

  boost::shared_ptr<FluxFunction> SplineFlux2::makeFlux(const std::string& fluxName){
      return(boost::dynamic_pointer_cast<FluxFunction>(
          boost::make_shared<SplineFlux2>(fluxName)));
  }

  double SplineFlux2::getFlux(ParticleType type, double energy, double cosZenith) const{
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
    lc = -uc;    // because data is symmetrical around cos(zenith)
    if (!((energy-pow(10, ue)) * (energy-pow(10, le)) <= 0 )){
        std::cerr << "Warning: Chosen energy not within physics extents" << '\n';
    }
    if (!( (lc <= cosZenith) && (cosZenith <= uc) )){
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
NNF_REGISTER_FLUX("H3a_SIBYLL21_conv",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_k",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_K0",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_K0L",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_K0S",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_pi",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL21_mu",&nuflux::SplineFlux2::makeFlux);
//
NNF_REGISTER_FLUX("H3a_SIBYLL23C",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_pr",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_conv",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_k",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_K0",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_K0L",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_K0S",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_pi",&nuflux::SplineFlux2::makeFlux);
NNF_REGISTER_FLUX("H3a_SIBYLL23C_mu",&nuflux::SplineFlux2::makeFlux);
//
