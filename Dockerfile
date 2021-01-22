FROM seamless-python-interop-devcontainer

WORKDIR /usr/local/src/seamless-python-interop
COPY . /usr/local/src/seamless-python-interop
RUN cd /usr/local/src/seamless-python-interop &&\
    mkdir build &&\
    cd build &&\
    cmake .. && make

CMD cd /usr/local/src/seamless-python-interop/build &&\
    python ../callbacks.py