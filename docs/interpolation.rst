.. _Interpolation:

Interpolation
#############

Nuflux contains several different methods for interpolating flux tables. Over the years, as better methods were deveopled, the older methods were kept to maintain reproducability with older analyses. This page contains a description of each interpolation method which is available.

New interpolation methods
=========================
.. _New_interpolation_methods:


NewSplineFlux
-------------

The fluxes in this class were calculated with MCEq for several different zenith angles. The 2D array of flux values (one dimension for energy, the other for zenith angles) was interpolated with photospline into a 2D spline surface, stored in a FITS file. Evaluation of the spline surface with nuflux again requires photospline.

Available fluxes in this class:

* :ref:`H3a_SIBYLL21 <H3a_SIBYLL21>`
* :ref:`H3a_SIBYLL23C <H3a_SIBYLL23C>`



Older interpolation methods
===========================
.. _Older_interpolation_methods:

Unfortunately, the documentation for the older interpolation methods is quite incomplete. We are currently trying to find someone to fix this. If you are familiar with any of the below methods, feel free to contribute to the documentation!

ANFlux
------

Evaluation of the fluxes requires photospline.

Available fluxes in this class:

* :ref:`CORSIKA_GaisserH3a <CORSIKA_GaisserH3a>`


Integral Preserving
-------------------

Available fluxes in this class:

* :ref:`IPhonda2006_sno_solmin <IPhonda2006_sno_solmin>`
* :ref:`IPhonda2014_spl_solmin <IPhonda2014_spl_solmin>`


Legacy Conventional
-------------------

This method uses an emerical polynomial model to fit the provided tables. There are 15 parameters which fit
to powers of both :math:`\log_{10}(E)` and :math:`\cos(\theta)`.
The provided tables only contains values up to 1 TeV, to extrapolate the model to higher energies the model of T. Gaisser is used.
The model parameters are fit to the table and the model is used above 700 GeV.

[Gaisser and Honda, Ann. Rev. Nucl. Part. Sci. **52**, 153 (2002). `hep-ph/0203272 <https://arxiv.org/abs/hep-ph/0203272>`_].
[Gaisser T., *Cosmic Rays and Particle Physics*, Cambrige University Press, 1990, p. 88].

Available fluxes in this class:

* :ref:`bartol <bartol>`
* :ref:`honda2006 <honda2006>`


Legacy Prompt
-------------

Available fluxes in this class:

* :ref:`sarcevic <sarcevic>`

Simple Spline
-------------

Evaluation of the fluxes requires photospline.

Available fluxes in this class:

* :ref:`BERSS_H3p <BERSS_H3p>`
