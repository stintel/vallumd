FROM debian:8

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -qq update && apt-get -qq upgrade && apt-get -qq install \
	build-essential \
	cmake \
  file \
	libipset-dev \
	libmosquitto-dev \
	libssl-dev \
	pkg-config

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
