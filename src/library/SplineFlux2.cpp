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
// the readExtents() function. -----------------------------------------
  double SplineFlux2::getMinEnergy()const{
    return 9e-2;
  }
  double SplineFlux2::getMaxEnergy()const{
    return 4e10;
  }
// ---------------------------------------------------------------------

  double SplineFlux2::readExtents(ParticleType type)const{
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >
      ::const_iterator it=components.find(type);
    double low_energy=it->second->lower_extent(0), up_energy=it->second->upper_extent(0);
    double low_coszen=it->second->lower_extent(1), up_coszen=it->second->upper_extent(1);
    std::cout << "Extents for dim energy:\t\t" << pow(10,low_energy) << "\t" << pow(10,up_energy) << std::endl;
    low_coszen = -up_coszen;    // because data is symmetrical around cos(zenith)
    std::cout << "Extents for dim cos(zenith):\t" << low_coszen << "\t" << up_coszen << std::endl;
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

        for(std::size_t f=0; f<files.size(); f++){
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
    if((energy < getMinEnergy()) || (energy > getMaxEnergy())){
      return(0.0);
    }
    if((cosZenith < -1) || (cosZenith > +1)){
      return(0.0);
    }
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >
        ::const_iterator it=components.find(type);
    if(it==components.end()){
      if (isNeutrino(type)) {
        return 0;
      } else {
        throw std::runtime_error( name+" does not support particle type " +
                                  boost::lexical_cast<std::string>(type));
      }
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
