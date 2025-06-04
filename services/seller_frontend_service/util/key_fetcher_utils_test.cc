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

#include <memory>
#include <string>

#include <include/gmock/gmock-actions.h>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "include/gtest/gtest.h"
#include "services/common/public_key_url_allowlist.h"

namespace privacy_sandbox::bidding_auction_servers {
namespace {

using ::testing::ContainsRegex;
using ::testing::Return;

constexpr absl::string_view kBuyerHostMapForSingleBuyer =
    R"json(
    {
      "https://bid1.com": {
        "url": "dns:///bfe-dev.bidding1.com:443",
        "cloudPlatform": "AWS"
      }
    }
    )json";

TEST(KeyFetcherUtilsTest, ParseCloudPlatformPublicKeysMap_ValidInput) {
  constexpr absl::string_view platform_format = R"json(
{
  "GCP": "%s",
  "AWS": "%s",
  "Azure": "%s"
}
)json";

  std::string per_platform_public_key_endpoints =
      absl::StrFormat(platform_format, kGCPProdPublicKeyEndpoint,
                      kAWSProdPublicKeyEndpoint, kAzureProdPublicKeyEndpoint);

  auto map = ParseCloudPlatformPublicKeysMap(per_platform_public_key_endpoints);
  ASSERT_TRUE(map.ok());
  EXPECT_EQ(map->size(), 3);

  EXPECT_EQ((*map)[server_common::CloudPlatform::kGcp][0],
            kGCPProdPublicKeyEndpoint);
  EXPECT_EQ((*map)[server_common::CloudPlatform::kAws][0],
            kAWSProdPublicKeyEndpoint);
  EXPECT_EQ((*map)[server_common::CloudPlatform::kAzure][0],
            kAzureProdPublicKeyEndpoint);
}

TEST(KeyFetcherUtilsTest, ParseCloudPlatformPublicKeysMap_InvalidJson) {
  absl::string_view invalid_json = R"json( { )json";

  auto map = ParseCloudPlatformPublicKeysMap(invalid_json);
  ASSERT_TRUE(absl::IsInvalidArgument(map.status()));
}

TEST(KeyFetcherUtilsTest,
     ParseCloudPlatformPublicKeysMap_EmptyValueForCloudPlatform) {
  absl::string_view json =
      R"json({ "": "https://publickeyservice.foo/v1alpha/publicKeys" })json";

  auto map = ParseCloudPlatformPublicKeysMap(json);
  ASSERT_TRUE(absl::IsInvalidArgument(map.status()));
  EXPECT_EQ(map.status().message(), kEmptyCloudPlatformError);
}

TEST(KeyFetcherUtilsTest,
     ParseCloudPlatformPublicKeysMap_EmptyValueForEndpoint) {
  absl::string_view json = R"json({ "GCP": "" })json";

  auto map = ParseCloudPlatformPublicKeysMap(json);
  ASSERT_TRUE(absl::IsInvalidArgument(map.status()));
  EXPECT_EQ(map.status().message(), kEmptyEndpointError);
}

TEST(ProtoCloudPlatformToScpCloudPlatformTest, ReturnsGcpForGcp) {
  EXPECT_EQ(server_common::CloudPlatform::kGcp,
            ProtoCloudPlatformToScpCloudPlatform(
                EncryptionCloudPlatform::ENCRYPTION_CLOUD_PLATFORM_GCP));
}

TEST(ProtoCloudPlatformToScpCloudPlatformTest, ReturnsAwsForAws) {
  EXPECT_EQ(server_common::CloudPlatform::kAws,
            ProtoCloudPlatformToScpCloudPlatform(
                EncryptionCloudPlatform::ENCRYPTION_CLOUD_PLATFORM_AWS));
}

TEST(ProtoCloudPlatformToScpCloudPlatformTest, ReturnsLocalForUnspecified) {
  EXPECT_EQ(
      server_common::CloudPlatform::kLocal,
      ProtoCloudPlatformToScpCloudPlatform(
          EncryptionCloudPlatform::ENCRYPTION_CLOUD_PLATFORM_UNSPECIFIED));
}

TEST(KeyFetcherUtilsTest,
     ValidateSfePublicKeyEndpoints_FailsOnNoKeysForBuyerCloudPlatform) {
  constexpr absl::string_view platform_format = R"json(
{
  "GCP": "%s"
}
)json";
  std::string per_platform_public_key_endpoints =
      absl::StrFormat(platform_format, kGCPProdPublicKeyEndpoint);
  auto cloud_public_keys_map =
      ParseCloudPlatformPublicKeysMap(per_platform_public_key_endpoints);
  ASSERT_TRUE(cloud_public_keys_map.ok());

  auto map = ParseIgOwnerToBfeDomainMap(kBuyerHostMapForSingleBuyer);
  ASSERT_TRUE(map.ok());

  auto error = ValidateSfePublicKeyEndpoints(*cloud_public_keys_map, *map);
  ASSERT_FALSE(error.ok());
  EXPECT_EQ(
      error.message(),
      "Invalid runtime configs provided - a supplied buyer cloud platform in "
      "BUYER_SERVER_HOSTS is not present in SFE_PUBLIC_KEYS_ENDPOINTS");
}

}  // namespace
}  // namespace privacy_sandbox::bidding_auction_servers
