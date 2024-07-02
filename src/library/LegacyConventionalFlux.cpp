#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include "nuflux/LegacyConventionalFlux.h"

const double MIN_ENERGY = 10;
const double MAX_ENERGY = 1e9;

namespace nuflux{
  LegacyConventionalFlux::component readConvComponent(const std::string& fname){

    std::string path = detail::getDataPath("LegacyConventionalFlux/"+fname);


    std::ifstream infile(path.c_str());
    if(!infile)
      throw std::runtime_error("Unable to read "+path);
    LegacyConventionalFlux::component c;
    infile >> c;
    return(c);
  }

  LegacyConventionalFlux::LegacyConventionalFlux(const std::string& fluxName):
    FluxFunction(fluxName)
  {
    components[NuMu]   =readConvComponent(fluxName+"_numu.dat");
    components[NuMuBar]=readConvComponent(fluxName+"_numubar.dat");
    components[NuE]    =readConvComponent(fluxName+"_nue.dat");
    components[NuEBar] =readConvComponent(fluxName+"_nuebar.dat");
    //this looks dumb but is actually necessary to enforce the continuity constraint
    setRelativePionContribution(1.0);
    setRelativeKaonContribution(1.0);
  }

  boost::shared_ptr<FluxFunction> LegacyConventionalFlux::makeFlux(const std::string& fluxName){
    return(boost::dynamic_pointer_cast<FluxFunction>(boost::make_shared<LegacyConventionalFlux>(fluxName)));
  }

  double LegacyConventionalFlux::getMinEnergy() const{
    return MIN_ENERGY;
  }
  double LegacyConventionalFlux::getMaxEnergy() const{
    return MAX_ENERGY;
  }

  double LegacyConventionalFlux::getFlux(ParticleType type, double energy, double cosZenith) const{
    std::map<ParticleType,component>::const_iterator it=components.find(type);
    if(it==components.end()){
      if (isNeutrino(type)) {
        return 0;
      } else {
        throw std::runtime_error( name+" does not support particle type " +
                                  boost::lexical_cast<std::string>(type));
      }
    }
    return(it->second.getFlux(energy,cosZenith)*kneeCorrection(energy));
  }

  //returns true if the given critical energy is one associated with kaons,
  //or false if it is associated with pions
  bool classifyCriticalEnergy(double E_c){
    double tol=1e-3;
    double piPlusMinus=115;
    double kPlusMinus=850;
    double kLong=205;
    double none=1000; //of course we use this value to indicate no data

    if(std::abs(E_c/piPlusMinus-1)<tol)
      return(false);
    if(std::abs(E_c/none-1)<tol)
      return(false); //shouldn't actually matter what we return
    if(std::abs(E_c/kPlusMinus-1)<tol)
      return(true);
    if(std::abs(E_c/kLong-1)<tol)
      return(true);
    throw std::domain_error("Unable to identify particle type associated with a critical energy of "+
                            boost::lexical_cast<std::string>(E_c)+" GeV");
  }

  std::istream& operator>>(std::istream& is, LegacyConventionalFlux::component::progenitorComponent& c){
    is >> c.A >> c.B >> c.E_c;
    c.E_c*=1e3; //convert TeV to GeV
    c.kaonic=classifyCriticalEnergy(c.E_c);
    return(is);
  }

  std::istream& operator>>(std::istream& is, LegacyConventionalFlux::component& c){
    //read all 49 unlabeled parameters from the input stream
    for(unsigned int i=0; i<15; i++)
      is >> c.lowECoeffs[i];
    for(unsigned int i=0; i<4; i++)
      is >> c.lowECutoffs[i];
    for(unsigned int i=0; i<2; i++)
      is >> c.directComponents[i];
    is >> c.highEIndex;
    is >> c.norm;
    for(unsigned int i=0; i<5; i++)
      is >> c.inclinationCoeffs[i];
    c.inclinationCoeffs[0]*=c.inclinationCoeffs[0]; //square this one ahead of time
    for(unsigned int i=0; i<6; i++)
      is >> c.normTweaks[i];
    for(unsigned int i=0; i<3; i++)
      is >> c.modifiedComponents[i];
    is >> c.modificationScale;
    c.modificationScale*=1e3; //convert TeV to GeV
    is >> c.modificationNorm;
    //make note that the pion/kaon ratio is the default
    c.pionAdjust=1.0;
    c.kaonAdjust=1.0;
    c.pionKaonNorm=1.0;
    return(is);
  }

