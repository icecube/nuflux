#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <boost/make_shared.hpp>
#include <boost/math/constants/constants.hpp>

#include <nuflux/IPLEFlux.h>

namespace nuflux{
  boost::shared_ptr<FluxFunction> IntegralPreservingFlux::makeFlux(const std::string& fluxName){
    return(boost::dynamic_pointer_cast<FluxFunction>(boost::make_shared<IntegralPreservingFlux>(fluxName)));
  }

  IntegralPreservingFlux::IntegralPreservingFlux(const std::string& fluxName):
    FluxFunction(fluxName)
  {
    loadTables(fluxName+"_nue",NuE);
    loadTables(fluxName+"_nbe",NuEBar);
    loadTables(fluxName+"_num",NuMu);
    loadTables(fluxName+"_nbm",NuMuBar);
  }

  IntegralPreservingFlux::~IntegralPreservingFlux(){}

  void IntegralPreservingFlux::loadTables(const std::string& fluxName, ParticleType type){

    SetSubGeV = fluxName.find("sk")  != std::string::npos; // Extend to SubGeV for fluxes at SK

    const double enBinWidth=.05;
    {
      std::string fname = detail::getDataPath("IPLEFlux/"+fluxName+"3D.dat");
      std::ifstream fluxFile3D((fname).c_str());
      if(!fluxFile3D){
        throw std::runtime_error("Failed to open " + fname );
      }
      double energy, cosZenith, azimuth, fluxpe;
      double lastAzimuth=-1;
      std::map<std::pair<double,double>,dumbHistogram> p3DFluxMap;
      dumbHistogram* curHist=NULL;
      while(fluxFile3D >> energy >> cosZenith >> azimuth >> fluxpe){
        double flux=fluxpe*energy;
        if(azimuth!=lastAzimuth){
          curHist=&(p3DFluxMap.insert(std::make_pair( std::make_pair(cosZenith,azimuth),
                                                      dumbHistogram(-1.025))).first->second);
          lastAzimuth = azimuth;
        }
        curHist->push_back(log10(energy)+enBinWidth/2,flux);
      }

      for(std::map<std::pair<double,double>,dumbHistogram>::const_iterator it=p3DFluxMap.begin(), end=p3DFluxMap.end(); it!=end; it++){
        const dumbHistogram& hist=it->second;
        std::pair<std::vector<double>,std::vector<double> > integratedFlux=hist.accumulate();
        CubicSpline integratedSpline(integratedFlux.first,integratedFlux.second);
        energySplines3D[type].insert(std::make_pair(it->first,integratedSpline));
      }
    }

    {
      std::string fname = detail::getDataPath("IPLEFlux/"+fluxName+"2D.dat");
      std::ifstream fluxFile2D((fname).c_str());
      if(!fluxFile2D){
        throw std::runtime_error("Failed to open " + name );
      }
      double energy, cosZenith, fluxpe;
      double lastCosZenith=-2;
      std::map<double,dumbHistogram> p2DFluxMap;
      dumbHistogram* curHist=NULL;
      while(fluxFile2D >> energy >> cosZenith >> fluxpe){
        double flux=fluxpe*energy;
        if(cosZenith!=lastCosZenith){
          curHist=&(p2DFluxMap.insert(std::make_pair(cosZenith,dumbHistogram(-1.025))).first->second);
          lastCosZenith = cosZenith;
        }
        curHist->push_back(log10(energy)+enBinWidth/2,flux);
      }

      for(std::map<double,dumbHistogram>::const_iterator it=p2DFluxMap.begin(), end=p2DFluxMap.end(); it!=end; it++){
        const dumbHistogram& hist=it->second;
        std::pair<std::vector<double>,std::vector<double> > integratedFlux=hist.accumulate();
        CubicSpline integratedSpline(integratedFlux.first,integratedFlux.second);
        energySplines2D[type].insert(std::make_pair(it->first,integratedSpline));
      }
    }
  }

  double IntegralPreservingFlux::getMinEnergy() const{
    if (SetSubGeV==true) {
      return 1e-1;
    }
    else {
      return 1;
    }
  }

  double IntegralPreservingFlux::getMaxEnergy() const{
    return 1e4;
  }


  double IntegralPreservingFlux::getFlux(ParticleType type, double energy, double cosZenith) const{
    if(cosZenith<-1 || cosZenith>1){
      return 0;
    }
    if(energy< getMinEnergy() || energy> getMaxEnergy()){
      return 0;
    }
    return(evaluate2D(type, energy, cosZenith));
  }

