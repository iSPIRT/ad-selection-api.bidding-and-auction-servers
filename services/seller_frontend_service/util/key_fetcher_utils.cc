// Copyright 2023 Google LLC
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "services/seller_frontend_service/util/key_fetcher_utils.h"

#include <string>
#include <utility>
#include <vector>

#include <rapidjson/document.h>

#include "services/common/clients/config/trusted_server_config_client.h"
#include "services/common/constants/common_service_flags.h"
#include "services/common/loggers/request_log_context.h"
#include "services/common/public_key_url_allowlist.h"
#include "services/common/util/json_util.h"
#include "services/seller_frontend_service/runtime_flags.h"
#include "src/encryption/key_fetcher/fake_key_fetcher_manager.h"
#include "src/encryption/key_fetcher/interface/key_fetcher_manager_interface.h"
#include "src/util/status_macro/status_macros.h"

namespace privacy_sandbox::bidding_auction_servers {

using ::privacy_sandbox::server_common::PublicKeyFetcherFactory;

absl::StatusOr<PlatformToPublicKeyServiceEndpointMap>
ParseCloudPlatformPublicKeysMap(
    absl::string_view public_keys_endpoint_map_str) {
  PS_ASSIGN_OR_RETURN(rapidjson::Document public_keys_endpoint_map,
                      ParseJsonString(public_keys_endpoint_map_str));

  PlatformToPublicKeyServiceEndpointMap per_platform_endpoints;
  for (auto itr = public_keys_endpoint_map.MemberBegin();
       itr != public_keys_endpoint_map.MemberEnd(); ++itr) {
    if (!itr->name.IsString()) {
      return absl::InvalidArgumentError(kInvalidTypeForCloudPlatform);
    }

    const std::string& cloud_platform_str = itr->name.GetString();
    if (cloud_platform_str.empty()) {
      return absl::InvalidArgumentError(kEmptyCloudPlatformError);
    }

    server_common::CloudPlatform cloud_platform;
    if (absl::EqualsIgnoreCase(cloud_platform_str, "GCP")) {
      cloud_platform = server_common::CloudPlatform::kGcp;
    } else if (absl::EqualsIgnoreCase(cloud_platform_str, "AWS")) {
      cloud_platform = server_common::CloudPlatform::kAws;
    } else if (absl::EqualsIgnoreCase(cloud_platform_str, "AZURE")) {
      cloud_platform = server_common::CloudPlatform::kAzure;
    } else {
      return absl::InvalidArgumentError(
          absl::StrCat(kUnsupportedCloudPlatformValue, cloud_platform_str));
    }

    if (!itr->value.IsString()) {
      return absl::InvalidArgumentError(kInvalidTypeForEndpoint);
    }

    const std::string& public_key_service_endpoint = itr->value.GetString();
    if (public_key_service_endpoint.empty()) {
      return absl::InvalidArgumentError(kEmptyEndpointError);
    } else if (!IsAllowedPublicKeyUrl(public_key_service_endpoint,
                                      PS_IS_PROD_BUILD)) {
      return absl::InvalidArgumentError(
          absl::StrCat(kEndpointNotAllowlisted, public_key_service_endpoint));
    }

    per_platform_endpoints.try_emplace(
        cloud_platform, std::vector<std::string>{public_key_service_endpoint});
  }

  return per_platform_endpoints;
}

absl::Status ValidateSfePublicKeyEndpoints(
    const PlatformToPublicKeyServiceEndpointMap& endpoints_map,
    const absl::flat_hash_map<std::string, BuyerServiceEndpoint>&
        buyer_server_hosts) {
  for (const auto& entry : buyer_server_hosts) {
    if (!endpoints_map.contains(entry.second.cloud_platform)) {
      return absl::InvalidArgumentError(
          "Invalid runtime configs provided - a supplied buyer cloud "
          "platform in BUYER_SERVER_HOSTS is not present "
          "in SFE_PUBLIC_KEYS_ENDPOINTS");
    }
  }

  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<server_common::PublicKeyFetcherInterface>>
CreateSfePublicKeyFetcher(
    const TrustedServersConfigClient& config_client,
    const absl::flat_hash_map<std::string, BuyerServiceEndpoint>&
        buyer_server_hosts) {
  if (config_client.GetBooleanParameter(TEST_MODE)) {
    return nullptr;
  }

  if (!config_client.HasParameter(SFE_PUBLIC_KEYS_ENDPOINTS)) {
    return absl::InternalError(
        "No value supplied for SFE_PUBLIC_KEYS_ENDPOINTS");
  }

  PS_ASSIGN_OR_RETURN(
      PlatformToPublicKeyServiceEndpointMap endpoints_map,
      ParseCloudPlatformPublicKeysMap(
          config_client.GetStringParameter(SFE_PUBLIC_KEYS_ENDPOINTS)));
  PS_RETURN_IF_ERROR(
      ValidateSfePublicKeyEndpoints(endpoints_map, buyer_server_hosts));

  return PublicKeyFetcherFactory::Create(endpoints_map, SystemLogContext());
}

server_common::CloudPlatform ProtoCloudPlatformToScpCloudPlatform(
    EncryptionCloudPlatform cloud_platform) {
  switch (cloud_platform) {
    case EncryptionCloudPlatform::ENCRYPTION_CLOUD_PLATFORM_AWS:
      return server_common::CloudPlatform::kAws;
    case EncryptionCloudPlatform::ENCRYPTION_CLOUD_PLATFORM_GCP:
      return server_common::CloudPlatform::kGcp;
#if defined(MICROSOFT_AD_SELECTION_BUILD)
    case EncryptionCloudPlatform::ENCRYPTION_CLOUD_PLATFORM_AZURE:
      return server_common::CloudPlatform::kAzure;
#endif  // defined(MICROSOFT_AD_SELECTION_BUILD)
    default:
      return server_common::CloudPlatform::kLocal;
  }
}

}  // namespace privacy_sandbox::bidding_auction_servers
