# Copyright (c) Microsoft Corporation
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

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("//:config.bzl", "ENABLE_CORE_DUMPS_DEFINES")

cc_library(
    name = "runtime_flags",
    hdrs = [
        "runtime_flags.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//services/common/constants:common_service_flags",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "k_anon_reactors",
    srcs = [
        "join_reactor.cc",
        "query_reactor.cc",
    ],
    hdrs = [
        "base_kanon_reactor.h",
        "join_reactor.h",
        "query_reactor.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//api:microsoft_bidding_auction_servers_cc_grpc_proto",
        "//services/common:feature_flags",
        "//services/common/clients/kv_server:kv_async_client",
        "//services/common/code_dispatch:code_dispatch_reactor",
        "//services/common/metric:server_definition",
        "//services/common/util:json_util",
        "//services/common/util:request_metadata",
        "//services/common/util:request_response_constants",
        "//services/k_anonymity_service/benchmarking:k_anonymity_benchmarking_logger",
        "//services/k_anonymity_service/benchmarking:k_anonymity_no_op_logger",
        "//services/k_anonymity_service/data:runtime_config",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@google_privacysandbox_servers_common//src/logger:request_context_impl",
        "@google_privacysandbox_servers_common//src/util/status_macro:status_macros",
        "@google_privacysandbox_servers_common//src/util/status_macro:status_util",
        "@rapidjson",
    ],
)

cc_test(
    name = "k_anon_query_reactor_test",
    size = "small",
    srcs = ["query_reactor_test.cc"],
    deps = [
        ":k_anon_reactors",
        "//services/common/constants:common_service_flags",
        "//services/common/encryption:key_fetcher_factory",
        "//services/common/encryption:mock_crypto_client_wrapper",
        "//services/common/test:mocks",
        "//services/common/test:random",
        "//services/common/test/utils:test_init",
        "//services/k_anonymity_service/benchmarking:k_anonymity_benchmarking_logger",
        "//services/k_anonymity_service/benchmarking:k_anonymity_no_op_logger",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@google_privacysandbox_servers_common//src/encryption/key_fetcher:key_fetcher_manager",
        "@google_privacysandbox_servers_common//src/encryption/key_fetcher/mock:mock_key_fetcher_manager",
    ],
)

cc_test(
    name = "k_anon_join_reactor_test",
    size = "small",
    srcs = ["join_reactor_test.cc"],
    deps = [
        ":k_anon_reactors",
        "//services/common/constants:common_service_flags",
        "//services/common/encryption:key_fetcher_factory",
        "//services/common/encryption:mock_crypto_client_wrapper",
        "//services/common/test:mocks",
        "//services/common/test:random",
        "//services/common/test/utils:test_init",
        "//services/k_anonymity_service/benchmarking:k_anonymity_benchmarking_logger",
        "//services/k_anonymity_service/benchmarking:k_anonymity_no_op_logger",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@google_privacysandbox_servers_common//src/encryption/key_fetcher:key_fetcher_manager",
        "@google_privacysandbox_servers_common//src/encryption/key_fetcher/mock:mock_key_fetcher_manager",
    ],
)

cc_test(
    name = "k_anonymity_service_test",
    size = "small",
    srcs = ["k_anonymity_service_test.cc"],
    deps = [
        ":k_anon_reactors",
        ":k_anonymity_service",
        "//services/common/encryption:key_fetcher_factory",
        "//services/common/encryption:mock_crypto_client_wrapper",
        "//services/common/test:mocks",
        "//services/common/test/utils:service_utils",
        "//services/common/test/utils:test_init",
        "//services/k_anonymity_service/benchmarking:k_anonymity_benchmarking_logger",
        "//services/k_anonymity_service/benchmarking:k_anonymity_no_op_logger",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@google_privacysandbox_servers_common//src/encryption/key_fetcher:key_fetcher_manager",
    ],
)

cc_library(
    name = "k_anonymity_service",
    srcs = [
        "k_anonymity_service.cc",
    ],
    hdrs = [
        "k_anonymity_service.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":k_anon_reactors",
        ":runtime_flags",
        "//api:microsoft_bidding_auction_servers_cc_grpc_proto",
        "//api:microsoft_bidding_auction_servers_cc_proto",
        "//services/common/clients/config:config_client",
        "//services/common/metric:server_definition",
        "@aws_sdk_cpp//:core",
        "@com_github_grpc_grpc//:grpc++",
        "@google_privacysandbox_servers_common//src/encryption/key_fetcher:key_fetcher_manager",
        "@google_privacysandbox_servers_common//src/public/cpio/interface:cpio",
        "@google_privacysandbox_servers_common//src/telemetry",
    ],
)

cc_binary(
    name = "server",
    srcs = ["k_anonymity_main.cc"],
    linkopts = [
        "-Wl,-rpath,\\$$ORIGIN/../lib",
    ],
    local_defines = ENABLE_CORE_DUMPS_DEFINES,
    malloc = "@com_google_tcmalloc//tcmalloc",
    visibility = ["//visibility:public"],
    deps = [
        ":k_anonymity_service",
        "//api:microsoft_bidding_auction_servers_cc_grpc_proto",
        "//api:microsoft_bidding_auction_servers_cc_proto",
        "//services/common/blob_fetch:blob_fetcher",
        "//services/common/clients/config:config_client_util",
        "//services/common/encryption:crypto_client_factory",
        "//services/common/encryption:key_fetcher_factory",
        "//services/common/telemetry:configure_telemetry",
        "//services/common/util:file_util",
        "//services/common/util:request_response_constants",
        "//services/common/util:tcmalloc_utils",
        "//services/k_anonymity_service/benchmarking:k_anonymity_benchmarking_logger",
        "//services/k_anonymity_service/benchmarking:k_anonymity_no_op_logger",
        "//services/k_anonymity_service/data:runtime_config",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",  # for grpc_cli
        "@com_google_absl//absl/debugging:failure_signal_handler",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_google_sandboxed_api//sandboxed_api/sandbox2:comms",
        "@google_privacysandbox_servers_common//src/concurrent:executor",
        "@google_privacysandbox_servers_common//src/encryption/key_fetcher:key_fetcher_manager",
        "@google_privacysandbox_servers_common//src/public/cpio/interface/blob_storage_client",
        "@google_privacysandbox_servers_common//src/telemetry",
        "@google_privacysandbox_servers_common//src/util:rlimit_core_config",
        "@google_privacysandbox_servers_common//src/util/status_macro:status_macros",
        "@inference_common//sandbox:sandbox_executor",
    ],
)
