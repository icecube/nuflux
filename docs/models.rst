

Neutrino Flux Models
====================

The following is a list of models supported by nuflux.


BERSS_H3a
---------

Variants: ``BERSS_H3a_central``

The prompt atmospheric calculation from  Bhattacharya *et al.* assuming the Gaisser H3a cosmic ray spectrum as input. The version included in this library was obtained by fitting a spline to data supplied by the authors of that paper to Joanna Kiryluk on or about 2015-02-09 with the following assumptions: 1) The flux is independent of zenith angle 2) The flux is the same for all flavors of neutrinos (and antineutrinos) except nu_tau, for which it is zero.

Bhattacharya *et al.* J., High Energ. Phys. **2015**, 110 (2015).
`arXiv:1502.01076 <https://arxiv.org/abs/1502.01076>`_.

BERSS_H3p
---------

Variants: ``BERSS_H3p_central``, ``BERSS_H3p_lower``, ``BERSS_H3p_upper``

The family of prompt atmospheric calculation from Bhattacharya *et al.* assuming the 'Gaisser H3p' (all proton) cosmic ray spectrum as input. These fluxes represent the lower, central and upper points on the error band due to QCD uncertainties (cosmic ray flux uncertainties are not included) obtained in that paper. The version given here is constructed in the same manner as for BERSS_H3a_central (with the same assumptions about the angular distribution and neutrino flavors).

CORSIKA_GaisserH3a
------------------

Variants: ``CORSIKA_GaisserH3a_QGSJET-II``, ``CORSIKA_GaisserH3a_SIBYLL-2.1``, ``CORSIKA_GaisserH3a_average``


IPhonda2006_sno_solmin
----------------------

IPhonda2014_spl_solmin
----------------------

bartol
------

The interaction model is published in Barr *et al.* and tables are available from `Here <http://www-pnp.physics.ox.ac.uk/%7Ebarr/fluxfiles/0408i/index.html>`_.

|  Barr *et al.*, Phys. Rev. D **70**, 023006 (2004). `astro-ph/0403630 <https://arxiv.org/abs/astro-ph/0403630>`_.
|  Barr *et al.*, 28th International Cosmic Ray Conferenece (ICRC2003) 1411. `PDF <http://www-rccn.icrr.u-tokyo.ac.jp/icrc2003/PROCEEDINGS/PDF/351.pdf>`_.
   

honda2006
---------
Alternative Knee Models: ``gaisserH3a_elbert``, ``gaisserH4a_elbert``, ``gst13_elbert``, ``gst13star_elbert``, ``polygonato_mod_elbert``

Honda *et al.*, Phys. Rev. D **75**, 043006 (2007).
`astro-ph/0611418 <http://arxiv.org/abs/astro-ph/0611418>`_.

sarcevic
--------
|  Variants: ``sarcevic_max``, ``sarcevic_min``, ``sarcevic_std``
|  Alternative Knee Models: ``gaisserH3a_elbert``, ``gaisserH4a_elbert``, ``polygonato_mod_elbert``

For prompt neutrinos a new model is available by R. Enberg *et al.* This is a pqcd model where parameters where changed so that the model is provided as a best prediction (std) and a max and min ones that provide the uncertainty. Some of the highest predictions (_opt models can be discarded since they seem really too optimistic!)

Enberg *et al.*, Phys. Rev. D **78**, 043005 (2008). `arXiv:0806.0418 <https://arxiv.org/abs/0806.0418>`_
