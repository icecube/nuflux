#include <boost/make_shared.hpp>
#include <boost/math/constants/constants.hpp>

#include "nuflux/LEFlux.h"

namespace nuflux {

  LEFlux::LEFlux(const std::string& fluxName) : FluxFunction(fluxName)
  {
    fluxes_[NuMu]    = boost::make_shared<Evaluator>(detail::getDataPath("LEFlux/"+fluxName+"_numu.fits"));
    fluxes_[NuMuBar] = boost::make_shared<Evaluator>(detail::getDataPath("LEFlux/"+fluxName+"_numubar.fits"));

    fluxes_[NuE]    = boost::make_shared<Evaluator>(detail::getDataPath("LEFlux/"+fluxName+"_nue.fits"));
    fluxes_[NuEBar] = boost::make_shared<Evaluator>(detail::getDataPath("LEFlux/"+fluxName+"_nuebar.fits"));
  }

  boost::shared_ptr<FluxFunction>
  LEFlux::makeFlux(const std::string& fluxName)
  {
    return(boost::dynamic_pointer_cast<FluxFunction>(boost::make_shared<LEFlux>(fluxName)));
  }

  double
  LEFlux::getFlux(ParticleType type, double energy, double cosZenith) const
  {
    std::map<ParticleType, boost::shared_ptr<Evaluator> >::const_iterator f = fluxes_.find(type);
    if (f == fluxes_.end()){
      return 0.0;
    }
    else{
      return (*(f->second))(energy, cosZenith);
    }
  }

  double
  LEFlux::getFlux(ParticleType type, double energy, double azimuth, double cosZenith) const
  {

    std::map<ParticleType, boost::shared_ptr<Evaluator> >::const_iterator f = fluxes_.find(type);

    if (f == fluxes_.end()){
      return 0.0;
    }
    else
      return (*(f->second))(energy, azimuth, cosZenith);
  }

  LEFlux::Evaluator::Evaluator(const std::string& fname):data(fname){}
  LEFlux::Evaluator::~Evaluator(){}

  std::pair<double, double>
  LEFlux::Evaluator::GetExtents(size_t dim) const
  {
    if(dim >= data.get_ndim())
      throw std::out_of_range("Dimension index out of range");
    return(std::make_pair(data.lower_extent(dim), data.upper_extent(dim)));
  }

  // The Honda Splines are done as (cosZen, azimuth (radians), energy (GeV)), so the
  // extents have to match. At energies about > 5 GeV, there is almost no azimuthal
  // flux dependence, but the HONDA tables are splined in cosZenith, azimuth, and energy.
  // Thus, use a generic value of 15 degrees for azimuth. 
  
  double
  LEFlux::Evaluator::operator()(double energy, double cosZenith) const
  {
    double coords[3];
  
    double azimuth = 15 / (boost::math::constants::pi<double>()/180.);
  
    // Extrapolate with a constant outside the valid zenith and azimuth range
    coords[0] = std::max(GetExtents(0).first, std::min(cosZenith, GetExtents(0).second));
    coords[1] = std::max(GetExtents(1).first, std::min(std::abs(azimuth), GetExtents(1).second));
  
    coords[2] = energy;

    // Set flux to 0 outside the valid energy range
    if (coords[2] <= GetExtents(2).first || coords[2] > GetExtents(2).second){
      log_warn ("Outside valid energy range for the chosen 2-dimensional flux model");
      return 0.0;
    }
    
    // Throw a warning if the user is requesting a flux using the cosZenith and energy,
    // while it would be better to also include any azimuthal dependency.

    if (coords[2] <= 2){
      log_warn ("Below 2 GeV, the geomagnetic field can cause azimuth dependent flux differences. It is best to switch to the GetFlux() method which also takes into account azimuth");
    }
  
    int centers[3];
    if(data.searchcenters(coords, centers) == 0){
      double result = data.ndsplineeval(coords, centers, 0);
      return result;
    }
  
    return(0.0);
  }

  
  // The Honda Spline is done in terms of azimuth in degrees and cos(zenith)
  // but the Evaluator is case in terms in the particle values in radians

  double
  LEFlux::Evaluator::operator()(double energy, double azimuth, double cosZenith) const
  {
    // The Honda Splines are done as (cosZen, azimuth (radians), energy (GeV)), so the extents have to match
  
    double coords[3];
  
    azimuth = azimuth / (boost::math::constants::pi<double>()/180.);
  
    // Extrapolate with a constant outside the valid zenith and azimuth range
    coords[0] = std::max(GetExtents(0).first, std::min(cosZenith, GetExtents(0).second));
    coords[1] = std::max(GetExtents(1).first, std::min(std::abs(azimuth), GetExtents(1).second));
  
    coords[2] = energy;

    // Set flux to 0 outside the valid energy range
    if (coords[2] <= GetExtents(2).first || coords[2] > GetExtents(2).second){
      log_warn ("Outside valid energy range for the chosen 3-dimensional flux model");
      return 0.0;
    }
  
    int centers[3];
    if(data.searchcenters(coords, centers) == 0){
      double result = data.ndsplineeval(coords, centers, 0);
      return result;
    }
  
    return(0.0);
  }

} //namespace nuflux

NNF_REGISTER_FLUX("honda2012_spl_solmin",&nuflux::LEFlux::makeFlux);
