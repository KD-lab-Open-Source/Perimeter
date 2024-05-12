FROM debian:buster

# Create work dir
WORKDIR /work

#Run scripts to prepare env for compilation
COPY ./docker/linux/prepare_pkgs /tmp/prepare_pkgs
RUN bash /tmp/prepare_pkgs && rm /tmp/prepare_pkgs
COPY ./docker/prepare_boost /tmp/prepare_boost
RUN bash /tmp/prepare_boost && rm /tmp/prepare_boost
COPY ./docker/linux/prepare_ffmpeg /tmp/prepare_ffmpeg
RUN bash /tmp/prepare_ffmpeg && rm /tmp/prepare_ffmpeg
COPY ./docker/linux/prepare_sdl_pkgs /tmp/prepare_sdl_pkgs
RUN bash /tmp/prepare_sdl_pkgs && rm /tmp/prepare_sdl_pkgs
COPY ./docker/linux/prepare_sdl /tmp/prepare_sdl
RUN bash /tmp/prepare_sdl && rm /tmp/prepare_sdl
