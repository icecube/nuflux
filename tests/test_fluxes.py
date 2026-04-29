#!/usr/bin/env python3

import json
import math
import os
import sys
from pathlib import Path

import nuflux
try:
    import numpy as np
    from numpy.testing import assert_allclose
    have_numpy = True
except ImportError:
    have_numpy = False
    def close(a, b, rtol, atol):
        return math.fabs(a - b) <= (atol + rtol * math.fabs(b))
import pytest

def read_models(path):
    with (path).open() as f:
        data = json.load(f)
    return data, [
        (energy, model, knee)
        for energy, v1 in data.items()
        if energy.endswith("_data")
        for model, v2 in v1.items()
        for knee in v2
    ]

data, models = read_models(Path(__file__).parent / "test_data.json")
spline_data, spline_models = read_models(Path(__file__).parent / "test_data_splines.json")

with (Path(__file__).parent / "pion_contrib.json").open() as f:
    pion_data = json.load(f)


def do_test_flux(data, energy, model, knee):
    assert model in nuflux.availableFluxes()
    if knee != "none":
        assert knee in nuflux.kneesForFlux(model)
    flux = nuflux.makeFlux(model)
    flux.knee_reweighting_model = knee
    emin, emax = flux.energy_range
    geom_energy = (emin * emax) ** 0.5
    energies = data["low_energies"] if energy == "low_energy_data" else data["high_energies"]
    coszeniths = data["coszeniths"]
    if have_numpy:
        eng, cz = np.meshgrid(energies, coszeniths)
    for nu_str, nu in nuflux.ParticleType.names.items():
        y = data[energy][model][knee].get(nu_str, 0)
        if have_numpy:
            assert_allclose(flux.getFlux(nu, eng, cz), y, rtol=1e-13, atol=1e-13)
        else:
            for ci, c in enumerate(coszeniths):
                for ei, e in enumerate(energies):
                    assert close(flux.getFlux(nu, e, c), y[ci][ei] if y!=0 else y, 
                                 rtol=1e-13, atol=1e-13)
        assert flux.getFlux(nu, math.nextafter(emin, -math.inf), 0) == 0
        assert flux.getFlux(nu, math.nextafter(emax, +math.inf), 0) == 0
        assert flux.getFlux(nu, geom_energy, -1.0000000000000002) == 0
        assert flux.getFlux(nu, geom_energy, +1.0000000000000002) == 0
    for nu in [11, 13, 15, 17, 22, 2212, 1000260560]:
        with pytest.raises(RuntimeError):
            flux.getFlux(nuflux.ParticleType(nu), geom_energy, 0)
        with pytest.raises(RuntimeError):
            flux.getFlux(nuflux.ParticleType(-nu), geom_energy, 0)


@pytest.mark.parametrize(("energy", "model", "knee"), models)
def test_flux(energy, model, knee):
	do_test_flux(data, energy, model, knee)


@pytest.mark.parametrize(("energy", "model", "knee"), spline_models)
@pytest.mark.skipif("NO_PHOTOSPLINE" in os.environ, reason="requires photospline")
def test_spline_flux(energy, model, knee):
	do_test_flux(spline_data, energy, model, knee)


pik_models = [
    (energy, model, knee)
    for energy, v1 in pion_data.items()
    if energy != "doc"
    for model, v2 in v1.items()
    for knee in v2
]


@pytest.mark.parametrize(("model", "knee", "pion_contrib"), pik_models)
def test_pion_kaon_contributions(model, knee, pion_contrib):
    energies = [1.e+02, 1.e+03, 1.e+04, 1.e+05, 1.e+06, 1.e+07, 1.e+08, 1.e+09]
    coszeniths = [-1, -0.5, 0, 0.5, 1]
    if have_numpy:
        eng, cz = np.meshgrid(energies, coszeniths)
    flux = nuflux.makeFlux(model)
    emin, emax = flux.energy_range
    flux.knee_reweighting_model = knee

    print("testing ", model, knee, pion_contrib)
    flux.relative_pion_contribution = float(pion_contrib)
    flux.relative_kaon_contribution = 1.0
    for ptype, y in pion_data[model][knee][pion_contrib].items():
        pdgid = nuflux.ParticleType.names[ptype]
        if have_numpy:
            np.testing.assert_allclose(
                flux.getFlux(pdgid, eng, cz), y, rtol=1e-13, atol=1e-13
            )
        else:
            for ci, c in enumerate(coszeniths):
                for ei, e in enumerate(energies):
                    assert close(flux.getFlux(pdgid, e, c), y[ci][ei] if y!=0 else y, 
                                 rtol=1e-13, atol=1e-13)


if __name__ == "__main__":
    sys.exit(pytest.main(["-v", __file__, *sys.argv[1:]]))
