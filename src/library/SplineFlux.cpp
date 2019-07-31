#include <fstream>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <nuflux/SplineFlux.h>

namespace nuflux{
  
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
    if(type==NuTau || type==NuTauBar)
      return(0);
    std::map<ParticleType,boost::shared_ptr<photospline::splinetable<>> >::const_iterator it=components.find(type);
    if(it==components.end())
      throw std::runtime_error(name+" does not support particle type "+boost::lexical_cast<std::string>(type));
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
