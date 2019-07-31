#include <nuflux/particleType.h>

#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>

#ifndef NO_HDF5
namespace NewNuFlux{
  hid_t getHDF5TypeForParticleType(){
    //blar, not thread safe; don't care right now
    static bool initialized=false;
    static hid_t type;
    if(initialized)
      return(type);
    
    type=H5Tenum_create(H5T_NATIVE_INT); //TODO: might be non-optimal
    
    I3Particle::ParticleType dummy;
#define reg_value(r,data,value)                                         \
    H5Tenum_insert(type, BOOST_PP_STRINGIZE(value), (dummy=I3Particle::value,&dummy));
    
    BOOST_PP_SEQ_FOR_EACH(reg_value,unused,I3PARTICLE_H_I3Particle_ParticleType);
    
    initialized=true;
    return(type);
  }
} //namesapce NewNuFlux
#endif
