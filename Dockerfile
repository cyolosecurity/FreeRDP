FROM dorowu/ubuntu-desktop-lxde-vnc:focal AS xfreerdp-instruction-server-compiler

RUN mkdir -p /src/FreeRDP{channels ci client compat external include libfreerdp packaging rdtk resources server third-party tools uwac winpr cmake docs scripts}


COPY CMakeCPack.cmake CMakeCPackOptions.cmake.in CMakeLists.txt LICENSE /src/FreeRDP/
COPY channels /src/FreeRDP/channels
COPY ci /src/FreeRDP/ci
COPY client /src/FreeRDP/client
COPY compat /src/FreeRDP/compat
COPY external /src/FreeRDP/external
COPY include /src/FreeRDP/include
COPY libfreerdp /src/FreeRDP/libfreerdp
COPY packaging /src/FreeRDP/packaging
COPY rdtk /src/FreeRDP/rdtk
COPY resources /src/FreeRDP/resources
COPY server /src/FreeRDP/server
COPY third-party /src/FreeRDP/third-party
COPY tools /src/FreeRDP/tools
COPY uwac /src/FreeRDP/uwac
COPY winpr /src/FreeRDP/winpr
COPY cmake /src/FreeRDP/cmake
COPY docs /src/FreeRDP/docs
COPY scripts /src/FreeRDP/scripts

RUN apt-get update --allow-insecure-repositories # Don't run upgrade

RUN apt-get install -y build-essential zlib1g-dev pkg-config ffmpeg cmake ninja-build libssl-dev libavcodec-dev libavformat-dev libavfilter-dev heimdal-dev libicu-dev xorg-dev libcups2-dev libfuse3-dev xsltproc docbook-xsl libusb-1.0-0-dev libcjson-dev clang-format liburiparser-dev libmicrohttpd-dev libjansson-dev

RUN cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja -S /src/FreeRDP -B /FreeRDP

RUN cmake --build /FreeRDP  --target xfreerdp -j 4



FROM dorowu/ubuntu-desktop-lxde-vnc:focal AS xfreerdp-instruction-server

RUN apt-get update --allow-insecure-repositories # Don't run upgrade

RUN apt-get install -y dnsutils zlib1g-dev ffmpeg libssl-dev libavcodec-dev libavformat-dev libavfilter-dev heimdal-dev libicu-dev xorg-dev libcups2-dev libfuse3-dev xsltproc docbook-xsl libusb-1.0-0-dev libcjson-dev clang-format liburiparser-dev libmicrohttpd-dev libjansson-dev

RUN mkdir /FreeRDP

WORKDIR /FreeRDP

COPY --from=xfreerdp-instruction-server-compiler /FreeRDP /FreeRDP

COPY  xfreerdp.sh add-to-supervisord.conf /FreeRDP

RUN cat ./add-to-supervisord.conf >> /etc/supervisor/conf.d/supervisord.conf
