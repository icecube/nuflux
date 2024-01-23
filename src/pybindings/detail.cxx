#include <string>
#include <filesystem>
#include <boost/python.hpp>

namespace fs = std::filesystem;
namespace bp = boost::python;

namespace nuflux{
  namespace detail{
    std::string getDataPath(std::string fname){
      std::string basedir = bp::extract<std::string>(bp::import("nuflux").attr("__file__"));
      return fs::path(basedir).parent_path() / "data" / fname;
    }
  }
}
