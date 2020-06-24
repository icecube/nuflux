#include <iostream>
#include <iomanip>
#include <nuflux/nuflux.h>


using namespace std;

// Add values for new fluxes to the list, with the following parameters:
// ParticleType: NuMu, energy: 1e3, cos(zenith): 0
map<string, double> values = {
  {"BERSS_H3a_central",             1.52244e-14},
  {"BERSS_H3p_central",             1.56008e-14},
  {"BERSS_H3p_lower",                1.2502e-14},
  {"BERSS_H3p_upper",               1.82599e-14},
  {"CORSIKA_GaisserH3a_QGSJET-II",  3.55230e-11},
  {"CORSIKA_GaisserH3a_SIBYLL-2.1", 5.01658e-11},
  {"CORSIKA_GaisserH3a_average",    4.30771e-11},
  {"IPhonda2006_sno_solmin",        4.79297e-11},
  {"IPhonda2014_spl_solmin",        5.04902e-11},
  {"bartol",                        5.48218e-11},
  {"honda2006",                     4.64762e-11},
  {"sarcevic_max",                  3.85434e-14},
  {"sarcevic_min",                  1.67841e-14},
  {"sarcevic_std",                   3.1054e-14},
  {"H3a_SIBYLL21",                  6.85051e-11},
  {"H3a_SIBYLL21_total",            6.85051e-11},
  {"H3a_SIBYLL21_conv",             6.85051e-11},
  {"H3a_SIBYLL21_k",                4.57034e-11},
  {"H3a_SIBYLL21_K0",               7.27295e-13},
  {"H3a_SIBYLL21_pi",               2.13187e-11},
  {"H3a_SIBYLL21_mu",               1.62218e-12},
  {"H3a_SIBYLL21_prompt",           3.72286e-17},
  {"H3a_SIBYLL23C",                 4.80517e-11},
  {"H3a_SIBYLL23C_total",           4.80517e-11},
  {"H3a_SIBYLL23C_conv",            4.80119e-11},
  {"H3a_SIBYLL23C_k",               2.65559e-11},
  {"H3a_SIBYLL23C_K0",              8.22353e-13},
  {"H3a_SIBYLL23C_pi",              1.98839e-11},
  {"H3a_SIBYLL23C_mu",              1.48019e-12},
  {"H3a_SIBYLL23C_prompt",          4.14663e-14},
};


int main(){
  bool good=1;
  vector<string> fluxes = nuflux::availableFluxes();

  for (vector<string>::iterator it = fluxes.begin() ; it != fluxes.end(); ++it){
    boost::shared_ptr<nuflux::FluxFunction> flux(nuflux::makeFlux(*it));
    double f=flux->getFlux(nuflux::NuMu,1e3,0);
    bool pass = abs(f - values[*it])/f < 1e-5 ;
    good *=pass;
    std::cout << std::setw(29) << *it << " " <<f <<" "<< values[*it] << " " << (pass?"PASS":"FAIL") << endl;
  }

  if (good){
    cout << "All test pass" << endl;
    exit(0);
  }else{
    cout << "Some tests fail" << endl;
    exit(-1);
  }
}
