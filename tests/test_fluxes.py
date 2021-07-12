#!/usr/bin/env python3

import unittest
import numpy as np
import nuflux
# Importing local data from test_data.py:
from test_data import low_energy_data, high_energy_data


class TestNuFlux(unittest.TestCase):
    def compare_fluxes(self,data,energies):
        self.N=0

        coszeniths = [-1,-.5,0,.5,1]
        E,cz = np.meshgrid(energies,coszeniths)

        for model, x in data.items():
            flux = nuflux.makeFlux(model)
            for knee,y in x.items():
                flux.knee_reweighting_model=knee
                print("testing ",model,knee)
                for nu, z in y.items():
                    particle=nuflux.ParticleType.names[nu]
                    np.testing.assert_allclose(z,flux.getFlux(particle,E,cz),rtol=1e-13)
                    self.N+=len(E)

        print("{} passed comparisons".format(self.N))

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
