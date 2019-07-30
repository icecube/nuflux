#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <nuflux/LegacyPromptFlux.h>

namespace NewNuFlux{
	
	LegacyPromptFlux::component readPromptComponent(const std::string& path){
		std::ifstream infile(path.c_str());
		if(!infile)
			throw std::runtime_error("Unable to read "+path);
		LegacyPromptFlux::component c;
		infile >> c;
		return(c);
	}
	
	LegacyPromptFlux::LegacyPromptFlux(const std::string& fluxName):
	FluxFunction(fluxName),
	KneeReweightable("none"){
          components[I3Particle::NuMu]   =readPromptComponent(detail::getDataPath("LegacyPromptFlux/"+fluxName+"_numupro.dat"));
          components[I3Particle::NuMuBar]=readPromptComponent(detail::getDataPath("LegacyPromptFlux/"+fluxName+"_numubarpro.dat"));
          components[I3Particle::NuE]    =readPromptComponent(detail::getDataPath("LegacyPromptFlux/"+fluxName+"_nuepro.dat"));
          components[I3Particle::NuEBar] =readPromptComponent(detail::getDataPath("LegacyPromptFlux/"+fluxName+"_nuebarpro.dat"));
	}
	
	boost::shared_ptr<FluxFunction> LegacyPromptFlux::makeFlux(const std::string& fluxName){
		return(boost::dynamic_pointer_cast<FluxFunction>(boost::make_shared<LegacyPromptFlux>(fluxName)));
	}
	
	double LegacyPromptFlux::getFlux(particleType type, double energy, double cosZenith) const{
		//as a special case, allow tau neutrinos, but always return zero flux for them
		if(type==I3Particle::NuTau || type==I3Particle::NuTauBar)
			return(0);
		std::map<particleType,component>::const_iterator it=components.find(type);
		if(it==components.end())
			throw std::runtime_error(name+" does not support particle type "+boost::lexical_cast<std::string>(type));
		return(it->second.getFlux(energy,cosZenith)*kneeCorrection(energy));
	}
	
	std::istream& operator>>(std::istream& is, LegacyPromptFlux::component& c){
		is >> c.eMin;
		double dummy; //dummy variable onto which we read all of the unused parameter errors
		for(unsigned int i=0; i<15; i++)
			is >> c.coeffs[i] >> dummy;
		
		//there is one more value at the end of the data files,
		//but since the old code didn't read it, it is impossible
		//to know what it means, so we just leave it alone
		
		return(is);
	}
	
	double LegacyPromptFlux::component::getFlux(double energy, double cosZenith) const{
		if(energy<eMin) //never compute fluxes below the minimum energy
			return(0.0); //might want to issue a warning here?
	
		double lE=log10(energy);
		cosZenith=-std::abs(cosZenith); //sky is symmetrical, parameterization is for upgoing directions
		double c=cosZenith, c2=c*c, c3=c2*c, c4=c2*c2;
		double ex=(((((coeffs[0]*c) + coeffs[1])*c + coeffs[2])*c + coeffs[3])*c + coeffs[4])*c + coeffs[5]
		          + (((((coeffs[6]*lE) + coeffs[7])*lE + coeffs[8])*lE + coeffs[9])*lE + coeffs[10])*lE
		          + ((((coeffs[11]*c)*lE + coeffs[12]*c2)*lE + coeffs[13]*c3)*lE + coeffs[14]*c4)*lE;
		return(pow(10.,ex));
	}
	
	LegacyPromptFlux::kneeFunction::kneeFunction():
	c0(0),c1(1),x0(0),e0(0){}
	
	double LegacyPromptFlux::kneeFunction::operator()(double energy) const{
		const double lE=log10(energy);
		return(c1 + c0/(1 + exp(x0*(lE-e0))));
	}
	
	std::istream& operator>>(std::istream& is, LegacyPromptFlux::kneeFunction& kf){
		return(is >> kf.c0 >> kf.c1 >> kf.x0 >> kf.e0);
	}
	
	void LegacyPromptFlux::setKneeReweightingModel(std::string reweightModel){
		kneeCorrectionName=reweightModel;
		if(kneeCorrectionName=="none"){
			kneeCorrection=kneeFunction();
			return;
		}
		std::string correctionFilePath=detail::getDataPath("LegacyPromptFlux/"+name+"_"+reweightModel+".dat");
		std::ifstream infile(correctionFilePath.c_str());
		if(!infile)
			throw std::runtime_error("Unable to read "+correctionFilePath+"; are you sure that "
									 +reweightModel+" is a valid knee model for "+name+"?");
		infile >> kneeCorrection;
	}
	
} //namespace NewNuFlux

NNF_REGISTER_FLUX("sarcevic_min",&NewNuFlux::LegacyPromptFlux::makeFlux);
NNF_REGISTER_FLUX("sarcevic_std",&NewNuFlux::LegacyPromptFlux::makeFlux);
NNF_REGISTER_FLUX("sarcevic_max",&NewNuFlux::LegacyPromptFlux::makeFlux);

NNF_REGISTER_KNEE("sarcevic_min","gaisserH3a_elbert");
NNF_REGISTER_KNEE("sarcevic_min","gaisserH4a_elbert");
NNF_REGISTER_KNEE("sarcevic_min","polygonato_mod_elbert");
NNF_REGISTER_KNEE("sarcevic_std","gaisserH3a_elbert");
NNF_REGISTER_KNEE("sarcevic_std","gaisserH4a_elbert");
NNF_REGISTER_KNEE("sarcevic_std","polygonato_mod_elbert");
NNF_REGISTER_KNEE("sarcevic_max","gaisserH3a_elbert");
NNF_REGISTER_KNEE("sarcevic_max","gaisserH4a_elbert");
NNF_REGISTER_KNEE("sarcevic_max","polygonato_mod_elbert");
