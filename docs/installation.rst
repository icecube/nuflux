.. _Installtion:

Installation
############

Please follow the installation instructions below.
If you have problems, refer to the :ref:`Known problems <Known_problems>` section.
If that doesn't help to resolve your issue, feel free to contact the developers (see :ref:`About <About>` section).


Prerequisites
-------------
.. _Prerequisites:

The nuflux module requires the following packages:

* **cfitsio** from NASA's website, `https://heasarc.gsfc.nasa.gov/fitsio <https://heasarc.gsfc.nasa.gov/fitsio/>`_.
  Installation instructions are included in the tar ball.
* **photospline** from `https://github.com/icecube/photospline <https://github.com/icecube/photospline>`_.
  Access to icecube required.
* **libboost-python-dev** obtained via package manager.


Python
------
.. _Python:

Installation via Python3-pip::

  pip3 install --user git+https://github.com/icecube/nuflux

If you want to install from a specific branch::

  pip3 install --user git+https://github.com/icecube/nuflux.git@<branch>

Build from scratch
---------------------
.. _Build_from_scratch:

If you want to run nuflux from a c++ program, and/or you want to get involved in the nuflux development,
it has to be installed with meson and ninja.

::

  git clone https://github.com/icecube/nuflux
  cd nuflux
  meson build
  ninja -C build
  sudo ninja -C build install


Known problems
--------------
.. _Known_problems:


Here are some errors that have been encountered with Python and Meson installation,
and the steps taken to resolve them:

* **photospline and boost are installed in non-standard locations**

  The Python installation may fail if photospline and boost_python are installed in non-standard locations,
  e.g. headers in ``/opt/toolsets/mystuff/include``, libraries in ``/opt/toolsets/mystuff/lib``.
  In this case, set the ``CMAKE_PREFIX_PATH`` and ``BOOST_ROOT`` environment variables to the root of your toolset installation:

  ::

    CMAKE_PREFIX_PATH=/opt/toolsets/photospline and `BOOST_ROOT`=/opt/toolsets/boost pip install --user git+https://github.com/icecube/nuflux


* **Meson is out of date**

  ::

    meson.build:6:0: ERROR: method 'cmake' is invalid

  The problem here is (most likely) actually not with cmake, but with meson.
  Try to update meson. On Ubuntu 18.04 for instance, the default meson version is 0.29,
  which is like half a century old. Remove the old version with the package manager,
  and get the newest one with Python, like:

  ::

    pip3 install --user meson==0.53

* **Meson is trying to use an old Python**

  ::

    Message: Numpy found: /usr/local/lib/python2.7/dist-packages/numpy/core/include
    ERROR: Invalid Boost modules: python27 Run-time dependency Boost (python27) found: NO
    meson.build:69:2: ERROR: Dependency "boost" not found

  It's trying to use Python27 while we want to use it with Python3.
  This a relatively easy fix, just open the ``meson_options.txt`` and change the value of the Python option,
  like:

  ::

    option('python', type : 'string', value : 'python3', description : 'python path to compile against')

* **Missing softlink to boost**

  Error message for Python installation:

  ::

    /usr/bin/ld: cannot find -lboost_python36

  Error message for Meson installation:

  ::

    Message: Numpy found: /usr/local/lib/python3.6/dist-packages/numpy/core/include
    ERROR: Invalid Boost modules: python36 Run-time dependency Boost (python36) found: NO

    meson.build:69:2: ERROR: Dependency "boost" not found

  For some reason boost seems to be missing, which we clearly installed earlier.
  The library simply has an unexpected name, so it's enough to create a softlink, e.g.:

  ::

    cd /usr/lib/x86_64-linux-gnu/
    sudo ln -s libboost_python-py36.so libboost_python36.so
