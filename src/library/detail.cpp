#include <string>
#include "config.h"

namespace nuflux{
  namespace detail{
    // Some package relocation strategies (like conda, and maybe only conda)
    // replace string literals containing the build-time installation prefix
    // with a null-padded version of the install-time prefix. The extra layer
    // of indirection below prevents the compiler from pre-computing the
    // length of the string literal prepended to the supplied path, which
    // would include padding nulls, truncating the (null-terminated) sequence
    // returned by c_str().
    const char* getBaseDir() {
        return DATA_DIR "/";
    }
    std::string getDataPath(std::string fname){
      const char* env_p = std::getenv("NUFLUX_DATA");
      if (env_p){
        return std::string(env_p) + "/" + fname;
      }else{
        return getBaseDir() + fname;
      }
    }
  }
}
