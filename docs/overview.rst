Overview
========

Nuflux is a library for evaluating various atmospheric neutrino flux models.
Models for atmospheric neutrinos are often released as tables binned in neutrino energy, :math:`E_\nu` and zenith angle :math:`\theta_\nu`.
Nuflux contains various interpolation routines for these models so that neutrino fluxes can be
represented as a smooth function of energy and zenith.

All models define the flux, :math:`\Phi_\nu(E_\nu,\theta_\nu)`, as a differential flux, per unit energy, area, solid angle and time:

.. math:: 
  \Phi_\nu(E_\nu,\theta_\nu) = \frac{{\rm d}N}{{\rm d}E\,{\rm d}\Omega\,{\rm d}A\,{\rm d}t}

All flux values are expressed in units of :math:`\rm{GeV^{-1}\,cm^{-2}\,sr^{-1}\,s^{-1}}`

Several things to note before beginning:

* The returned flux represents the flux incident on the surface of the earth, no attempt is made to include oscillations or interaction losses while traveling through the Earth.
* All fluxes are assumed to be symmetrical between down-going and up-going.
* Time dependent effects like solar modulation or seasonal variations on cosmic ray air showers are not included. Solar modulation is only relevant below 10 GeV
* For prompt neutrino models :math:`\nu_\mu`, :math:`\bar{\nu}_\mu`, :math:`\nu_e`, and :math:`\bar{\nu}_e` are all assumed to be equal to eachother, :math:`\nu_\tau` and :math:`\bar{\nu}_\tau` are different from the other flavors bur are equal to each other, only a single model for charm and beauty decays is considered.

Particle Encoding
-----------------

To represent the neutrino flavor, nuflux uses `Monte Carlo Particle Numbering Scheme <http://pdg.lbl.gov/mc_particle_id_contents.html>`_ defined by the Particle Data Group. This scheme assigns a unique integer to every particle, and anti-particles always have a negitive value. For your convience nuflux provides enumerated types with these values:

+-----------------------+------------+----------+
| neutrino flavor       | enum       | PDG Code |
+=======================+============+==========+
| :math:`\nu_e`         |``NuE``     |       12 | 
+-----------------------+------------+----------+
|:math:`\bar{\nu}_e`    |``NuEBar``  |      -12 |
+-----------------------+------------+----------+
| :math:`\nu_\mu`       | ``NuMu``   |       14 | 
+-----------------------+------------+----------+
|:math:`\bar{\nu}_\mu`  | ``NuMuBar``|      -14 |
+-----------------------+------------+----------+
| :math:`\nu_\tau`      | ``NuTau``  |       16 | 
+-----------------------+------------+----------+
|:math:`\bar{\nu}_\tau` |``NuTauBar``|      -16 |
+-----------------------+------------+----------+      

