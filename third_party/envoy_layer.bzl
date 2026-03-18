# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use the License except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Repository rule to extract envoy-distroless image as a flattened tar layer.

Used by BFE/SFE services that need base=runtime-cc-debian (shell) + Envoy.
rules_oci lacks container_flatten; crane export produces the flattened
filesystem tar required for oci_image tars=[].
"""


# envoy-distroless digests from container_deps.bzl (v1.31.4)
ENVOY_AMD64_DIGEST = "sha256:f3e9f6139898db74177ac4f41dabd8750a39724ec28c5762a2c5ac61cc965253"
ENVOY_ARM64_DIGEST = "sha256:ed571f4a0e1ff09617cc845397cf320ceeeda74d9ebaadba78879e80a3008365"
ENVOY_IMAGE = "docker.io/envoyproxy/envoy-distroless"

# Crane v0.20.2 (matches install_golang_apps)
CRANE_LINUX_X86_64_SHA = "c14340087103ba9dadf61d45acd20675490fd0ccbd56ac7901fc1b502137f44b"
CRANE_LINUX_X86_64_URL = "https://github.com/google/go-containerregistry/releases/download/v0.20.2/go-containerregistry_Linux_x86_64.tar.gz"
CRANE_LINUX_ARM64_SHA = "aff0db48825124c9331ea310057214bd4e92c01aa2e414d539e9659841d9422a"
CRANE_LINUX_ARM64_URL = "https://github.com/google/go-containerregistry/releases/download/v0.20.2/go-containerregistry_Linux_arm64.tar.gz"

def _envoy_layer_impl(repository_ctx):
    os_arch = repository_ctx.os.arch
    if os_arch == "aarch64" or os_arch == "arm64":
        sha256, url = CRANE_LINUX_ARM64_SHA, CRANE_LINUX_ARM64_URL
    else:
        sha256, url = CRANE_LINUX_X86_64_SHA, CRANE_LINUX_X86_64_URL

    # Download crane
    repository_ctx.download(
        url = [url],
        output = "crane.tar.gz",
        sha256 = sha256,
    )

    # Extract (tarball has crane, gcrane at top level)
    result = repository_ctx.execute(["tar", "-xzf", "crane.tar.gz"])
    if result.return_code != 0:
        fail("Failed to extract crane: " + result.stderr)

    crane = repository_ctx.path("crane")

    # Export envoy-distroless for each platform (crane can pull any platform)
    # crane export IMAGE OUTPUT_FILE [flags] - output is positional, not -o
    for platform, digest, out in [
        ("linux/amd64", ENVOY_AMD64_DIGEST, "envoy_amd64.tar"),
        ("linux/arm64", ENVOY_ARM64_DIGEST, "envoy_arm64.tar"),
    ]:
        ref = ENVOY_IMAGE + "@" + digest
        result = repository_ctx.execute([
            str(crane),
            "export",
            "--platform=" + platform,
            ref,
            out,
        ])
        if result.return_code != 0:
            fail("crane export failed for %s: %s" % (platform, result.stderr))

    # BUILD file - visibility public so BFE/SFE packages can depend
    repository_ctx.file("BUILD", """
filegroup(
    name = "envoy_amd64",
    srcs = ["envoy_amd64.tar"],
    visibility = ["//visibility:public"],
)
filegroup(
    name = "envoy_arm64",
    srcs = ["envoy_arm64.tar"],
    visibility = ["//visibility:public"],
)
""")

_envoy_layer = repository_rule(
    implementation = _envoy_layer_impl,
    attrs = {},
)

def envoy_layer_repositories():
    _envoy_layer(name = "envoy_layer")
