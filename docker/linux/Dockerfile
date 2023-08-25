FROM debian:buster

# Create work dir
WORKDIR /work

#Run scripts to prepare env for compilation
COPY ./docker/linux/prepare_pkgs /tmp/prepare_pkgs
RUN bash /tmp/prepare_pkgs
COPY ./docker/linux/prepare_ffmpeg /tmp/prepare_ffmpeg
RUN bash /tmp/prepare_ffmpeg
COPY ./docker/linux/prepare_sdl_pkgs /tmp/prepare_sdl_pkgs
RUN bash /tmp/prepare_sdl_pkgs
COPY ./docker/linux/prepare_sdl /tmp/prepare_sdl
RUN bash /tmp/prepare_sdl

#Cleanup
RUN rm /tmp/prepare_pkgs /tmp/prepare_ffmpeg /tmp/prepare_sdl_pkgs /tmp/prepare_sdl
