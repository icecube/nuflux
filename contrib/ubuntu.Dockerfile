FROM ubuntu:latest

#install dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get -y install build-essential git cmake ninja-build pkg-config libcfitsio-dev python-is-python3 libboost-python-dev python3-numpy python3-pip
RUN pip3 install meson

#checkout photospline
RUN git clone https://github.com/icecube/photospline.git

#build photospline
WORKDIR /photospline
RUN cmake .
RUN make install

#checkout nuflux
WORKDIR /
RUN git clone https://github.com/icecube/nuflux.git

#build nuflux
RUN mkdir /build
WORKDIR /build
RUN meson /nuflux
RUN ninja install

#test nuflux
ENV LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu:/usr/local/lib
ENV PYTHONPATH=/usr/local/lib/python3.8/site-packages
RUN ninja test
