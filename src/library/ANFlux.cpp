#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include "nuflux/ANFlux.h"

namespace nuflux {

ANFlux::ANFlux(const std::string& fluxName) : FluxFunction(fluxName)
{
  std::string nuefile = detail::getDataPath("ANFlux/" + fluxName + "_nue.fits");
  boost::shared_ptr<Evaluator> evaluator = boost::make_shared<Evaluator>(nuefile);
  fluxes_[NuE] = evaluator;
  fluxes_[NuEBar] = evaluator;
  std::string numufile = detail::getDataPath("ANFlux/" + fluxName + "_numu.fits");
  evaluator = boost::make_shared<Evaluator>(numufile);
  fluxes_[NuMu] = evaluator;
  fluxes_[NuMuBar] = evaluator;

  emin_ = pow(10,std::nextafter(evaluator->GetExtents(0).first,std::numeric_limits<double>::infinity())) ;
  emax_ = pow(10,evaluator->GetExtents(0).second) ;
}

  double ANFlux::getMinEnergy() const{
    return emin_;
  }
  double ANFlux::getMaxEnergy() const{
    return emax_;
  }



boost::shared_ptr<FluxFunction>
ANFlux::makeFlux(const std::string& fluxName)
{
  return(boost::dynamic_pointer_cast<FluxFunction>(boost::make_shared<ANFlux>(fluxName)));
}

double
ANFlux::getFlux(ParticleType type, double energy, double cosZenith) const
{
  if ((energy < emin_) || (energy > emax_) || (cosZenith < -1) || (cosZenith > 1)){
    return 0;
  }

  std::map<ParticleType, boost::shared_ptr<Evaluator> >::const_iterator f = fluxes_.find(type);
  if (f == fluxes_.end()){
    if (isNeutrino(type)) {
      return 0;
    } else {
      throw std::runtime_error( name+" does not support particle type " +
                                boost::lexical_cast<std::string>(type));
    }
  }
  else
    // The current parameterizations are for nu + nubar.
    // FIXME: separate them, then remove the factor of 2!
    return (*(f->second))(energy, cosZenith)/2.;
}

ANFlux::Evaluator::Evaluator(const std::string& fname):data(fname){}

ANFlux::Evaluator::~Evaluator(){}

std::pair<double, double>
ANFlux::Evaluator::GetExtents(size_t dim) const
{
  if(dim >= data.get_ndim())
    throw std::out_of_range("Dimension index out of range");
  return(std::make_pair(data.lower_extent(dim), data.upper_extent(dim)));
}

double
ANFlux::Evaluator::operator()(double energy, double cosZenith) const
{
  double coords[2];
  coords[0] = std::log10(energy);
  // Set flux to 0 outside the valid energy range
  if (coords[0] <= GetExtents(0).first || coords[0] > GetExtents(0).second)
    return 0.0;
  // Extrapolate with a constant outside the valid zenith range
  coords[1] = std::max(GetExtents(1).first, std::min(std::abs(cosZenith), GetExtents(1).second));
  int centers[2];
  if(data.searchcenters(coords, centers)){
    double result=data.ndsplineeval(coords, centers, 0);
    return(std::pow(10, result)/std::pow(energy, 3.0));
  }

  return(0.0);
}

} //namespace nuflux

NNF_REGISTER_FLUX("CORSIKA_GaisserH3a_QGSJET-II",&nuflux::ANFlux::makeFlux);
NNF_REGISTER_FLUX("CORSIKA_GaisserH3a_SIBYLL-2.1",&nuflux::ANFlux::makeFlux);
NNF_REGISTER_FLUX("CORSIKA_GaisserH3a_average",&nuflux::ANFlux::makeFlux);
