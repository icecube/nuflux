
Implementation Details
######################

Interpolation Methods
=====================

Nuflux contains several different methods for interpolating flux tables.
Over the years, as better methods were deveopled,  the older methods were kept
to maintain reproducability with older analyses. This page contains a description
of each interpolation method which is available.

ANFlux
------

* :ref:`CORSIKA_GaisserH3a`

Requires Photospline

Integral Preserving
-------------------

* :ref:`IPhonda2006_sno_solmin`
* :ref:`IPhonda2014_spl_solmin`

Legacy Conventional
-------------------

* :ref:`bartol`
* :ref:`honda2006`
  
This method uses an emerical polynomial model to fit the provided tables. There are 15 parameters which fit
to powers of both :math:`\log_{10}(E)` and :math:`\cos(\theta)`. 
The provided tables only contains values up to 1 TeV, to extrapolate the model to higher energies the model of T. Gaisser is used.
The model parameters are fit to the table and the model is used above 700 GeV.

|  Gaisser and Honda, Ann. Rev. Nucl. Part. Sci. **52**, 153 (2002). `hep-ph/0203272 <https://arxiv.org/abs/hep-ph/0203272>`_.
|  Gaisser T., *Cosmic Rays and Particle Physics*, Cambrige University Press, 1990, p. 88.

Legacy Prompt
-------------

* :ref:`sarcevic`

Same as Legacy Conventional?

Simple Spline
-------------

* :ref:`BERSS_H3p`

Requires photospline

Reweighting Methods
===================

Pion to Kaon Ratio
------------------

Knee Reweighting
----------------
