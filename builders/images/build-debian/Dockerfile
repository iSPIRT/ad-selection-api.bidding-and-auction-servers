# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Use fixed manifest digest to ensure reproducible.
# https://hub.docker.com/layers/library/ubuntu/focal-20240530/images/sha256-85c08a37b74bc18a7b3f8cf89aabdfac51c525cdbc193a753f7907965e310ec2
# ubuntu v20.04, focal-20240530
# DisableDockerDetector "ADO bug ticket to follow"
ARG BASE_IMAGE="mcr.microsoft.com/mirror/docker/library/ubuntu@sha256:fa17826afb526a9fc7250e0fbcbfd18d03fe7a54849472f86879d8bf562c629e"

# ignore this hadolint error as BASE_IMAGE contains an image tag
# hadolint ignore=DL3006
FROM ${BASE_IMAGE} as libprofiler-builder
ENV CC=clang \
    CXX=clang++
ADD https://github.com/gperftools/gperftools/releases/download/gperftools-2.16/gperftools-2.16.tar.gz /build/gperftools.tar.gz
ADD https://apt.llvm.org/llvm.sh /build/llvm.sh
COPY compile_libprofiler /scripts/
RUN /scripts/compile_libprofiler

FROM docker/buildx-bin:v0.10 AS buildx-bin

# ignore this hadolint error as BASE_IMAGE contains an image tag
# hadolint ignore=DL3006
FROM ${BASE_IMAGE}
ARG LOCALE=en_US.UTF-8
ARG TARGETARCH
COPY install_apps install_golang_apps install_go.sh generate_system_bazelrc .bazelversion /scripts/
COPY get_workspace_mount /usr/local/bin
COPY gitconfig /etc
COPY --from=buildx-bin /buildx /usr/libexec/docker/cli-plugins/docker-buildx
COPY --from=libprofiler-builder /usr/lib/libprofiler.so.* /usr/lib/x86_64-linux-gnu/
RUN \
  ln -rs /usr/lib/x86_64-linux-gnu/libprofiler.so.0.* /usr/lib/x86_64-linux-gnu/libprofiler.so.0 && \
  ln -rs /usr/lib/x86_64-linux-gnu/libprofiler.so.0.* /usr/lib/x86_64-linux-gnu/libprofiler.so

ENV BUILD_ARCH="${TARGETARCH}" \
    WORKSPACE=/src/workspace \
    CC=clang \
    TZ=Etc/UTC \
    LANG=${LOCALE} \
    LANGUAGE=${LOCALE} \
    LC_ALL=${LOCALE} \
    LC_CTYPE=${LOCALE}

RUN \
  chmod 644 /etc/gitconfig && \
  /scripts/install_apps --locale ${LOCALE} && \
  /scripts/generate_system_bazelrc --user-root-name ubuntu && \
  /scripts/install_golang_apps && \
  rm -rf /scripts

ENV PATH="/opt/bin:${PATH}:/usr/local/go/bin" \
    PYTHON_BIN_PATH="/opt/bin/python3" \
    PYTHON_LIB_PATH="/usr/lib/python3.9"