  double LegacyConventionalFlux::component::progenitorComponent::operator()(double energy, double inclination) const{
    return(A/(1+B*inclination*energy/E_c));
  }

  /** @brief A correction for the mean interaction length of mesons at production altitude
   * The form of this correction is derived in <a href="https://arxiv.org/abs/hep-ph/0407078v1">a report by D. Chirkin</a>.
   */
  double LegacyConventionalFlux::component::inclination(double abscosZenith) const{
    return(sqrt((abscosZenith*abscosZenith + inclinationCoeffs[0] + inclinationCoeffs[1]*pow(abscosZenith, inclinationCoeffs[2]) + inclinationCoeffs[3]*pow(abscosZenith, inclinationCoeffs[4]))
                /(1+inclinationCoeffs[0]+inclinationCoeffs[1]+inclinationCoeffs[3])));
  }

  //figure out what energy is appropriate for transitioning to the high energy calculation for the given direction
  double LegacyConventionalFlux::component::transitionEnergy(double cosZenith) const{
    double acz=std::abs(cosZenith);
    if(acz>.5)
      return(lowECutoffs[0]);
    else if(acz>.3)
      return(lowECutoffs[1]);
    else if(acz>.1)
      return(lowECutoffs[2]);
    else
      return(lowECutoffs[3]);
  }

  double LegacyConventionalFlux::component::getFlux(double energy, double cosZenith) const{
    //never compute fluxes outside this energy range
    if(energy<MIN_ENERGY || energy>MAX_ENERGY){
      return(0.0);
    }
    if((cosZenith < -1) || (cosZenith > +1)){
      return(0.0);
    }
    if(energy<transitionEnergy(cosZenith)){
      //we can use the low energy polynomial
      double lE=log10(energy);
      cosZenith=-std::abs(cosZenith); //sky is symmetrical, parameterization is for upgoing directions
      double c=cosZenith, c2=c*c, c3=c2*c, c4=c2*c2;
      double ex=(((((lowECoeffs[0]*c) + lowECoeffs[1])*c + lowECoeffs[2])*c + lowECoeffs[3])*c + lowECoeffs[4])*c + lowECoeffs[5]
        + (((((lowECoeffs[6]*lE) + lowECoeffs[7])*lE + lowECoeffs[8])*lE + lowECoeffs[9])*lE + lowECoeffs[10])*lE
        + ((((lowECoeffs[11]*c)*lE + lowECoeffs[12]*c2)*lE + lowECoeffs[13]*c3)*lE + lowECoeffs[14]*c4)*lE;
      return(pionKaonNorm*pow(10,ex));
    }
    //otherwise, we're in the high energy regime
    cosZenith=std::abs(cosZenith);

    //figure out the correction factor for the normalization
    //code preserved from original, without comment
    double normCosTheta[6] = {0.1, 0.2, 0.4, 0.6, 0.8, 1.0};
    int normIdx = 0;
    double normTweak = 1.0;
    // Tweak is not accurate very close to zero
    for (int i = 0; i < 6; i++)
      normIdx = (cosZenith > normCosTheta[i]) ? i : normIdx;
    normTweak = normTweaks[normIdx] +
      (normTweaks[normIdx+1]-normTweaks[normIdx])/(normCosTheta[normIdx+1]-normCosTheta[normIdx]) *
      (cosZenith-normCosTheta[normIdx]);

    double inc=inclination(cosZenith);
    double core=0;
    for(unsigned int i=0; i<2; i++)
      core+=directComponents[i](energy,inc)*(directComponents[i].kaonic?kaonAdjust:pionAdjust);
    double modification=modificationNorm*(1-exp(-modificationScale/energy/inc));
    for(unsigned int i=0; i<3; i++)
      core+=modification*modifiedComponents[i](energy,inc)*(modifiedComponents[i].kaonic?kaonAdjust:pionAdjust);
    return(normTweak*norm*pow(energy,highEIndex-1)*pow(10.,-3*highEIndex)*core);
  }

