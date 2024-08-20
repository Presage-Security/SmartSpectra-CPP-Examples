FROM --platform=linux/amd64 ubuntu:22.04 as build

ENV TZ=US/Eastern
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install dependencies
# TODO: (why) is numpy necessary for this build?
RUN apt update && apt upgrade -y && apt install nodejs npm -y && apt install -y --no-install-recommends \
    git \
    wget \
    apt-transport-https \
    curl \
    gnupg \
    libegl1-mesa-dev \
    python3-pip  \
    libglew-dev \
    default-jdk \
    libusb-1.0-0-dev && pip install numpy --no-cache-dir

# Install Bazelisk
ARG BAZELISK_VERSION=1.18.0
ARG BAZELISK_SHA256_SUM=ce52caa51ef9e509fb6b7e5ad892e5cf10feb0794b0aed4d2f36adb00a1a2779
RUN wget -O /usr/bin/bazel \
         https://github.com/bazelbuild/bazelisk/releases/download/v${BAZELISK_VERSION}/bazelisk-linux-amd64 && \
    echo "${BAZELISK_SHA256_SUM}  /usr/bin/bazel" sha256sum --check - && \
    chmod +x /usr/bin/bazel


WORKDIR /code

ENV GLOG_logtostderr=1

COPY . .
RUN ./setup_opencv.sh --ci

RUN bazel build --copt=-O3  --define MEDIAPIPE_DISABLE_GPU=1 --spawn_strategy=standalone //applications/cpp_file_example

FROM --platform=linux/amd64 ubuntu:22.04

WORKDIR /code

COPY --from=build /code/bazel-bin/applications/cpp_file_example/cpp_file_example /code/preprocess
COPY --from=build /code/graph /code/graph
COPY --from=build /code/mediapipe/modules /code/mediapipe/modules
COPY --from=build /code/setup_opencv.sh /code/

ENV TZ=US/Eastern
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install opencv dependencies
RUN apt update && apt upgrade -y && apt install -y --no-install-recommends \
  libusb-1.0-0-dev \
  libopencv-core-dev \
  libopencv-imgproc-dev \
  libopencv-video-dev \
  libopencv-highgui-dev \
  libopencv-videoio-dev \
  libopencv-contrib-dev \
  && rm -rf /var/lib/apt/lists/*

RUN ./setup_opencv.sh --ci

CMD ["/code/preprocess", "--headless", "--also_log_to_stderr", "--start_with_recording_on", "--file_stream_path=/data_in/frame_0000000000000000.png", "--also_log_to_stderr",  "--erase_read_files=true", "--file_stream_rescan_delay=5", "--buffer_duration=0.5", "--output_path=/data_out/", "--status_file_directory_path=/data_out/"]