  double IntegralPreservingFlux::getFlux(ParticleType type, double energy, double azimuth, double cosZenith) const{
    //if(azimuth<0 || azimuth>2*boost::math::constants::pi<double>())
    //log_fatal_stream("Out of range azimuth angle " << azimuth);
    if(azimuth<0 || azimuth>360){
      throw std::runtime_error("Out of range azimuth angle " + std::to_string(azimuth));
    }
    if(cosZenith < -1 || cosZenith > 1){
      return 0;
    }
    if(energy < getMinEnergy() || energy > getMaxEnergy()){
      return 0;
    }
    //Above 15.0 GeV, any azimuthal dependence should be negligible.
    //Due to these small fluctuations, this integral-preserving method does not
    //work well, and as such the azimuth averaged value is always returned here.
    if(energy>15)
      return(evaluate2D(type, energy, cosZenith));
    return(InterpolateAzimuth(type, energy, azimuth, cosZenith));
  }

  double IntegralPreservingFlux::evaluate2D(ParticleType type, double energy, double cosZenith) const{
    double logEnergy=log10(energy);
    dumbHistogram hFluxVCosZenith(-1);
    std::map<ParticleType, std::map<double,CubicSpline> >::const_iterator typeIt=energySplines2D.find(type);

    if(typeIt==energySplines2D.end()){
      if (isNeutrino(type)) {
        return 0;
      } else {
        throw std::runtime_error( name + " does not support particle type " +
                                  boost::lexical_cast<std::string>(type));
      }
    }

    for(int cz=0; cz<20; cz++){
      double LCosZenith=(10*cz-95)/100.;
      std::map<double,CubicSpline>::const_iterator it;
      it=typeIt->second.find(LCosZenith);
      if(it==typeIt->second.end()){
        throw std::runtime_error("Internal error: angle not found (" + std::to_string(LCosZenith) + ')');
      }
      double flux=it->second.derivative(logEnergy);
      hFluxVCosZenith.push_back((10*cz-90)/100.,flux);
    }
    std::pair<std::vector<double>,std::vector<double> > integratedFlux=hFluxVCosZenith.accumulate();
    CubicSpline integratedSpline(integratedFlux.first,integratedFlux.second);
    double UnitFactor = 1e-4; //converts from m^2 -> cm^2
    return(integratedSpline.derivative(cosZenith)*UnitFactor/energy);
  }

  double IntegralPreservingFlux::InterpolateAzimuth(ParticleType type, double energy, double azimuth, double cosZenith) const{
    dumbHistogram hFluxVAzimuth(0);
    double AzBinWidth = 30.0;
    for(unsigned int i=0; i<12; i++){
      double Flux=InterpolateCZFlux(type, energy, AzBinWidth*(i+.5), cosZenith);
      hFluxVAzimuth.push_back(AzBinWidth*(i+1),Flux);
    }
    double UnitFactor = 1e-4; //converts from m^2 -> cm^2
    std::pair<std::vector<double>,std::vector<double> > integratedFlux=hFluxVAzimuth.accumulate();
    CubicSpline integratedSpline(integratedFlux.first,integratedFlux.second);
    return(integratedSpline.derivative(azimuth)*UnitFactor/energy);
  }

  double IntegralPreservingFlux::InterpolateCZFlux(ParticleType type, double energy, double azimuth, double cosZenith) const{
    double logEnergy=log10(energy);
    dumbHistogram hFluxVCosZenith(-1);
    std::map<ParticleType, std::map<std::pair<double,double>,CubicSpline> >::const_iterator typeIt=energySplines3D.find(type);

    if(typeIt==energySplines3D.end()){
      if (isNeutrino(type)) {
        return 0;
      } else {
        throw std::runtime_error( name + " does not support particle type " +
                                  boost::lexical_cast<std::string>(type));
      }
    }

    for(int cz=0; cz<20; cz++){
      double LCosZenith=(10*cz-95)/100.;
      std::map<std::pair<double,double>,CubicSpline>::const_iterator it;
      it=typeIt->second.find(std::make_pair(LCosZenith,azimuth));
      if(it==typeIt->second.end()){
        throw std::runtime_error(
          "Internal error: angle combination not found (" + std::to_string(LCosZenith) + ',' + std::to_string(azimuth) + ')'
        );
      }
      double flux=it->second.derivative(logEnergy);
      hFluxVCosZenith.push_back((10*cz-90)/100.,flux);
    }
    std::pair<std::vector<double>,std::vector<double> > integratedFlux=hFluxVCosZenith.accumulate();
    CubicSpline integratedSpline(integratedFlux.first,integratedFlux.second);
    return(integratedSpline.derivative(cosZenith));
  }

} //namespace nuflux

NNF_REGISTER_FLUX("IPhonda2006_sno_solmin",&nuflux::IntegralPreservingFlux::makeFlux);
NNF_REGISTER_FLUX("IPhonda2014_sk_solmin",&nuflux::IntegralPreservingFlux::makeFlux);
NNF_REGISTER_FLUX("IPhonda2014_sk_solmax",&nuflux::IntegralPreservingFlux::makeFlux);
NNF_REGISTER_FLUX("IPhonda2014_spl_solmin",&nuflux::IntegralPreservingFlux::makeFlux);
NNF_REGISTER_FLUX("IPhonda2014_spl_solmax",&nuflux::IntegralPreservingFlux::makeFlux);