  void LegacyConventionalFlux::component::adjustPionKaonRatio(double pionAdjust, double kaonAdjust){
    const double czStep=.1;
    const double enEps=.1;

    //set the adjusted ratio
    this->pionAdjust=pionAdjust;
    this->kaonAdjust=kaonAdjust;
    //set no relative normalization factor
    pionKaonNorm=1;
    //figure out how much mismatch we have between the low energy and high energy parameterizations
    double mismatch=0.0;
    unsigned int samples=0;
    for(double cz=-1; cz<=1; cz+=czStep, samples++){
      double en=transitionEnergy(cz);
      double low=getFlux(en-enEps,cz);
      double high=getFlux(en+enEps,cz);
      mismatch+=high/low;
    }
    mismatch/=samples;

    //set the relative normalization
    pionKaonNorm=mismatch;
  }

  void LegacyConventionalFlux::setRelativePionContribution(double adjust){
    pionAdjust=adjust;
    for(std::map<ParticleType,component>::iterator it=components.begin(), end=components.end();
        it!=end; it++)
      it->second.adjustPionKaonRatio(pionAdjust,kaonAdjust);
  }

  void LegacyConventionalFlux::setRelativeKaonContribution(double adjust){
    kaonAdjust=adjust;
    for(std::map<ParticleType,component>::iterator it=components.begin(), end=components.end();
        it!=end; it++)
      it->second.adjustPionKaonRatio(pionAdjust,kaonAdjust);
  }

  //knee reweighting stuff
  LegacyConventionalFlux::kneeSpline::kneeSpline():
    x(1,0.0),y(1,1.0),b(1,0.0),c(1,0.0),d(1,0.0){}

  double LegacyConventionalFlux::kneeSpline::operator()(double energy) const{
    const double lE=log10(energy);
    int idx=std::distance(x.begin(),std::lower_bound(x.begin(),x.end(),lE));
    if(idx>0)
      idx--;
    double dx=lE-x[idx];
    return(y[idx]+dx*(b[idx]+dx*(c[idx]+dx*d[idx])));
  }

  std::istream& operator>>(std::istream& is, LegacyConventionalFlux::kneeSpline& s){
    s.x.resize(100);
    for(unsigned int i=0; i<100; i++)
      is >> s.x[i];
    s.y.resize(100);
    for(unsigned int i=0; i<100; i++)
      is >> s.y[i];
    s.b.resize(100);
    for(unsigned int i=0; i<100; i++)
      is >> s.b[i];
    s.c.resize(100);
    for(unsigned int i=0; i<100; i++)
      is >> s.c[i];
    s.d.resize(100);
    for(unsigned int i=0; i<100; i++)
      is >> s.d[i];
    return(is);
  }

  void LegacyConventionalFlux::setKneeReweightingModel(std::string reweightModel){
    kneeCorrectionName=reweightModel;
    if(kneeCorrectionName=="none"){
      kneeCorrection=kneeSpline();
      return;
    }
    std::string correctionFilePath=detail::getDataPath("LegacyConventionalFlux/"+name+"_"+reweightModel+".dat");
    std::ifstream infile(correctionFilePath.c_str());
    if(!infile)
      throw std::runtime_error( "Unable to read "+correctionFilePath+"; are you sure that "
                                +reweightModel+" is a valid knee model for "+name+"?");
    infile >> kneeCorrection;
  }
}

NNF_REGISTER_FLUX("honda2006",&nuflux::LegacyConventionalFlux::makeFlux);
NNF_REGISTER_FLUX("bartol",&nuflux::LegacyConventionalFlux::makeFlux);

NNF_REGISTER_KNEE("honda2006","gaisserH3a_elbert");
NNF_REGISTER_KNEE("honda2006","gaisserH4a_elbert");
NNF_REGISTER_KNEE("honda2006","gst13_elbert");
NNF_REGISTER_KNEE("honda2006","gst13star_elbert");
NNF_REGISTER_KNEE("honda2006","polygonato_mod_elbert");
