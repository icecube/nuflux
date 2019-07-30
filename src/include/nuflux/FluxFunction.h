#ifndef NNF_FLUXFUNCTION_H
#define NNF_FLUXFUNCTION_H

#include <cmath>
#include <string>
#include <stdexcept>

#include <boost/preprocessor/cat.hpp>
#include <boost/shared_ptr.hpp>

#include <nuflux/logging.h>
#include <nuflux/particleType.h>


namespace NewNuFlux{

	typedef I3Particle::ParticleType particleType;

	///The interface for all neutrino fluxes
	class FluxFunction{
	public:
		FluxFunction(std::string name):name(name){}
		virtual ~FluxFunction(){}
		
		std::string getName() const{ return(name); }
	
		///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
		virtual double getFlux(particleType type, double energy, double cosZenith) const=0;

		///Computes the expected flux for neutrinos of the given type, energy, azimuth, and zenith angle
		///If the derived classes do not have a specific azimuth dependence then just use the normal
		///flux call.
		virtual double getFlux(particleType type, double energy, double azimuth, double cosZenith) const{
		  return (getFlux( type, energy, cosZenith));
		}

	protected:
		std::string name;
	};
	
	///Things which are useful or necessary internally for the library but which aren't useful for users
	namespace detail{
		///Add a flux to the registry
		void registerNeutrinoFlux(const std::string& name, 
		                          boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&));
		///Add a flux to the registry with a warning that it is deprecated
		void registerNeutrinoFlux(const std::string& name,
		                          boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&),
								  const std::string& deprecationReason);
		
		struct FluxRegisterererer{
			FluxRegisterererer(const std::string& name, 
			                   boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&)){
				registerNeutrinoFlux(name,factoryFn);
			}
			FluxRegisterererer(const std::string& name,
			                   boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&),
							   const std::string& deprecationReason){
				registerNeutrinoFlux(name,factoryFn,deprecationReason);
			}
		};
		std::string getDataPath(std::string fname);

	} //namespace detail

} //namespace NewNuFlux




#define NNF_REGISTER_FLUX(name,factory) \
namespace{ NewNuFlux::detail::FluxRegisterererer BOOST_PP_CAT( registerererer , __LINE__ ) (name,factory); }
#define NNF_REGISTER_DEPRECATED_FLUX(name,factory,reason) \
namespace{ NewNuFlux::detail::FluxRegisterererer BOOST_PP_CAT( registerererer , __LINE__ ) (name,factory,reason); }

#endif
