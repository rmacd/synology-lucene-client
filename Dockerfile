FROM alpine:latest

RUN apk add --no-cache \
    git \
    cmake \
    g++ \
    make \
    boost-dev

WORKDIR /usr/src
RUN git clone https://github.com/luceneplusplus/LucenePlusPlus.git ./lpp
WORKDIR /usr/src/lpp
RUN git checkout rel_3.0.9
RUN mkdir build
WORKDIR /usr/src/lpp/build
RUN cmake -DCMAKE_CXX_FLAGS="-DBOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT=1" ..
RUN make -j $(( $( (cat /sys/fs/cgroup/cpuset.cpus.effective 2>/dev/null || cat /sys/fs/cgroup/cpuset/cpuset.effective_cpus) | cut -d '-' -f 2) + 1))
RUN make install

## now build the searchfiles binary
WORKDIR /usr/src/app
COPY . .
RUN mkdir ./build
WORKDIR /usr/src/app/build
RUN cmake -DCMAKE_CXX_FLAGS="-DBOOST_VARIANT_USE_RELAXED_GET_BY_DEFAULT=1" ..
RUN make -j $(( $( (cat /sys/fs/cgroup/cpuset.cpus.effective 2>/dev/null || cat /sys/fs/cgroup/cpuset/cpuset.effective_cpus) | cut -d '-' -f 2) + 1))

## stage 1
FROM alpine:latest
RUN apk add --no-cache boost-system lucene++
COPY --from=0 /usr/src/app/build/searchfiles /usr/local/bin/searchfiles

ENTRYPOINT ["/usr/local/bin/searchfiles"]
CMD ["-index", "index"]
EXPOSE 18080
