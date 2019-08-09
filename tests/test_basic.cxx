#include <iostream>
#include <iomanip>

#include <nuflux/nuflux.h>

using namespace std;

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
  {"honda2012_spl_solmin",                    0},
  {"sarcevic_max",                  3.85434e-14},
  {"sarcevic_min",                  1.67841e-14},
  {"sarcevic_std",                   3.1054e-14},
};

  
int main(){
  bool good=1;
  vector<string> fluxes = nuflux::availableFluxes();

  for (vector<string>::iterator it = fluxes.begin() ; it != fluxes.end(); ++it){
    if (*it=="honda2012_spl_solmin") {
        continue;
      }
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
