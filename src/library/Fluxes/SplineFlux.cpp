#include <fstream>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <nuflux/SplineFlux.h>

namespace NewNuFlux{
	
	SimpleSplineFlux::SimpleSplineFlux(const std::string& fluxName):
	FluxFunction(fluxName){

          components[I3Particle::NuMu]=boost::make_shared<photospline::splinetable<>>(detail::getDataPath("SplineFlux/"+fluxName+"_numu.fits"));
          components[I3Particle::NuMuBar]=boost::make_shared<photospline::splinetable<>>(detail::getDataPath("SplineFlux/"+fluxName+"_numubar.fits"));
          components[I3Particle::NuE]=boost::make_shared<photospline::splinetable<>>(detail::getDataPath("SplineFlux/"+fluxName+"_nue.fits"));
          components[I3Particle::NuEBar]=boost::make_shared<photospline::splinetable<>>(detail::getDataPath("SplineFlux/"+fluxName+"_nuebar.fits"));
	}
	
	boost::shared_ptr<FluxFunction> SimpleSplineFlux::makeFlux(const std::string& fluxName){
		return(boost::dynamic_pointer_cast<FluxFunction>(boost::make_shared<SimpleSplineFlux>(fluxName)));
	}
	
	double SimpleSplineFlux::getFlux(particleType type, double energy, double cosZenith) const{
		if(type==I3Particle::NuTau || type==I3Particle::NuTauBar)
			return(0);
		std::map<particleType,boost::shared_ptr<photospline::splinetable<>> >::const_iterator it=components.find(type);
		if(it==components.end())
			throw std::runtime_error(name+" does not support particle type "+boost::lexical_cast<std::string>(type));
		double coords[2]={log10(energy),cosZenith};
		int centers[2];
		it->second->searchcenters(coords,centers);
		double lf=it->second->ndsplineeval(coords,centers,0);
		return(pow(10,lf));
	}
	
} //namespace NewNuFlux

NNF_REGISTER_FLUX("BERSS_H3a_central",&NewNuFlux::SimpleSplineFlux::makeFlux);
NNF_REGISTER_FLUX("BERSS_H3p_lower",&NewNuFlux::SimpleSplineFlux::makeFlux);
NNF_REGISTER_FLUX("BERSS_H3p_central",&NewNuFlux::SimpleSplineFlux::makeFlux);
NNF_REGISTER_FLUX("BERSS_H3p_upper",&NewNuFlux::SimpleSplineFlux::makeFlux);
