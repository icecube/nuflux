#include <fstream>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <nuflux/SplineFlux.h>

namespace nuflux{

  double SimpleSplineFlux::getMinEnergy()const{
    return 1e2;
  }

  double SimpleSplineFlux::getMaxEnergy()const{
    return 1e9;
  }

  double SimpleSplineFlux::readExtents(ParticleType type)const{
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >
     ::const_iterator it=components.find(type);
    double le=it->second->lower_extent(0), ue=it->second->upper_extent(0);
    double lc=it->second->lower_extent(1), uc=it->second->upper_extent(1);
    std::cout << "Extents for dim energy:\t\t" << pow(10,le) << "\t" << pow(10,ue) << std::endl;
    std::cout << "Extents for dim cos(zenith):\t" << lc << "\t" << uc << std::endl;
    return 0;
  }

  SimpleSplineFlux::SimpleSplineFlux(const std::string& fluxName):
    FluxFunction(fluxName){

    components[NuMu]   =boost::make_shared<photospline::splinetable<>>(detail::getDataPath("SplineFlux/"+fluxName+"_numu.fits"));
    components[NuMuBar]=boost::make_shared<photospline::splinetable<>>(detail::getDataPath("SplineFlux/"+fluxName+"_numubar.fits"));
    components[NuE]    =boost::make_shared<photospline::splinetable<>>(detail::getDataPath("SplineFlux/"+fluxName+"_nue.fits"));
    components[NuEBar] =boost::make_shared<photospline::splinetable<>>(detail::getDataPath("SplineFlux/"+fluxName+"_nuebar.fits"));
  }

  boost::shared_ptr<FluxFunction> SimpleSplineFlux::makeFlux(const std::string& fluxName){
    return(boost::dynamic_pointer_cast<FluxFunction>(boost::make_shared<SimpleSplineFlux>(fluxName)));
  }

  double SimpleSplineFlux::getFlux(ParticleType type, double energy, double cosZenith) const{
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >::const_iterator it=components.find(type);
    if(it==components.end())
      throw std::runtime_error(name+" does not support particle type "+boost::lexical_cast<std::string>(type));

    // Warn the user about coordinates outside of physics extents:
    double le=it->second->lower_extent(0), ue=it->second->upper_extent(0);
    double lc=it->second->lower_extent(1), uc=it->second->upper_extent(1);
    if (!((energy-pow(10, ue)) * (energy-pow(10, le)) <= 0 )){
      return 0;
    }
    if (!( (lc <= cosZenith) && (cosZenith <= uc) )){
      return 0;
    }

    double coords[2]={log10(energy),cosZenith};
    int centers[2];
    it->second->searchcenters(coords,centers);
    double lf=it->second->ndsplineeval(coords,centers,0);
    return(pow(10,lf));
  }

} //namespace nuflux

NNF_REGISTER_FLUX("BERSS_H3a_central",&nuflux::SimpleSplineFlux::makeFlux);
NNF_REGISTER_FLUX("BERSS_H3p_lower",&nuflux::SimpleSplineFlux::makeFlux);
NNF_REGISTER_FLUX("BERSS_H3p_central",&nuflux::SimpleSplineFlux::makeFlux);
NNF_REGISTER_FLUX("BERSS_H3p_upper",&nuflux::SimpleSplineFlux::makeFlux);
