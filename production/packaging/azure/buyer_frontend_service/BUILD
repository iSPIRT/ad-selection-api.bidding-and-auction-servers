# Portions Copyright (c) Microsoft Corporation
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

load("@container_structure_test//:defs.bzl", "container_structure_test")
load("@rules_oci//oci:defs.bzl", "oci_image", "oci_load")
load(
    "@rules_pkg//pkg:mappings.bzl",
    "pkg_attributes",
    "pkg_files",
)
load("@rules_pkg//pkg:tar.bzl", "pkg_tar")
load("@rules_pkg//pkg:zip.bzl", "pkg_zip")

pkg_files(
    name = "server_executables",
    srcs = [
        "bin/init_server_basic",
        "//services/buyer_frontend_service:server",
        "@com_github_grpc_grpc//test/cpp/util:grpc_cli",
    ],
    attributes = pkg_attributes(mode = "0555"),
    prefix = "/server/bin",
)

server_binaries = [
    ":server_executables",
]

pkg_zip(
    name = "server_binaries",
    srcs = server_binaries,
)

pkg_tar(
    name = "server_binaries_tar",
    srcs = server_binaries,
)

pkg_files(
    name = "etc_envoy_files",
    srcs = [
        "bin/envoy.yaml",
        "//api:bidding_auction_servers_descriptor_set",
    ],
    attributes = pkg_attributes(mode = "0444"),
    prefix = "/etc/envoy",
    visibility = [
        "//production/packaging:__subpackages__",
    ],
)

pkg_tar(
    name = "envoy_config_tar",
    srcs = [
        ":etc_envoy_files",
    ],
)

# Flatten the envoy-distroless OCI image into a single filesystem tar.
# This replaces container_flatten from rules_docker.
genrule(
    name = "envoy_distroless_flat",
    srcs = select({
        "@platforms//cpu:arm64": ["@envoy-distroless-arm64//:envoy-distroless-arm64"],
        "@platforms//cpu:x86_64": ["@envoy-distroless-amd64//:envoy-distroless-amd64"],
    }),
    outs = ["envoy_distroless_flat.tar"],
    cmd_bash = """
        set -eo pipefail
        IMG_DIR="$(SRCS)"
        INDEX_JSON="$$IMG_DIR/index.json"
        if [[ ! -f "$$INDEX_JSON" ]]; then
            echo "ERROR: index.json not found in OCI image dir $$IMG_DIR" >&2
            exit 1
        fi
        TMPDIR="$$(mktemp -d)"
        trap "rm -rf $$TMPDIR" EXIT
        MANIFEST_HASH=$$(python3 -c "
import json, sys
with open('$$INDEX_JSON') as f:
    idx = json.load(f)
print(idx['manifests'][0]['digest'].replace('sha256:', ''))
")
        LAYERS=$$(python3 -c "
import json, sys
with open('$$IMG_DIR/blobs/sha256/$$MANIFEST_HASH') as f:
    m = json.load(f)
for l in m['layers']:
    print(l['digest'].replace('sha256:', ''))
")
        for LAYER in $$LAYERS; do
            tar -xzf "$$IMG_DIR/blobs/sha256/$$LAYER" -C "$$TMPDIR" 2>/dev/null || true
        done
        tar -cf "$@" -C "$$TMPDIR" .
    """,
)

pkg_files(
    name = "root_certs",
    srcs = ["@com_github_grpc_grpc//:root_certificates"],
    renames = {
        "@com_github_grpc_grpc//:etc/roots.pem": "etc/ssl/certs/ca-certificates.crt",
    },
)

root_certs_files = [
    ":root_certs",
]

pkg_tar(
    name = "root_certs_tar",
    srcs = root_certs_files,
)

oci_image(
    name = "server_docker_image",
    base = select({
        "@platforms//cpu:arm64": "@runtime-cc-debian-arm64",
        "@platforms//cpu:x86_64": "@runtime-cc-debian-amd64",
    }),
    cmd = [
        "/server/bin/init_server_basic",
    ],
    entrypoint = ["sh"],
    exposed_ports = [
        "51052",
        "50051",
        "50050",
    ],
    labels = {"tee.launch_policy.log_redirect": "always"},
    tars = [
        ":server_binaries_tar",
        ":envoy_distroless_flat",
        ":envoy_config_tar",
        ":root_certs_tar",
    ] + select({
        "//:e2e_build": [
        ],
        "//conditions:default": [],
    }),
)

oci_load(
    name = "server_docker_image_load",
    image = ":server_docker_image",
    repo_tags = [],
)

filegroup(
    name = "server_docker_image.tar",
    srcs = [":server_docker_image_load"],
    output_group = "tarball",
)

container_structure_test(
    name = "structure_test",
    size = "large",
    configs = ["test/structure.yaml"],
    driver = "tar",
    image = ":server_docker_image.tar",
)

container_structure_test(
    name = "commands_test",
    size = "large",
    configs = ["test/commands.yaml"],
    driver = "docker",
    image = ":server_docker_image",
)

# server artifacts
pkg_zip(
    name = "server_artifacts",
    srcs = server_binaries,
)

genrule(
    name = "copy_to_dist",
    srcs = [
        ":server_artifacts",
        ":server_docker_image.tar",
        "//api:bidding_auction_servers_descriptor_set",
    ],
    outs = ["copy_to_dist.bin"],
    cmd_bash = """cat << EOF > '$@'
mkdir -p dist/debian
cp $(execpath :server_artifacts) dist/debian/$$(basename $(RULEDIR))_artifacts.zip
cp $(execpath :server_docker_image.tar) dist/debian/$$(basename $(RULEDIR))_image.tar
cp $(execpath //api:bidding_auction_servers_descriptor_set) dist
builders/tools/normalize-dist
EOF""",
    executable = True,
    local = True,
    message = "copying server artifacts to dist/debian directory",
)
