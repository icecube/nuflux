#include <nuflux/nuflux.h>
#include <iostream>
#include "config.h"

namespace nuflux{

  ///Return the current nuflux version
  std::string getVersion(){
    return NUFLUX_VERSION;
  }

  namespace detail{
    struct FluxFactory{
      boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&);
      std::string deprecationReason;

      FluxFactory(boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&),
                  const std::string& deprecationReason):
        factoryFn(factoryFn),deprecationReason(deprecationReason){}

      boost::shared_ptr<FluxFunction> operator()(const std::string& name) const{
        if(!deprecationReason.empty()){
          std::cerr << "WARNING: The '" << name << "' flux implementation is deprecated:\n " << deprecationReason << '\n';
        }
        return(factoryFn(name));
      }
    };
    std::map<std::string,FluxFactory>* registry=NULL;

    void registerNeutrinoFlux(const std::string& name,
                              boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&)){
      if(!registry)
        registry=new std::map<std::string,detail::FluxFactory>;
      registry->insert(std::make_pair(name,detail::FluxFactory(factoryFn,"")));
    }

    void registerNeutrinoFlux(const std::string& name,
                              boost::shared_ptr<FluxFunction>(*factoryFn)(const std::string&),
                              const std::string& deprecationReason){
      if(!registry)
        registry=new std::map<std::string,detail::FluxFactory>;
      registry->insert(std::make_pair(name,detail::FluxFactory(factoryFn,deprecationReason)));
    }

    std::multimap<std::string,std::string>* kneeRegistry=NULL;

    void registerKneeModel(const std::string baseModel, const std::string name){
      if(!kneeRegistry)
        kneeRegistry=new std::multimap<std::string,std::string>;
      kneeRegistry->insert(std::make_pair(baseModel,name));
    }


  }

  bool isNeutrino(ParticleType pdgid){
    return ( (pdgid==12) || (pdgid==-12) || (pdgid==14) || (pdgid==-14) || (pdgid==16) || (pdgid==-16) );
  }

  boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName){
    if(!detail::registry)
      throw std::invalid_argument("Internal error: Flux registry does not exist");
    std::map<std::string,detail::FluxFactory>::const_iterator it=detail::registry->find(fluxName);
    if(it==detail::registry->end())
      throw std::invalid_argument("Unregistered flux name: "+fluxName);
    return(it->second(fluxName));
  }

  std::vector<std::string> availableFluxes(){
    std::vector<std::string> validNames;
    if(!detail::registry)
      return(validNames);
    for(std::map<std::string,detail::FluxFactory>::const_iterator it=detail::registry->begin(), end=detail::registry->end();
        it!=end; it++)
      validNames.push_back(it->first);
    return(validNames);
  }

  std::vector<std::string> kneesForFlux(std::string fluxName){
    std::vector<std::string> validKnees;
    if(!detail::kneeRegistry)
      return(validKnees);
    typedef std::multimap<std::string,std::string>::const_iterator Iterator;
    std::pair<Iterator,Iterator> range=detail::kneeRegistry->equal_range(fluxName);
    for(Iterator it=range.first; it!=range.second; it++)
      validKnees.push_back(it->second);
    return(validKnees);
  }

  void printModels(){
    std::cout << "Available models are: \n";
    std::vector<std::string> models=availableFluxes();
    for(std::vector<std::string>::const_iterator model=models.begin(), modelEnd=models.end(); model!=modelEnd; model++){
      std::cout << '-'  << *model << '\n';
      std::vector<std::string> knees=kneesForFlux(*model);
      if(!knees.empty()){
        std::cout << "  alternate knee models:\n";
        for(std::vector<std::string>::const_iterator knee=knees.begin(), kneeEnd=knees.end(); knee!=kneeEnd; knee++)
          std::cout << "  +" << *knee << '\n';
      }
    }
  }
}
