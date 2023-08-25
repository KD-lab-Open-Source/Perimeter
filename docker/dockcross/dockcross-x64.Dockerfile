FROM dockcross/windows-shared-x64:latest

RUN echo "v2"

ENV DEFAULT_DOCKCROSS_IMAGE dockcross-x64-perimeter

RUN apt-get install software-properties-common lsb-release -y
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 86B72ED9
RUN add-apt-repository "deb [arch=amd64] http://pkg.mxe.cc/repos/apt buster main"
RUN apt-get update
 
# Download boost, untar, setup install with bootstrap and install
RUN cd /work && wget http://downloads.sourceforge.net/project/boost/boost/1.78.0/boost_1_78_0.tar.gz
RUN cd /work && tar xfvz /work/boost_1_78_0.tar.gz
RUN rm /work/boost_1_78_0.tar.gz
RUN cd /work/boost_1_78_0 && ./bootstrap.sh --prefix=/usr/local || true
RUN cd /work/boost_1_78_0 && ./b2 install || true
RUN rm -rf /work/boost_1_78_0

RUN cd /work && git clone https://github.com/ianlancetaylor/libbacktrace.git
RUN cd /work/libbacktrace && ./configure --host=$CROSS_TRIPLE --prefix=/usr/local --enable-static --disable-shared
RUN cd /work/libbacktrace && make -j$NCPU
RUN cd /work/libbacktrace && sudo make install
 
RUN cd /work && wget https://github.com/libsdl-org/SDL/releases/download/release-2.26.5/SDL2-devel-2.26.5-mingw.zip
RUN cd /work && unzip SDL2-devel-2.26.5-mingw.zip
 
RUN cd /work && wget https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-devel-2.6.3-mingw.zip
RUN cd /work && unzip SDL2_mixer-devel-2.6.3-mingw.zip
 
RUN cd /work && wget https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-devel-2.6.3-mingw.zip
RUN cd /work && unzip SDL2_image-devel-2.6.3-mingw.zip
 
RUN cd /work && wget https://github.com/libsdl-org/SDL_net/releases/download/release-2.2.0/SDL2_net-devel-2.2.0-mingw.zip
RUN cd /work && unzip SDL2_net-devel-2.2.0-mingw.zip

RUN cd /work/SDL2-2.26.5 && make install-package arch=x86_64-w64-mingw32 prefix=/usr/local
RUN cd /work/SDL2_mixer-2.6.3 && make install-package arch=x86_64-w64-mingw32 prefix=/usr/local
RUN cd /work/SDL2_image-2.6.3 && make install-package arch=x86_64-w64-mingw32 prefix=/usr/local
RUN cd /work/SDL2_net-2.2.0 && make install-package arch=x86_64-w64-mingw32 prefix=/usr/local
RUN rm -rf /work/SDL2-2.26.5 /work/SDL2_mixer-2.6.3 /work/SDL2_image-2.6.3 /work/SDL2_net-2.2.0

WORKDIR /work

RUN apt-get install -y \
 mxe-x86-64-w64-mingw32.shared-pthread \
 mxe-x86-64-w64-mingw32.static-zlib
