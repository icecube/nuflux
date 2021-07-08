#!/usr/bin/env python3

import numpy as np
import pylab as plt
import nuflux


gamma = 2
fig = plt.figure(figsize=[9.6,4.8])
ax = plt.subplot(111)
flux = nuflux.makeFlux("IPhonda2014_sk_solmin")
erange = flux.energy_range
nutypes = [
        (nuflux.NuMu, 'C0', '-', r"$\nu_\mu$"),
        (nuflux.NuMuBar, 'C1', '-', r"$\overline{\nu}_\mu$"),
        (nuflux.NuE, 'C2', '-', r"$\nu_e$"),
        (nuflux.NuEBar, 'C3', '-', r"$\overline{\nu}_e$")
        ]

for typ, color, line, label in nutypes:

    nu_type=typ
    nu_cos_zenith = 0
    nu_energy= np.logspace(np.log10(erange[0]),np.log10(erange[1]),100)

    fluxvalues = flux.getFlux(nu_type,nu_energy,nu_cos_zenith)

    ax.plot(nu_energy, nu_energy**gamma * fluxvalues,
            label=label, color=color, ls=line)

ax.loglog()
box = ax.get_position()
ax.set_position([box.x0, box.y0+box.height*.05, box.width * 0.7, box.height *.95])
ax.set_xlabel(r"$E_\nu\ [GeV]$",fontsize=12)
ax.set_ylabel(r"$\frac{E^{2}\rm{d}\Phi}{\rm{d}\,E\rm{d}A\,\rm{d}\,\Omega\,\rm{d}t}\ \left[\frac{\rm{GeV}}{\rm{cm}^{2}\,\rm{sr}\,\rm{s}}\right]$",fontsize=18)
ax.set_title(r"HKKMS2014 flux at SuperK")
ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))

plt.show()
# fig.savefig('./scripts/nuflux_types.svg')
