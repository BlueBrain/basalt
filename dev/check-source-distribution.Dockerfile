FROM python:3.7

RUN apt-get update \
 && apt-get install -y \
        build-essential \
        cmake \
        doxygen \
        librocksdb-dev \
 && pip3 install -U \
    pip \
    setuptools \
    wheel
