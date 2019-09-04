#ifndef NNF_INTERFACES_H
#define NNF_INTERFACES_H

#include <string>

namespace nuflux{
  
  class KneeReweightable{
  protected:
    std::string kneeCorrectionName;
  public:
    KneeReweightable(std::string kneeName="none"):kneeCorrectionName(kneeName){}
    
    virtual void setKneeReweightingModel(std::string reweightModel)=0;
    std::string getKneeReweightingModel() const{ return(kneeCorrectionName); }

    virtual ~KneeReweightable() {}
  };
  
  class PionKaonAdjustable{
  protected:
    double pionAdjust;
    double kaonAdjust;
  public:
    PionKaonAdjustable(double pionAdjust=1.0, double kaonAdjust=1.0):
      pionAdjust(pionAdjust),kaonAdjust(kaonAdjust){}
    
    ///boosts the contribution from pion decays
    ///\param adjust the factor by which to increase the pion contribution
    virtual void setRelativePionContribution(double adjust)=0;
    double getRelativePionContribution() const{ return(pionAdjust); }
    
    ///boosts the contribution from kaon decays
    ///\param adjust the factor by which to increase the kaon contribution
    virtual void setRelativeKaonContribution(double adjust)=0;
    double getRelativeKaonContribution() const{ return(kaonAdjust); }

    virtual ~PionKaonAdjustable(){}
  };
  
  namespace detail{
    
    void registerKneeModel(const std::string baseModel, const std::string name);
    
    struct KneeRegisterererer{
      KneeRegisterererer(const std::string baseModel, const std::string name){
        registerKneeModel(baseModel,name);
      }
    };
    
  } //namespace detail
  
} //namespace nuflux

#define NNF_REGISTER_KNEE(baseModel,name)                               \
  namespace{ nuflux::detail::KneeRegisterererer BOOST_PP_CAT( registerererer , __LINE__ ) (baseModel,name); }

#endif //NNF_INTERFACES_H
