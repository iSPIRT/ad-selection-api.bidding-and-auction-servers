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

load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_flatten",
    "container_image",
    "container_layer",
)
load("@io_bazel_rules_docker//contrib:test.bzl", "container_test")
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
        "//services/seller_frontend_service:server",
        "@com_github_grpc_grpc//test/cpp/util:grpc_cli",
        "@jq",
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

container_layer(
    name = "server_binary_layer",
    directory = "/",
    tars = [
        ":server_binaries_tar",
    ],
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

container_layer(
    name = "envoy_config_layer",
    directory = "/",
    tars = [
        ":envoy_config_tar",
    ],
    visibility = [
        "//production/packaging:__subpackages__",
        "//services:__subpackages__",
    ],
)

container_flatten(
    name = "envoy_distroless_flat",
    image = select({
        "@platforms//cpu:arm64": "@envoy-distroless-arm64//image",
        "@platforms//cpu:x86_64": "@envoy-distroless-amd64//image",
    }),
)

container_layer(
    name = "envoy_distroless_layer",
    tars = [
        ":envoy_distroless_flat.tar",
    ],
    visibility = [
        "//production/packaging:__subpackages__",
        "//services:__subpackages__",
    ],
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

container_layer(
    name = "root_certs_layer",
    directory = "/",
    tars = [
        ":root_certs_tar",
    ],
)

container_image(
    name = "server_docker_image",
    base = select({
        "@platforms//cpu:arm64": "@runtime-cc-debian-arm64//image",
        "@platforms//cpu:x86_64": "@runtime-cc-debian-amd64//image",
    }),
    cmd = [
        "/server/bin/init_server_basic",
    ],
    entrypoint = ["sh"],
    labels = {"tee.launch_policy.log_redirect": "always"},
    layers = [
        ":server_binary_layer",
        ":envoy_distroless_layer",
        ":envoy_config_layer",
        ":root_certs_layer",
    ] + select({
        "//:e2e_build": [
        ],
        "//conditions:default": [],
    }),
    # Envoy, SFE, and SFE Non-TLS Healthcheck ports, respectively:
    ports = [
        "51052",
        "50051",
        "50050",
    ],
)

container_test(
    name = "structure_test",
    size = "large",
    configs = ["test/structure.yaml"],
    driver = "tar",
    image = ":server_docker_image",
)

container_test(
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
