# Portions Copyright (c) Microsoft Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use it except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("@rules_oci//oci:defs.bzl", "oci_image")
load("@rules_oci//oci/private:load.bzl", "oci_load")
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

# Envoy layer: flattened envoy-distroless via crane export (repo rule).
# oci_image tars= doesn't accept select(); use platform-specific images + alias.
# target_compatible_with prevents arm64 image from being built on amd64 (and vice versa).
oci_image(
    name = "server_oci_image_amd64",
    base = "@runtime-cc-debian-amd64",
    target_compatible_with = ["@platforms//cpu:x86_64"],
    cmd = ["/server/bin/init_server_basic"],
    entrypoint = ["sh"],
    env = {
        "GLOG_logtostderr": "1",
        "GLOG_stderrthreshold": "0",
        "GRPC_DNS_RESOLVER": "native",
    },
    exposed_ports = ["51052", "50051", "50050"],
    labels = {"tee.launch_policy.log_redirect": "always"},
    tars = [
        "@envoy_layer//:envoy_amd64",
        ":server_binaries_tar",
        ":envoy_config_tar",
        ":root_certs_tar",
    ],
)

oci_image(
    name = "server_oci_image_arm64",
    base = "@runtime-cc-debian-arm64",
    target_compatible_with = ["@platforms//cpu:arm64"],
    cmd = ["/server/bin/init_server_basic"],
    entrypoint = ["sh"],
    env = {
        "GLOG_logtostderr": "1",
        "GLOG_stderrthreshold": "0",
        "GRPC_DNS_RESOLVER": "native",
    },
    exposed_ports = ["51052", "50051", "50050"],
    labels = {"tee.launch_policy.log_redirect": "always"},
    tars = [
        "@envoy_layer//:envoy_arm64",
        ":server_binaries_tar",
        ":envoy_config_tar",
        ":root_certs_tar",
    ],
)

alias(
    name = "server_oci_image",
    actual = select({
        "@platforms//cpu:arm64": ":server_oci_image_arm64",
        "@platforms//cpu:x86_64": ":server_oci_image_amd64",
    }),
)

oci_load(
    name = "server_image_load",
    image = ":server_oci_image",
    repo_tags = "repo_tags.txt",
)

filegroup(
    name = "server_docker_image_tar",
    srcs = [":server_image_load"],
    output_group = "tarball",
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
        ":server_docker_image_tar",
        "//api:bidding_auction_servers_descriptor_set",
    ],
    outs = ["copy_to_dist.bin"],
    cmd_bash = """cat << EOF > '$@'
mkdir -p dist/debian
cp $(execpath :server_artifacts) dist/debian/$$(basename $(RULEDIR))_artifacts.zip
cp $(execpath :server_docker_image_tar) dist/debian/$$(basename $(RULEDIR))_image.tar
cp $(execpath //api:bidding_auction_servers_descriptor_set) dist
builders/tools/normalize-dist
EOF""",
    executable = True,
    local = True,
    message = "copying server artifacts to dist/debian directory",
)
