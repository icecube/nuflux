#!/usr/bin/env python3

import unittest
import numpy as np
import nuflux
# Importing local data from test_data.py:
from test_data import low_energy_data, high_energy_data

test_pdgids = [22, 2212, 12, -12, 14, -14, 16, -16]

class TestNuFlux(unittest.TestCase):
    def compare_fluxes(self,data,energies):
        coszeniths = [-1,-.5,0,.5,1]
        E,cz = np.meshgrid(energies,coszeniths)

        for model, x in data.items():
            flux = nuflux.makeFlux(model)
            emin, emax = flux.energy_range
            geomE=(emin*emax)**.5
            for knee,y in x.items():
                flux.knee_reweighting_model=knee
                pdgids = set(test_pdgids)
                print("testing ",model,knee)
                for nu, z in y.items():
                    particle=nuflux.ParticleType.names[nu]
                    np.testing.assert_allclose(z,flux.getFlux(particle,E,cz),rtol=1e-13)
                    self.assertEqual(flux.getFlux(particle,np.nextafter(emin,-np.inf),0),0)
                    self.assertEqual(flux.getFlux(particle,np.nextafter(emax,+np.inf),0),0)
                    self.assertEqual(flux.getFlux(particle,geomE,-1.0000000000000002),0)
                    self.assertEqual(flux.getFlux(particle,geomE,+1.0000000000000002),0)
                    pdgids.remove(particle)
                for particle in pdgids:
                    with self.assertRaises(Exception):
                        flux.getFlux(particle,geomE,0)



    def test_fluxes(self):
        self.compare_fluxes(
            high_energy_data,
            np.logspace(2,9,8)
        )

        self.compare_fluxes(
            low_energy_data,
            [1.e+00, 1.e+01, 1.e+02, 1.e+03]
        )

if __name__ == '__main__':
    unittest.main()
