#include <string>
#include <boost/python.hpp>

namespace bp = boost::python;

namespace nuflux{
  namespace detail{
    std::string getDataPath(std::string fname){
      bp::object pkg_resources = bp::import("pkg_resources");
      bp::object resource_filename = pkg_resources.attr("resource_filename");
      std::string path = bp::extract<std::string>(resource_filename("nuflux","data/"+fname));
      return path;
    }
  }
}

