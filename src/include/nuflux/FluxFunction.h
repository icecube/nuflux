#ifndef NNF_FLUXFUNCTION_H
#define NNF_FLUXFUNCTION_H

#include <cmath>
#include <string>
#include <stdexcept>

#include <boost/preprocessor/cat.hpp>
#include <boost/shared_ptr.hpp>

#include <nuflux/nuflux.h>

#define NUFLUX_ParticleType  (NuE)(NuEBar)(NuMu)(NuMuBar)(NuTau)(NuTauBar)

namespace nuflux{

  enum ParticleType {
    NuE = 12,
    NuEBar = -12,
    NuMu = 14,
    NuMuBar = -14,
    NuTau = 16,
    NuTauBar = -16,
  };

  /// Determines if particle type is a neutrino
  bool isNeutrino(ParticleType pdgid);

  ///The interface for all neutrino fluxes
  class FluxFunction{
  public:
    FluxFunction(std::string name):
      name(name),
      kneeCorrectionName("none")
      {}
    virtual ~FluxFunction(){}

    std::string getName() const{ return(name); }

    ///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
    virtual double getFlux(ParticleType type, double energy, double cosZenith) const=0;

    ///Computes the expected flux for neutrinos of the given type, energy, azimuth, and zenith angle
    ///If the derived classes do not have a specific azimuth dependence then just use the normal
    ///flux call.
    virtual double getFlux(ParticleType type, double energy, double azimuth, double cosZenith) const{
      return (getFlux( type, energy, cosZenith));
    }

    virtual double getMinEnergy() const{ return NAN;}
    virtual double getMaxEnergy() const{ return NAN;}
    virtual double readExtents(ParticleType type) const{ return NAN;}

    virtual void setKneeReweightingModel(std::string reweightModel){
      if (reweightModel != "none"){
        throw std::runtime_error("This model does not support knee reweighting");
      }
    }
    std::string getKneeReweightingModel() const{ return(kneeCorrectionName); }

  protected:
    std::string name;
    std::string kneeCorrectionName;
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
      FluxRegisterererer( const std::string& name,
                          boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&)){
        registerNeutrinoFlux(name,factoryFn);
      }
      FluxRegisterererer( const std::string& name,
                          boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&),
                          const std::string& deprecationReason){
        registerNeutrinoFlux(name,factoryFn,deprecationReason);
      }
    };
    std::string getDataPath(std::string fname);

    void registerKneeModel(const std::string baseModel, const std::string name);

    struct KneeRegisterererer{
      KneeRegisterererer(const std::string baseModel, const std::string name){
        registerKneeModel(baseModel,name);
      }
    };

  } //namespace detail

} //namespace nuflux

#define NNF_REGISTER_FLUX(name,factory)\
  namespace{ nuflux::detail::FluxRegisterererer BOOST_PP_CAT( registerererer , __LINE__ ) (name,factory); }
#define NNF_REGISTER_DEPRECATED_FLUX(name,factory,reason)\
  namespace{ nuflux::detail::FluxRegisterererer BOOST_PP_CAT( registerererer , __LINE__ ) (name,factory,reason); }
#define NNF_REGISTER_KNEE(baseModel,name)                               \
  namespace{ nuflux::detail::KneeRegisterererer BOOST_PP_CAT( registerererer , __LINE__ ) (baseModel,name); }

#endif
