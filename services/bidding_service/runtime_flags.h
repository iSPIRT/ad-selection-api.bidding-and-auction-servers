//  Copyright 2022 Google LLC
//  Copyright (C) Microsoft Corporation. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef FLEDGE_SERVICES_BIDDING_SERVICE_RUNTIME_FLAGS_H_
#define FLEDGE_SERVICES_BIDDING_SERVICE_RUNTIME_FLAGS_H_

#include <vector>

#include "absl/strings/string_view.h"
#include "services/bidding_service/inference/inference_flags.h"
#include "services/common/constants/common_service_flags.h"

#if defined(MICROSOFT_AD_SELECTION_BUILD)
#include "services/bidding_service/microsoft_routing/routing_flags.h"
#endif  // defined(MICROSOFT_AD_SELECTION_BUILD)

namespace privacy_sandbox::bidding_auction_servers {

// Define runtime flag names.
inline constexpr absl::string_view PORT = "BIDDING_PORT";
inline constexpr absl::string_view HEALTHCHECK_PORT =
    "BIDDING_HEALTHCHECK_PORT";
inline constexpr absl::string_view ENABLE_BIDDING_SERVICE_BENCHMARK =
    "ENABLE_BIDDING_SERVICE_BENCHMARK";
inline constexpr absl::string_view BUYER_CODE_FETCH_CONFIG =
    "BUYER_CODE_FETCH_CONFIG";
inline constexpr absl::string_view EGRESS_SCHEMA_FETCH_CONFIG =
    "EGRESS_SCHEMA_FETCH_CONFIG";
inline constexpr absl::string_view UDF_NUM_WORKERS = "UDF_NUM_WORKERS";
inline constexpr absl::string_view JS_WORKER_QUEUE_LEN = "JS_WORKER_QUEUE_LEN";
inline constexpr absl::string_view TEE_AD_RETRIEVAL_KV_SERVER_ADDR =
    "TEE_AD_RETRIEVAL_KV_SERVER_ADDR";
inline constexpr absl::string_view
    TEE_AD_RETRIEVAL_KV_SERVER_GRPC_ARG_DEFAULT_AUTHORITY =
        "TEE_AD_RETRIEVAL_KV_SERVER_GRPC_ARG_DEFAULT_AUTHORITY";
inline constexpr absl::string_view TEE_KV_SERVER_ADDR = "TEE_KV_SERVER_ADDR";
inline constexpr absl::string_view TEE_KV_SERVER_GRPC_ARG_DEFAULT_AUTHORITY =
    "TEE_KV_SERVER_GRPC_ARG_DEFAULT_AUTHORITY";
inline constexpr absl::string_view AD_RETRIEVAL_TIMEOUT_MS =
    "AD_RETRIEVAL_TIMEOUT_MS";
inline constexpr absl::string_view AD_RETRIEVAL_KV_SERVER_EGRESS_TLS =
    "AD_RETRIEVAL_KV_SERVER_EGRESS_TLS";
inline constexpr absl::string_view KV_SERVER_EGRESS_TLS =
    "KV_SERVER_EGRESS_TLS";
inline constexpr char
    BIDDING_TCMALLOC_BACKGROUND_RELEASE_RATE_BYTES_PER_SECOND[] =
        "BIDDING_TCMALLOC_BACKGROUND_RELEASE_RATE_BYTES_PER_SECOND";
inline constexpr absl::string_view
    BIDDING_TCMALLOC_MAX_TOTAL_THREAD_CACHE_BYTES =
        "BIDDING_TCMALLOC_MAX_TOTAL_THREAD_CACHE_BYTES";

inline constexpr int kNumRuntimeFlags = 16;
inline constexpr std::array<absl::string_view, kNumRuntimeFlags> kFlags = {
    PORT,
    HEALTHCHECK_PORT,
    ENABLE_BIDDING_SERVICE_BENCHMARK,
    BUYER_CODE_FETCH_CONFIG,
    EGRESS_SCHEMA_FETCH_CONFIG,
    UDF_NUM_WORKERS,
    JS_WORKER_QUEUE_LEN,
    TEE_AD_RETRIEVAL_KV_SERVER_ADDR,
    TEE_AD_RETRIEVAL_KV_SERVER_GRPC_ARG_DEFAULT_AUTHORITY,
    TEE_KV_SERVER_ADDR,
    TEE_KV_SERVER_GRPC_ARG_DEFAULT_AUTHORITY,
    AD_RETRIEVAL_KV_SERVER_EGRESS_TLS,
    KV_SERVER_EGRESS_TLS,
    AD_RETRIEVAL_TIMEOUT_MS,
    BIDDING_TCMALLOC_BACKGROUND_RELEASE_RATE_BYTES_PER_SECOND,
    BIDDING_TCMALLOC_MAX_TOTAL_THREAD_CACHE_BYTES,
};

inline std::vector<absl::string_view> GetServiceFlags() {
  std::vector<absl::string_view> flags(kFlags.begin(),
                                       kFlags.begin() + kNumRuntimeFlags);

  for (absl::string_view flag : kCommonServiceFlags) {
    flags.push_back(flag);
  }

  for (absl::string_view flag : kInferenceFlags) {
    flags.push_back(flag);
  }

#if defined(MICROSOFT_AD_SELECTION_BUILD)
  for (absl::string_view flag : microsoft::routing_utils::kRoutingFlags) {
    flags.push_back(flag);
  }
#endif  // defined(MICROSOFT_AD_SELECTION_BUILD)

  return flags;
}

}  // namespace privacy_sandbox::bidding_auction_servers

#endif  // FLEDGE_SERVICES_BIDDING_SERVICE_RUNTIME_FLAGS_H_
