#!/usr/bin/env python3

import json
import sys
from pathlib import Path

import nuflux
import numpy as np
import pytest
from numpy.testing import assert_allclose

with (Path(__file__).parent / "test_data.json").open() as f:
    data = json.load(f)
models = [
    (energy, model, knee)
    for energy, v1 in data.items()
    if energy != "doc"
    for model, v2 in v1.items()
    for knee in v2
]
coszeniths = [-1, -0.5, 0, 0.5, 1]
low_energies = [1.0e00, 1.0e01, 1.0e02, 1.0e03]
high_energies = np.logspace(2, 9, 8)

with (Path(__file__).parent / "pion_contrib.json").open() as f:
    pion_data = json.load(f)


@pytest.mark.parametrize(("energy", "model", "knee"), models)
def test_flux(energy, model, knee):
    flux = nuflux.makeFlux(model)
    flux.knee_reweighting_model = knee
    emin, emax = flux.energy_range
    geom_energy = (emin * emax) ** 0.5
    energies = low_energies if energy == "low_energy_data" else high_energies
    eng, cz = np.meshgrid(energies, coszeniths)
    assert model in nuflux.availableFluxes()
    if knee != "none":
        assert knee in nuflux.kneesForFlux(model)
    for nu_str, nu in nuflux.ParticleType.names.items():
        y = data[energy][model][knee].get(nu_str, 0)
        assert_allclose(flux.getFlux(nu, eng, cz), y, rtol=1e-13, atol=1e-13)
        assert flux.getFlux(nu, np.nextafter(emin, -np.inf), 0) == 0
        assert flux.getFlux(nu, np.nextafter(emax, +np.inf), 0) == 0
        assert flux.getFlux(nu, geom_energy, -1.0000000000000002) == 0
        assert flux.getFlux(nu, geom_energy, +1.0000000000000002) == 0
    for nu in [11, 13, 15, 17, 22, 2212, 1000260560]:
        with pytest.raises(RuntimeError):
            flux.getFlux(nu, geom_energy, 0)
        with pytest.raises(RuntimeError):
            flux.getFlux(-nu, geom_energy, 0)


pik_models = [
    (energy, model, knee)
    for energy, v1 in pion_data.items()
    if energy != "doc"
    for model, v2 in v1.items()
    for knee in v2
]


@pytest.mark.parametrize(("model", "knee", "pion_contrib"), pik_models)
def test_pion_kaon_contributions(model, knee, pion_contrib):
    energies = np.logspace(2, 9, 8)
    coszeniths = [-1, -0.5, 0, 0.5, 1]
    E, cz = np.meshgrid(energies, coszeniths)
    flux = nuflux.makeFlux(model)
    emin, emax = flux.energy_range
    flux.knee_reweighting_model = knee

    print("testing ", model, knee, pion_contrib)
    flux.relative_pion_contribution = float(pion_contrib)
    flux.relative_kaon_contribution = 1.0
    for ptype, y in pion_data[model][knee][pion_contrib].items():
        pdgid = nuflux.ParticleType.names[ptype]
        np.testing.assert_allclose(
            flux.getFlux(pdgid, E, cz), y, rtol=1e-13, atol=1e-13
        )


if __name__ == "__main__":
    sys.exit(pytest.main(["-v", __file__, *sys.argv[1:]]))
