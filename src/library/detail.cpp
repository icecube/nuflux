#include <string>
#include "config.h"

namespace nuflux{
  namespace detail{
    std::string getDataPath(std::string fname){
      return (DATA_DIR "/") + fname;
    }
  }
}
