FROM archlinux/base

RUN pacman -Syy --noconfirm fmt curl git cmake make gcc catch2 protobuf snappy

ADD install-yay /usr/sbin/install-yay
RUN install-yay docker

RUN su docker -c 'yay -S --noprogressbar --noconfirm benchmark'

ADD . /loki-cpp
WORKDIR /loki-cpp
RUN cmake -Bbuild -H.
RUN cmake --build build --target loki-cpp -- -j4
RUN cmake --build build --target install
RUN cmake --build build --target check    -- -j4
