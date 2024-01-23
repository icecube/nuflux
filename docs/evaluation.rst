.. _Evaluation:

Evaluation methods
##################

nuflux contains several different methods for evaluating flux tables.
Over the years, as better methods were deveopled, the older methods were kept to maintain reproducibility with older analyses.
This page contains a description of each evaluation method which is available.

New evaluation methods
=========================
.. _New_evaluation_methods:


SplineFlux2
-------------

The data for the fluxes in this class was either calculated with MCEq,
or (in some cases) taken from other sources in form of data tables.
The 2D array of flux values (one dimension for energy, the other for zenith angles)
was interpolated with photospline into a 2D spline surface and stored in a FITS file.
Evaluation of the spline surface with nuflux again requires photospline.

So far, this class is pretty similar to the ``SplineFlux`` class,
but has some additional features (e.g. tau flavor support),
with the potential of being extended with further features in the future.
``SplineFlux`` was kept as it is for backwards compatibility.

You can read the physics extents of a particular flux in this class with the ``readExtent()`` method.

Available fluxes in this class:

* :ref:`H3a_SIBYLL21 <H3a_SIBYLL21>`
* :ref:`H3a_SIBYLL23C <H3a_SIBYLL23C>`


Older evaluation methods
===========================
.. _Older_evaluation_methods:

Unfortunately, the documentation for the older evaluation methods is quite incomplete.
We are currently trying to find someone to fix this.
If you are familiar with any of the below methods, feel free to contribute to the documentation!

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
The provided tables only contains values up to 1 TeV,
to extrapolate the model to higher energies the model of T. Gaisser is used.
The model parameters are fit to the table and the model is used above 700 GeV.

[Gaisser and Honda, Ann. Rev. Nucl. Part. Sci. **52**, 153 (2002). `hep-ph/0203272 <https://arxiv.org/abs/hep-ph/0203272>`_].
[Gaisser T., *Cosmic Rays and Particle Physics*, Cambridge University Press, 1990, p. 88].

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

You can read the physics extents of a particular flux in this class with the ``readExtent()`` method.

Available fluxes in this class:

* :ref:`BERSS_H3p <BERSS_H3p>`


.. Reweighting Methods
.. ===================
..
.. Pion to Kaon Ratio
.. ------------------
..
.. Knee Reweighting
.. ----------------
