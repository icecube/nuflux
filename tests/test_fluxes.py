#!/usr/bin/env python3

import unittest
import numpy as np
import nuflux
# Importing local data from test_data.py:
from test_data import low_energy_data, high_energy_data

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
                print("testing ",model,knee)
                for nu_str, nu  in nuflux.ParticleType.names.items():
                    with self.subTest(model = model, knee = knee, nu = nu_str):
                        if nu_str in y:
                            np.testing.assert_allclose(y[nu_str], flux.getFlux(nu, E, cz), rtol = 1e-13)
                        else:
                            np.testing.assert_array_equal(0, flux.getFlux(nu, E, cz))
                        self.assertEqual(flux.getFlux(nu, np.nextafter(emin, -np.inf), 0), 0)
                        self.assertEqual(flux.getFlux(nu, np.nextafter(emax, +np.inf), 0), 0)
                        self.assertEqual(flux.getFlux(nu, geomE, -1.0000000000000002), 0)
                        self.assertEqual(flux.getFlux(nu, geomE, +1.0000000000000002), 0)
                for nu in [ 11, 13, 15, 17, 22, 2212, 1000260560 ]:
                    with self.subTest(model=model,knee=knee,nu=nu_str):
                        with self.assertRaises(Exception):
                            flux.getFlux(nu, geomE, 0)
                        with self.assertRaises(Exception):
                            flux.getFlux(-nu, geomE, 0)

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
