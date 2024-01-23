.. _Development:

Development
#################

If you are simply looking to use nuflux for your analysis, you can safely
disregard this section. :)

If you want to get involved in nuflux, you need access to the
`nuflux repository on github <https://github.com/icecube/nuflux>`_.
Request access on slack or via email from the current development group,
see the :ref:`About` section. Then install nuflux with Meson
(see :ref:`Build from scratch <Build_from_scratch>`).


.. _Project_structure:

Project structure
-----------------

The nuflux module is structured pretty intuitively.
Anyway, here's a short description of the intrastructure:

You find some basic files in the root of the working directory,
like the README and main Meson build file. ``/build`` contains the Ninja build
file and build logs as well as the binaries of compiled programs.
``/docs`` holds the documentation sources
(view the README file to find out how to build the docs for yourself).
All flux data is located in ``/nuflux/data``, where each flux class has its own folder.
``/scripts`` holds some example programs, and ``/tests`` is the location of
the unit test scripts (see section :ref:`Unit tests <Unit_tests>`).
All sources, headers, classes and libraries are stored in ``/src``.


.. _Unit_tests:

Unit tests
----------

When working on nuflux, please make sure that your modifications do not alter the module in an unexpected way,
by executing the unit test scripts in ``/tests`` before committing any changes.
This is to ensure nuflux' integrity and reproducibility, and that the fluxes are backwards-compatible with older analyses.
The scripts contain a simple static list of models and corresponding values for a certain range of
energy and zenith angle that are compared against the calculated values for the same parameters.
If the calculated value for a specific unit differs from the one in the list,
he test for that unit will fail and should be investigated.

**Note:** The C++ test is rather basic, with only single values that are compared.
If possible, use the more elaborated Python script for your unit test.

In C++::

    ninja -C build
    build/test_basic.cxx

In Python::

    python3 tests/test_fluxes.py


.. _Creating_new_fluxes:

Creating new fluxes
-------------------

Since the main purpose of nuflux is to provide atmospheric fluxes to analysers in an easy and simple-to-use manner,
the introduction of new models and fluxes is the major part of developing it.
This section is about creating new fluxes by means of `MCEq <https://github.com/mceq-project/MCEq>`_
and `photospline <https://github.com/icecube/photospline>`_, with the example of a specific combination of models.
Note that not all fluxes are created this way. This is just to give you an idea of how it might work.
If you already have ready-to-go flux tables or splines and are curious on how to integrate them into the project,
see :ref:`Storing new fluxes<Storing_new_fluxes>` instead.

1)  **Choose your weapon**: To make a new flux, there are three ingredients you need to get started:
    A primary cosmic ray (CR) model, a hadronic interaction (HI) model, and the type of particle
    (i.e. atmospheric lepton) you want to make the flux for.
    In this example, we choose ``HillasGaisser2012 (H3a)``, ``SIBYLL23C``, and ``NuMu``.

2)  **Calculate a tabulated spectrum with MCEq**: Together with a zenith angle of our choice
    (note that this is zenith and *not* cosine zenith), we feed our three ingredients to MCEq:

  ::

    from MCEq.core import config, MCEqRun
    import crflux.models as crf

    mceq = MCEqRun( interaction_model = 'SIBYLL23C',
                    primary_model = (crf.HillasGaisser2012, 'H3a'),
                    zenith = 20.0 )
    mceq.solve()
    solution = mceq.get_solution('numu', mag=0)
    energy = mceq.e_grid

  And obtain an array ``solutions``; one value for each value in the ``energy`` array.
  And that's basically it! We can now repeat this for several zenith angles,
  to have a 2D solution array (one dimension being energy, the other being zenith angle).

  **Remark:** MCEq can do a lot more than what is currently part of these instructions.
  For more information and options, please visit `MCEq's GitHub <https://github.com/mceq-project/MCEq>`_.

3)  **Interpolate the solutions with photospline**:
    The photospline software is using B-splining as interpolation method.
    It can turn our 2D solution array into a spline surface that can be evaluated at any desired point on the surface.
    The splining process is pretty neat but may require some practice.
    The spline surface is stored in a FITS file, that, if correctly formatted, can be integrated into nuflux.


**Create fluxes using the script**

Because the flux creation process can get rather complex,
nuflux comes with a handy little tool that can do the above steps for you (and more): ``scripts/create_fluxes``.
The script runs with Python3, and is extensively commented to guide you through it as swiftly as possible.
To better understand what the script does, you can run it with the provided example data
(that resembles an MCEq-created data file) first, before using it for steps 1 and 2.


.. _Storing_new_fluxes:

Storing new fluxes
------------------

In contrast to the previous section, this one should be applicable to all kinds of new fluxes.
Whether you created new fluxes according to the above instructions, or in any other way:
They have to be integrated into nuflux. Here is an example of how that can be accomplished.

1)  **Create a new flux class**: You can either choose one of the existing classes,
    e.g. ``ANFlux``, ``IPLEFlux``, or ``SplineFlux`` if you think your new flux fits the class, or,
    as we do in this example, create a new one (in this case, our flux is a spline flux,
    but has some new features that are not incorporated in the existing ``SplineFlux``).
    We call it ``SplineFlux3``:

  ::

    src/include/nuflux/SplineFlux3.h
    src/library/SplineFlux3.cpp

  It's easiest to copy-paste an existing class and adjust it.
  In this case, we copied the content of ``SplineFlux.cpp`` and added a few lines of code.
  Don't forget to add your class to the meson build file:

  ::

    # ...
    inc = include_directories('src/include')
    libnuflux = library(
      'nuflux',
      'src/library/ANFlux.cpp',
      'src/library/IPLEFlux.cpp',
      'src/library/LegacyConventionalFlux.cpp',
      'src/library/SplineFlux.cpp',
      'src/library/SplineFlux3.cpp',
      'src/library/FluxFunction.cpp',
      'src/library/LegacyPromptFlux.cpp',
      'src/library/logging.cpp',
      'src/library/detail.cpp',
      include_directories : inc,
      dependencies : [photospline,cfitsio],
      install : true)
    # ...

2) **Add the spline surfaces to nuflux**: For each flux class there's a folder with data files in the nuflux working directory. The FITS files that we created earlier go here. In our case::

    nuflux/data/SplineFlux3/

  To integrate these files to nuflux' installation directory, run::

    sudo ninja -C build install

  **Note**: You should provide one flux file for each flux and particle type, e.g. ``H3a_SIBYLL23C_conv_numu.fits``, ``H3a_SIBYLL23C_conv_nutaubar.fits``, etc. If a certain particle type is not supported in your flux, you should implement that accordingly in your flux class.

3) **Register the fluxes**: The last step is to tell the nuflux module about the new fluxes. This is done by adding them to the registry. Add the respective lines to the end of your class file ``SplineFlux3.cpp``, e.g.::

    NNF_REGISTER_FLUX("H3a_SIBYLL23C",&nuflux::SplineFlux3::makeFlux);
    NNF_REGISTER_FLUX("H3a_SIBYLL23C_pr",&nuflux::SplineFlux3::makeFlux);
    NNF_REGISTER_FLUX("H3a_SIBYLL23C_conv",&nuflux::SplineFlux3::makeFlux);
    NNF_REGISTER_FLUX("H3a_SIBYLL23C_k",&nuflux::SplineFlux3::makeFlux);
    NNF_REGISTER_FLUX("H3a_SIBYLL23C_pi",&nuflux::SplineFlux3::makeFlux);

  You need one of these lines for each new flux, but *not* for each particle type. Just omit the particle names at the end. nuflux will call the right file for each particle automatically.
