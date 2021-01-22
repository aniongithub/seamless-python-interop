# Dockerfile to set up a VS Code + Docker based native development environment
FROM ubuntu:20.04

# So programs like tzdata don't stop the image build process
ARG DEBIAN_FRONTEND=noninteractive

# https://stackoverflow.com/a/28406007/802203
# Set the locale
RUN apt-get update &&\
    apt-get install -y locales &&\
    sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    locale-gen

# Some basic tools
RUN apt-get update &&\
    apt-get install -y \
        apt-utils \
        wget nano jq \
        build-essential gdb \
        git

# Install Python3, pip3 and make them default
RUN apt-get update &&\
    apt-get install -y \
        curl python3.8 python3.8-dev python3-distutils cython3 &&\
    ln -sfn $(which python3.8) /usr/local/bin/python &&\
    ln -sfn $(which python3.8) /usr/local/bin/python3 &&\
    curl https://bootstrap.pypa.io/get-pip.py -o /tmp/get-pip.py &&\
    python3 /tmp/get-pip.py &&\
    ln -sfn $(which pip3) /usr/local/bin/pip

# Remove any old versions and install latest version of CMake using pip
# https://cmake-python-distributions.readthedocs.io/en/latest/
RUN pip3 install --upgrade cmake

# Install ctypesgen
ARG CTYPESGEN_TAG=ctypesgen-1.0.2
WORKDIR /usr/local/src/ctypesgen
RUN cd /usr/local/src &&\
    git clone https://github.com/davidjamesca/ctypesgen.git &&\
    cd ctypesgen &&\
    git checkout ${CTYPESGEN_TAG} &&\
    python3 setup.py install

# Install clang and clang-format for header processing
RUN apt-get install -y \
    build-essential clang clang-format

# Install ctypesgen to pxd for Cython bindings
ARG CTYPESGEN_TO_PXD_TAG=feature-force-forwards
WORKDIR /usr/local/src/ctypesgen_to_pxd
RUN cd /usr/local/src &&\
    git clone https://github.com/aniongithub/ctypesgen_to_pxd.git &&\
    cd ctypesgen_to_pxd &&\
    git checkout ${CTYPESGEN_TO_PXD_TAG} &&\
    ln -sfn /usr/local/src/ctypesgen_to_pxd/ctypesgen_to_pxd.py /usr/local/bin/ctypesgen_to_pxd