#!/usr/bin/env python3

import json
from pathlib import Path

import numpy as np
import pytest

import nuflux

with (Path(__file__).parent / "test_data.json").open() as f:
    data = json.load(f)


def compare_fluxes(subtests, data, energies):
    coszeniths = [-1, -0.5, 0, 0.5, 1]
    E, cz = np.meshgrid(energies, coszeniths)

    for model, x in data.items():
        flux = nuflux.makeFlux(model)
        emin, emax = flux.energy_range
        geomE = (emin * emax) ** 0.5
        for knee, y in x.items():
            flux.knee_reweighting_model = knee
            print("testing ", model, knee)
            for nu_str, nu in nuflux.ParticleType.names.items():
                with subtests.test(model=model, knee=knee, nu=nu_str):
                    if nu_str in y:
                        np.testing.assert_allclose(flux.getFlux(nu, E, cz), y[nu_str], rtol=1e-13, atol=1e-13)
                    else:
                        np.testing.assert_array_equal(flux.getFlux(nu, E, cz), 0)
                    assert flux.getFlux(nu, np.nextafter(emin, -np.inf), 0) == 0
                    assert flux.getFlux(nu, np.nextafter(emax, +np.inf), 0) == 0
                    assert flux.getFlux(nu, geomE, -1.0000000000000002) == 0
                    assert flux.getFlux(nu, geomE, +1.0000000000000002) == 0
            for nu in [11, 13, 15, 17, 22, 2212, 1000260560]:
                with subtests.test(model=model, knee=knee, nu=nu):
                    with pytest.raises(RuntimeError):
                        flux.getFlux(nu, geomE, 0)
                    with pytest.raises(RuntimeError):
                        flux.getFlux(-nu, geomE, 0)


def test_fluxes(subtests):
    compare_fluxes(subtests, data["high_energy_data"], np.logspace(2, 9, 8))
    compare_fluxes(subtests, data["low_energy_data"], [1.0e00, 1.0e01, 1.0e02, 1.0e03])


if __name__ == "__main__":
    exit(pytest.main(['-v',__file__]))
