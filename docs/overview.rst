.. _Overview:

Overview
========

nuflux is a library for evaluating various atmospheric neutrino flux models.
Its main purpose is to hold these fluxes, based on different combinations of models,
and make them easily available to analysers.

Models for atmospheric neutrinos are often released as tables binned in neutrino
energy :math:`E_\nu` and zenith angle :math:`\theta_\nu`.
nuflux contains various interpolation routines for these models so that neutrino
fluxes can be represented as a smooth function of energy and zenith.

All models define the flux, :math:`\Phi_\nu(E_\nu,\theta_\nu)`, as a differential flux,
per unit energy, area, solid angle and time:

.. math::
  \Phi_\nu(E_\nu,\theta_\nu) = \frac{{\rm d}N}{{\rm d}E\,{\rm d}\Omega\,{\rm d}A\,{\rm d}t}

All flux values are expressed in units of :math:`\rm{GeV^{-1}\,cm^{-2}\,sr^{-1}\,s^{-1}}`

Several things to note before beginning:

* The returned flux represents the flux incident on the surface of the earth,
  no attempt is made to include oscillations or interaction losses while traveling through the Earth.
* All fluxes are assumed to be symmetrical between down-going and up-going.
* Time dependent effects like solar modulation or seasonal variations on cosmic ray
  air showers are not included. Solar modulation is only relevant below 10 GeV.
* For older prompt neutrino models :math:`\nu_\mu`, :math:`\bar{\nu}_\mu`, :math:`\nu_e`,
  and :math:`\bar{\nu}_e` are all assumed to be equal to each other, :math:`\nu_\tau`
  and :math:`\bar{\nu}_\tau` are different from the other flavors but are equal to each other,
  only a single model for charm and beauty decays is considered.


Physics introduction
--------------------
.. _physics:

The main source of atmospheric leptons, i.e. muons and neutrinos,
are cosmic ray (CR) interactions with Earth's atmosphere.
Cosmic rays are mainly high energy protons and nuclei from within our beyond or Solar System and Galaxy.
There exist different approaches to model the spectrum of cosmic rays, called **primary cosmic ray models** or PRCRs
(“primary” refers to the original high energy particle incident on the atmosphere,
whereas the term “secondary” cosmic rays would describe all lighter particles produced
in interactions further down the line, e.g. air showers).
The selection of the PRCR model impacts the prediction of the atmospheric lepton flux.
Not all existing models are valid over the entire cosmic ray energy spectrum.

The primary cosmic ray particles interact with nuclei in the atmosphere, producing large amounts of hadrons.
The modelling of those hadronic interactions, **hadronic interacton model** or HI,
has a huge impact on the predicted lepton flux.
There are several flux components to be taken into account: The main contribution,
especially in the lower energy region arises from pion and kaon production by cosmic ray interactions in
the atmosphere that decay into leptons, which is called the **conventional flux**.
For higher energies, the atmospheric leptons are mainly produced by the decay of charmed mesons,
which is called the **prompt flux**.

All fluxes provided by nuflux are based on a combination of one PRCR and one HI model.
For more information and features of the available fluxes, see the :ref:`fluxes` section.



Particle encoding
-----------------
.. _Particle_encoding:

To represent the neutrino flavor, nuflux uses `Monte Carlo Particle Numbering Scheme <https://pdg.lbl.gov/2021/reviews/rpp2020-rev-monte-carlo-numbering.pdf>`_ defined by the Particle Data Group. This scheme assigns a unique integer to every particle, and anti-particles always have a negative value. For your convenience nuflux provides enumerated types with these values:

+-----------------------+------------+----------+
| Neutrino flavor       | Name       | PDG Code |
+=======================+============+==========+
| :math:`\nu_e`         |``NuE``     |      +12 |
+-----------------------+------------+----------+
|:math:`\bar{\nu}_e`    |``NuEBar``  |      −12 |
+-----------------------+------------+----------+
| :math:`\nu_\mu`       | ``NuMu``   |      +14 |
+-----------------------+------------+----------+
|:math:`\bar{\nu}_\mu`  | ``NuMuBar``|      −14 |
+-----------------------+------------+----------+
| :math:`\nu_\tau`      | ``NuTau``  |      +16 |
+-----------------------+------------+----------+
|:math:`\bar{\nu}_\tau` |``NuTauBar``|      −16 |
+-----------------------+------------+----------+
