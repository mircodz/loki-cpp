FROM archlinux/base:latest

RUN pacman -Syy --noconfirm fmt curl git cmake make gcc catch2 protobuf snappy

COPY . /loki-cpp
WORKDIR /loki-cpp
RUN cmake -Bbuild -H. -DBUILD_TESTS=ON
RUN cmake --build build --target loki-cpp -- -j4
RUN cmake --build build --target install
RUN cmake --build build --target check    -- -j4
