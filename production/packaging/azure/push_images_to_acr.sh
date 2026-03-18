#!/bin/bash
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
#
# Squash OCI images to single layer (CCE policy compatibility) and push to ACR.
# Usage:
#   ./push_images_to_acr.sh
#
# Required env vars:
#   BUILD_FLAVOR      e.g. prod, nonprod (default: prod)
#   RELEASE_VERSION   e.g. 4.8.1.0
#   ACR_REGISTRY      e.g. <registry_name>.azurecr.io
#   ACR_REPO_PATH     e.g. depainferencing/azure (registry path under ACR)
#
# Optional:
#   DIST_DIR          Directory containing *_image.tar (default: dist/azure, fallback: dist/debian)
#   WORKSPACE         Repo root (default: script dir/../../..)

set -o pipefail
set -o errexit

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
WORKSPACE="${WORKSPACE:-$(readlink -f "${SCRIPT_DIR}/../../..")}"
DIST_DIR="${DIST_DIR:-${WORKSPACE}/dist/azure}"
if [[ ! -d "${DIST_DIR}" ]]; then
  DIST_DIR="${WORKSPACE}/dist/debian"
fi

BUILD_FLAVOR="${BUILD_FLAVOR:-prod}"
RELEASE_VERSION="${RELEASE_VERSION:-oci-test}"
ACR_REGISTRY="${ACR_REGISTRY:-ispirt.azurecr.io}"
ACR_REPO_PATH="${ACR_REPO_PATH:-depainferencing/azure}"
ACR_BASE="${ACR_REGISTRY}/${ACR_REPO_PATH}"

if [[ -z "${RELEASE_VERSION}" ]]; then
  printf "Error: RELEASE_VERSION must be set\n" >&2
  exit 1
fi

printf "==== Squash and push to ACR ====\n"
printf "  BUILD_FLAVOR: %s\n" "${BUILD_FLAVOR}"
printf "  RELEASE_VERSION: %s\n" "${RELEASE_VERSION}"
printf "  ACR_BASE: %s\n" "${ACR_BASE}"
printf "  DIST_DIR: %s\n" "${DIST_DIR}"
printf "\n"

#######################################
# Squash image to single layer and push
# Arguments:
#   $1 - service name (e.g. bidding_service, buyer_frontend_service)
#   $2 - squash temp prefix (e.g. bidding-squash, bfe-squash)
#   $3 - use WORKDIR / for single layer (1 for BFE/envoy-distroless, 0 otherwise)
#######################################
squash_and_push() {
  local -r SERVICE="$1"
  local -r SQUASH_PREFIX="$2"
  local -r BFE_WORKDIR_FIX="${3:-0}"

  local -r IMAGE_TAR="${DIST_DIR}/${SERVICE}_image.tar"
  if [[ ! -f "${IMAGE_TAR}" ]]; then
    printf "Error: %s not found\n" "${IMAGE_TAR}" >&2
    exit 1
  fi

  local -r ACR_TAG="${ACR_BASE}/${SERVICE//_/-}:${BUILD_FLAVOR}-${RELEASE_VERSION}"

  printf -- "---- %s ----\n" "${SERVICE}"

  LOAD_OUTPUT=$(docker load -i "${IMAGE_TAR}")
  LOADED_IMAGE=$(echo "${LOAD_OUTPUT}" | grep "Loaded image:" | sed 's/Loaded image: //')
  printf "Loaded: %s\n" "${LOADED_IMAGE}"

  docker create --name "${SQUASH_PREFIX}-temp" "${LOADED_IMAGE}"
  docker export "${SQUASH_PREFIX}-temp" -o "${SQUASH_PREFIX}-squashed.tar"
  docker import "${SQUASH_PREFIX}-squashed.tar" "${SQUASH_PREFIX}-base:temp"

  ENTRYPOINT_JSON=$(docker inspect "${LOADED_IMAGE}" --format '{{json .Config.Entrypoint}}')
  CMD_JSON=$(docker inspect "${LOADED_IMAGE}" --format '{{json .Config.Cmd}}')
  ENV_VARS=$(docker inspect "${LOADED_IMAGE}" --format '{{range .Config.Env}}ENV {{.}}{{"\n"}}{{end}}')
  WORKDIR=$(docker inspect "${LOADED_IMAGE}" --format '{{.Config.WorkingDir}}')
  if [[ "${BFE_WORKDIR_FIX}" -eq 1 ]] && [[ "${WORKDIR}" = "/home/nonroot" ]]; then
    WORKDIR="/"
  fi
  WORKDIR="${WORKDIR:-/}"

  {
    echo "FROM ${SQUASH_PREFIX}-base:temp"
    echo "${ENV_VARS}"
    echo "WORKDIR ${WORKDIR}"
    if [[ "${ENTRYPOINT_JSON}" != "null" ]] && [[ -n "${ENTRYPOINT_JSON}" ]]; then
      echo "ENTRYPOINT ${ENTRYPOINT_JSON}"
    fi
    if [[ "${CMD_JSON}" != "null" ]] && [[ -n "${CMD_JSON}" ]]; then
      echo "CMD ${CMD_JSON}"
    fi
  } > Dockerfile."${SQUASH_PREFIX}-squashed"

  docker build -f Dockerfile."${SQUASH_PREFIX}-squashed" -t "${SQUASH_PREFIX}-final:temp" .

  LAYER_COUNT=$(docker inspect "${SQUASH_PREFIX}-final:temp" | jq -r '.[0].RootFS.Layers | length')
  printf "Squashed image has %s layer(s)\n" "${LAYER_COUNT}"

  docker rm "${SQUASH_PREFIX}-temp"
  docker rmi "${SQUASH_PREFIX}-base:temp" 2>/dev/null || true
  rm -f "${SQUASH_PREFIX}-squashed.tar" Dockerfile."${SQUASH_PREFIX}-squashed"

  docker tag "${SQUASH_PREFIX}-final:temp" "${ACR_TAG}"
  printf "Pushing %s to ACR...\n" "${ACR_TAG}"
  docker push "${ACR_TAG}"

  printf "Successfully pushed %s (squashed, %s layer(s))\n" "${ACR_TAG}" "${LAYER_COUNT}"

  docker rmi "${SQUASH_PREFIX}-final:temp" 2>/dev/null || true
  docker rmi "${LOADED_IMAGE}" 2>/dev/null || true

  printf "\n"
}

cd "${WORKSPACE}"

# Bidding service (runtime-cc-debian base: WORKDIR / or unset)
squash_and_push "bidding_service" "bidding-squash" 0

# Buyer frontend service (envoy-distroless base: override /home/nonroot → / for single layer)
squash_and_push "buyer_frontend_service" "bfe-squash" 1

printf "==== Done: both images squashed and pushed to ACR ====\n"
printf "  - %s/bidding-service:%s-%s\n" "${ACR_BASE}" "${BUILD_FLAVOR}" "${RELEASE_VERSION}"
printf "  - %s/buyer-frontend-service:%s-%s\n" "${ACR_BASE}" "${BUILD_FLAVOR}" "${RELEASE_VERSION}"
