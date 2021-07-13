#include <iostream>
#include <iomanip>
#include <nuflux/nuflux.h>


using namespace std;

// Add values for new fluxes to the list, with the following parameters:
// ParticleType: NuE, energy: 1e3, cos(zenith): 0
map<string, double> values = {
{"BERSS_H3a_central",             1.522442e-14},
{"BERSS_H3p_central",             1.560076e-14},
{"BERSS_H3p_lower",               1.250199e-14},
{"BERSS_H3p_upper",               1.825986e-14},
{"CORSIKA_GaisserH3a_QGSJET-II",  2.719133e-12},
{"CORSIKA_GaisserH3a_SIBYLL-2.1", 3.563726e-12},
{"CORSIKA_GaisserH3a_average",    3.303177e-12},
{"H3a_SIBYLL21",                  5.020319e-12},
{"H3a_SIBYLL21_K0",               1.463468e-12},
{"H3a_SIBYLL21_K0L",              1.458896e-12},
{"H3a_SIBYLL21_K0S",              5.104548e-15},
{"H3a_SIBYLL21_conv",             5.020319e-12},
{"H3a_SIBYLL21_k",                1.329195e-12},
{"H3a_SIBYLL21_mu",               2.465542e-12},
{"H3a_SIBYLL21_pi",               1.959868e-14},
{"H3a_SIBYLL23C",                 4.383778e-12},
{"H3a_SIBYLL23C_K0",              1.638553e-12},
{"H3a_SIBYLL23C_K0L",             1.633340e-12},
{"H3a_SIBYLL23C_K0S",             5.823607e-15},
{"H3a_SIBYLL23C_conv",            4.350807e-12},
{"H3a_SIBYLL23C_k",               8.010644e-13},
{"H3a_SIBYLL23C_mu",              2.105877e-12},
{"H3a_SIBYLL23C_pi",              1.813244e-14},
{"H3a_SIBYLL23C_pr",              3.908452e-14},
{"IPhonda2006_sno_solmin",        3.719170e-12},
{"IPhonda2014_spl_solmin",        3.905058e-12},
{"IPhonda2014_spl_solmax",        3.905058e-12},
{"IPhonda2014_sk_solmin",         3.814417e-12},
{"IPhonda2014_sk_solmax",         3.814417e-12},
{"bartol",                        7.714551e-12},
{"honda2006",                     3.813934e-12},
{"sarcevic_max",                  3.854341e-14},
{"sarcevic_min",                  1.678407e-14},
{"sarcevic_std",                  3.105402e-14},
};


int main(){
  bool good=1;
  vector<string> fluxes = nuflux::availableFluxes();

  for (vector<string>::iterator it = fluxes.begin() ; it != fluxes.end(); ++it){
    boost::shared_ptr<nuflux::FluxFunction> flux(nuflux::makeFlux(*it));
    double f=flux->getFlux(nuflux::NuE,1e3,0);
    bool pass = abs(f - values[*it])/f < 1e-5 ;
    good = good && pass;
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
