# run with:
# docker build -t kjmeagher/nuflux-manylinux2014_x86_64 - < manylinux2014.Dockerfile
# python -m cibuildwheel --platform linux
# docker push kjmeagher/nuflux-manylinux2014_x86_64
FROM quay.io/pypa/manylinux2014_x86_64:latest

WORKDIR /
RUN curl -L https://boostorg.jfrog.io/artifactory/main/release/1.84.0/source/boost_1_84_0.tar.bz2 | tar xj
WORKDIR /boost_1_84_0/
RUN ./bootstrap.sh --with-libraries=python --with-python=python3.7 &&\
    ./b2 install variant=release debug-symbols=off install --layout=system &&\
    ./bootstrap.sh --with-libraries=python --with-python=python3.8 &&\
    ./b2 install variant=release debug-symbols=off install --layout=system &&\
    ./bootstrap.sh --with-libraries=python --with-python=python3.9 &&\
    ./b2 install variant=release debug-symbols=off install --layout=system &&\
    ./bootstrap.sh --with-libraries=python --with-python=python3.10 &&\
    ./b2 install variant=release debug-symbols=off install --layout=system &&\
    ./bootstrap.sh --with-libraries=python --with-python=python3.11 &&\
    ./b2 install variant=release debug-symbols=off install --layout=system &&\
    ./bootstrap.sh --with-libraries=python --with-python=python3.12 &&\
    ./b2 install variant=release debug-symbols=off install --layout=system
WORKDIR /
RUN rm -rfv /boost_1_84_0
RUN curl -L https://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/cfitsio-4.3.1.tar.gz | tar xz
WORKDIR /cfitsio-4.3.1
RUN ./configure --prefix=/usr/local && make install
WORKDIR /
RUN rm -rfv /cfitsio-4.3.1
RUN curl -L https://github.com/icecube/photospline/archive/refs/tags/v2.2.1.tar.gz | tar xz
RUN cmake -E env LDFLAGS="-lz" cmake -S photospline-2.2.1 -B photospline_build -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release &&\
    make install -C photospline_build
WORKDIR /
RUN rm -rfv photospline-2.2.1 phosopline_build
