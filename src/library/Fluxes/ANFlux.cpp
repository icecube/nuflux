
#include "NewNuFlux/Fluxes/ANFlux.h"
#include <boost/make_shared.hpp>

namespace NewNuFlux {

ANFlux::ANFlux(const std::string& fluxName) : FluxFunction(fluxName)
{
	std::string base = detail::getDataDir() + "/ANFlux/" + fluxName;
	boost::shared_ptr<Evaluator> evaluator = boost::make_shared<Evaluator>(base+"_nue.fits");
	fluxes_[I3Particle::NuE] = evaluator;
	fluxes_[I3Particle::NuEBar] = evaluator;
	evaluator = boost::make_shared<Evaluator>(base+"_numu.fits");
	fluxes_[I3Particle::NuMu] = evaluator;
	fluxes_[I3Particle::NuMuBar] = evaluator;
}

boost::shared_ptr<FluxFunction>
ANFlux::makeFlux(const std::string& fluxName)
{
	return(boost::dynamic_pointer_cast<FluxFunction>(boost::make_shared<ANFlux>(fluxName)));
}

double
ANFlux::getFlux(particleType type, double energy, double cosZenith) const
{
	std::map<particleType, boost::shared_ptr<Evaluator> >::const_iterator f = fluxes_.find(type);
	if (f == fluxes_.end())
		return 0.0;
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

} //namespace NewNuFlux

NNF_REGISTER_FLUX("CORSIKA_GaisserH3a_QGSJET-II",&NewNuFlux::ANFlux::makeFlux);
NNF_REGISTER_FLUX("CORSIKA_GaisserH3a_SIBYLL-2.1",&NewNuFlux::ANFlux::makeFlux);
NNF_REGISTER_FLUX("CORSIKA_GaisserH3a_average",&NewNuFlux::ANFlux::makeFlux);
