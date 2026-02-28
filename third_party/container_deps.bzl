# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("@google_privacysandbox_servers_common//third_party:container_deps.bzl", common_container_deps = "container_deps")
load("@io_bazel_rules_docker//container:container.bzl", "container_pull")

def container_deps():
    common_container_deps()

    images = {
        "aws-lambda-python": {
            "arch_hashes": {
                # 3.9.2022.09.27.12
                "amd64": "c5d5475944755926a3caa6aac4486632f5fed11d531e6437b42dd48718725f29",
                "arm64": "d4d6d56eae30e4f74c6aa617043e2018142b2c4aafa02468653799c891bf86cf",
            },
            "registry": "public.ecr.aws",
            "repository": "lambda/python",
        },
        "envoy-distroless": {
            "arch_hashes": {
                # v1.31.4
                "amd64": "f3e9f6139898db74177ac4f41dabd8750a39724ec28c5762a2c5ac61cc965253",
                "arm64": "ed571f4a0e1ff09617cc845397cf320ceeeda74d9ebaadba78879e80a3008365",
            },
            "registry": "docker.io",
            "repository": "envoyproxy/envoy-distroless",
        },
        "runtime-cc-debian": {
            # debug build so we can use 'sh'
            "arch_hashes": {
                "amd64": "b0e4fa43f85883cb7045a9e9dbe741d3cbe66f4a29de35c40f2768beaf2c3804",
                "arm64": "225cfdcc73b5c80678285de5d53ced9a98e029497b271332bcb26cba85e81ceb",
            },
            "registry": "gcr.io",
            "repository": "distroless/cc-debian12",
        },
        "runtime-debian-slim": {
            "arch_hashes": {
                # stable-20221004-slim
                "amd64": "a4912461baca94ca557af4e779857867a25e0215d157d2dc04f148811e7877f8",
                "arm64": "af9a018b749427a53fded449bd1fbb2cbdc7077d8922b7ebb7bd6478ed40d8e7",
            },
            "registry": "docker.io",
            "repository": "library/debian",
        },
    }

    [
        container_pull(
            name = img_name + "-" + arch,
            digest = "sha256:" + hash,
            registry = image["registry"],
            repository = image["repository"],
        )
        for img_name, image in images.items()
        for arch, hash in image["arch_hashes"].items()
    ]
