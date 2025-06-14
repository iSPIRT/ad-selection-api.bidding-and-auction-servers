# Copyright 2022 Google LLC
# Copyright (C) Microsoft Corporation. All rights reserved.
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

load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")
load("@com_google_cpp_proto_builder//proto_builder:proto_builder.bzl", "cc_proto_builder_library")
load("@rules_buf//buf:defs.bzl", "buf_lint_test")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_descriptor_set", "proto_library")

package(default_visibility = ["//visibility:public"])

proto_library(
    name = "bidding_auction_servers_proto",
    srcs = ["bidding_auction_servers.proto"],
    deps = [
        "//api/udf:generate_bid_proto",
        "@com_google_googleapis//google/api:annotations_proto",
        "@com_google_protobuf//:empty_proto",
        "@com_google_protobuf//:struct_proto",
        "@google_privacysandbox_servers_common//src/logger:logger_proto",
    ],
)

proto_descriptor_set(
    name = "bidding_auction_servers_descriptor_set",
    deps = [":bidding_auction_servers_proto"],
)

cc_proto_library(
    name = "bidding_auction_servers_cc_proto",
    deps = [":bidding_auction_servers_proto"],
)

cc_grpc_library(
    name = "bidding_auction_servers_cc_grpc_proto",
    srcs = [":bidding_auction_servers_proto"],
    grpc_only = True,
    deps = [":bidding_auction_servers_cc_proto"],
)

buf_lint_test(
    name = "base_types_proto_lint",
    size = "small",
    config = ":buf.yaml",
    targets = [":bidding_auction_servers_proto"],
)

proto_library(
    name = "microsoft_bidding_auction_servers_proto",
    srcs = [
        "microsoft_k_anonymity.proto",
        "microsoft_routing_utils.proto",
    ],
    deps = [
        "@com_google_googleapis//google/api:annotations_proto",
        "@com_google_googleapis//google/api:httpbody_proto",
        "@com_google_googleapis//google/rpc:status_proto",
        "@com_google_protobuf//:empty_proto",
        "@com_google_protobuf//:struct_proto",
        "@google_privacysandbox_servers_common//src/logger:logger_proto",
        "@service_value_key_fledge_privacysandbox//public/query/v2:get_values_v2_proto",
    ],
)

proto_descriptor_set(
    name = "microsoft_bidding_auction_servers_descriptor_set",
    deps = [":microsoft_bidding_auction_servers_proto"],
)

cc_proto_library(
    name = "microsoft_bidding_auction_servers_cc_proto",
    deps = [":microsoft_bidding_auction_servers_proto"],
)

cc_grpc_library(
    name = "microsoft_bidding_auction_servers_cc_grpc_proto",
    srcs = [":microsoft_bidding_auction_servers_proto"],
    grpc_only = True,
    deps = [":microsoft_bidding_auction_servers_cc_proto"],
)

buf_lint_test(
    name = "microsoft_bidding_auction_servers_proto_lint",
    size = "small",
    config = ":buf.yaml",
    targets = [":microsoft_bidding_auction_servers_proto"],
)

proto_library(
    name = "k_anon_query_proto",
    srcs = ["k_anon_query.proto"],
    deps = [
        "@com_google_googleapis//google/api:annotations_proto",
        "@com_google_googleapis//google/api:field_behavior_proto",
        "@com_google_googleapis//google/api:httpbody_proto",
        "@com_google_protobuf//:struct_proto",
    ],
)

proto_descriptor_set(
    name = "k_anon_query_descriptor_set",
    deps = [":k_anon_query_proto"],
)

cc_proto_library(
    name = "k_anon_query_cc_proto",
    deps = [":k_anon_query_proto"],
)

cc_grpc_library(
    name = "k_anon_query_cc_grpc_proto",
    srcs = [":k_anon_query_proto"],
    grpc_only = True,
    deps = [":k_anon_query_cc_proto"],
)

buf_lint_test(
    name = "k_anon_base_types_proto_lint",
    size = "small",
    config = ":buf.yaml",
    targets = [":k_anon_query_proto"],
)

cc_proto_builder_library(
    name = "bidding_auction_servers_cc_proto_builder",
    cc_proto_library_deps = [":bidding_auction_servers_cc_proto"],
    proto = ["**"],
)

cc_proto_builder_library(
    name = "k_anon_query_cc_proto_builder",
    cc_proto_library_deps = [":k_anon_query_cc_proto"],
    proto = ["**"],
)
