#ifndef NEWNUFLUX_H
#define NEWNUFLUX_H

#include <vector>
#include <nuflux/FluxFunction.h>

namespace nuflux{

  ///Constructs a FluxFunction object for the named flux
  /// \fluxName the name of the flux being requested
  /// \throws std::invalid_argument if fluxName does not correspond to a known flux
  boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);

  ///Returns a list of the names of the supported flux models
  ///(which may be used when calling makeFlux)
  std::vector<std::string> availableFluxes();

  ///Returns a list of the names of the supported cosmic ray knee
  ///reweighting models for the given base neutrino flux
  ///\param fluxName the base neutrino flux
  std::vector<std::string> kneesForFlux(std::string fluxName);

  ///Print an listing of all supported models
  void printModels();

  ///Return the current nuflux version
  std::string getVersion();

} //namespace nuflux


std::string getDataPath(std::string fname);

#include <nuflux/Fluxes.h>

#endif //NEWNUFLUX_H
