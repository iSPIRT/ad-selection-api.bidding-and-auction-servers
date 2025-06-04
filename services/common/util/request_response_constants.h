/*
 * Copyright 2023 Google LLC
 * Copyright (C) Microsoft Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SERVICES_COMMON_UTIL_REQUEST_RESPONSE_CONSTANTS_H_
#define SERVICES_COMMON_UTIL_REQUEST_RESPONSE_CONSTANTS_H_

#include <stddef.h>

#include <array>
#include <cstdint>
#include <string>

#include "absl/strings/string_view.h"
#include "api/bidding_auction_servers.pb.h"

namespace privacy_sandbox::bidding_auction_servers {

// Number of Keys Microsoft adds to the auction input.
inline constexpr int kMicrosoftAuctionInputKeys = 1;

// Used to store a map of buyer origin to UpdateInterestGroupList.
// Just an alias for user convenience.
using UpdateGroupMap =
    ::google::protobuf::Map<std::string, UpdateInterestGroupList>;

using AdtechOriginDebugUrlsMap =
    ::google::protobuf::Map<std::string, DebugReports>;

// Maximum number of keys in the incoming ProtectedAuctionInput request.
inline constexpr int kNumRequestRootKeys = 8 + kMicrosoftAuctionInputKeys;

// Maximum number of keys that will be populated in the encoded CBOR
// ConsentedDebugConfig request.
inline constexpr int kNumConsentedDebugConfigKeys = 3;

// Number of Keys Microsoft adds to the Auction Result.
inline constexpr int kMicrosoftAuctionResultKeys = 1;

// Maximum number of keys that will be populated in the encoded CBOR
// AuctionResult response.
inline constexpr int kNumAuctionResultKeys = 21 + kMicrosoftAuctionResultKeys;

// Maximum number of keys that will be populated in the encoded CBOR
// WinReportingUrls response.
inline constexpr int kNumWinReportingUrlsKeys = 3;

// Maximum number of keys that will be populated in the encoded CBOR
// ReportingUrls response.
inline constexpr int kNumReportingUrlsKeys = 2;

// Maximum number of keys in each DebugReport.
inline constexpr int kNumDebugReportKeys = 4;

// Maximum number of keys that will be populated in the encoded CBOR
// DebugReports response.
inline constexpr int kNumDebugReportsKeys = 2;

// Maximum number of keys that will be populated in the encoded CBOR
// KAnonJoinCandidate response.
inline constexpr int kNumKAnonJoinCandidateKeys = 3;

// Maximum number of keys that will be populated in the encoded CBOR
// KAnonGhostWinner response.
inline constexpr int kNumKAnonGhostWinnerKeys = 6;

// Maximum number of keys that will be populated in the encoded CBOR
// GhostWinnerForTopLevelAuction response.
inline constexpr int kNumGhostWinnerForTopLevelAuctionKeys = 8;

// Maximum number of keys that will be populated in the encoded CBOR
// GhostWinnerPrivateAggregationSignals response.
inline constexpr int kNumGhostWinnerPrivateAggregationSignalsKeys = 2;

// Minimum size of the returned response in bytes.
inline constexpr size_t kMinAuctionResultBytes = 512;

// Maximum size of keys in the each Private Aggregation contribution.
inline constexpr int kNumContributionKeys = 2;

// Maximum size of keys in the each Private Aggregation event contribution.
inline constexpr int kNumEventContributionKeys = 2;

// Maximum size of keys in the each Private Aggregation ig contribution.
inline constexpr int kNumIgContributionKeys = 2;
inline constexpr int kNumIgContributionKeysWithoutIgIdx = 1;

// Maximum size of keys in the each paggResponse.
inline constexpr int kNumPAggResponseKeys = 2;

// Constants for the fields in the request payload.
inline constexpr char kVersion[] = "version";
inline constexpr char kPublisher[] = "publisher";
inline constexpr char kGenerationId[] = "generationId";
inline constexpr char kAdtechDebugId[] = "adtechDebugId";
inline constexpr char kSellerDebugId[] = "sellerDebugId";
inline constexpr char kBuyerDebugId[] = "buyerDebugId";
inline constexpr char kDebugReporting[] = "enableDebugReporting";
inline constexpr char kInterestGroups[] = "interestGroups";
inline constexpr char kAdRenderId[] = "ad_render_id";
inline constexpr char kName[] = "name";
inline constexpr char kBiddingSignalsKeys[] = "biddingSignalsKeys";
inline constexpr char kUserBiddingSignals[] = "userBiddingSignals";
inline constexpr char kAds[] = "ads";
inline constexpr char kAdComponents[] = "components";
inline constexpr char kBrowserSignals[] = "browserSignals";
inline constexpr char kBidCount[] = "bidCount";
inline constexpr char kJoinCount[] = "joinCount";
inline constexpr char kRecency[] = "recency";
inline constexpr char kRecencyMs[] = "recencyMs";
inline constexpr char kPrevWins[] = "prevWins";
inline constexpr char kConsentedDebugConfig[] = "consentedDebugConfig";
inline constexpr char kIsConsented[] = "isConsented";
inline constexpr char kToken[] = "token";
inline constexpr char kIsDebugResponse[] = "isDebugInfoInResponse";
inline constexpr int kRelativeTimeIndex = 0;
inline constexpr int kAdRenderIdIndex = 1;
inline constexpr char kRequestTimestampMs[] = "requestTimestampMs";
inline constexpr char kEnforceKAnon[] = "enforceKAnon";
inline constexpr char kIndex[] = "index";
inline constexpr char kUpdateIfOlderThanMs[] = "updateIfOlderThanMs";

#if defined(MICROSOFT_AD_SELECTION_BUILD)
inline constexpr char kMicrosoftGenuineHint[] =
    "microsoftGenuineHint";  // length: 20
#endif

// Constants for the fields in response sent back to clients.
inline constexpr char kBid[] = "bid";                              // length: 3
inline constexpr char kAdAuctionResultNonce[] = "nonce";           // length: 5
inline constexpr char kScore[] = "score";                          // length: 5
inline constexpr char kChaff[] = "isChaff";                        // length: 7
inline constexpr char kAdMetadata[] = "adMetadata";                // length: 10
inline constexpr char kAdRenderUrl[] = "adRenderURL";              // length: 11
inline constexpr char kBidCurrency[] = "bidCurrency";              // length: 11
inline constexpr char kDebugReports[] = "debugReports";            // length: 12
inline constexpr char kPAggResponse[] = "paggResponse";            // length: 12
inline constexpr char kUpdateGroups[] = "updateGroups";            // length: 12
inline constexpr char kBiddingGroups[] = "biddingGroups";          // length: 13
inline constexpr char kTopLevelSeller[] = "topLevelSeller";        // length: 14
inline constexpr char kBuyerReportingId[] = "buyerReportingId";    // length: 16
inline constexpr char kWinReportingUrls[] = "winReportingURLs";    // length: 16
inline constexpr char kInterestGroupName[] = "interestGroupName";  // length: 17
inline constexpr char kKAnonGhostWinners[] = "kAnonGhostWinners";  // length: 17
inline constexpr char kInterestGroupOwner[] =
    "interestGroupOwner";  // length: 18
#if defined(MICROSOFT_AD_SELECTION_BUILD)
inline constexpr char kMicrosoftIsKAnon[] =
    "microsoftWinnerIsKAnon";  // length: 22
#endif
inline constexpr char kKAnonWinnerJoinCandidates[] =
    "kAnonWinnerJoinCandidates";  // length: 25
inline constexpr char kKAnonWinnerPositionalIndex[] =
    "kAnonWinnerPositionalIndex";  // length: 26

// Constants for subfields returned in debugReports response object in
// AuctionResult.
inline constexpr char kReports[] = "reports";            // length: 7
inline constexpr char kAdTechOrigin[] = "adTechOrigin";  // length: 12

// Constants for subfields returned in debugReport in debugReports.
inline constexpr char kUrl[] = "url";                        // length: 3
inline constexpr char kIsWinReport[] = "isWinReport";        // length: 11
inline constexpr char kComponentWin[] = "componentWin";      // length: 12
inline constexpr char kIsSellerReport[] = "isSellerReport";  // length: 14

// Constants for subfields returned in KAnonJoinCandidate (or
// k_anon_winner_join_candidates) response object in AuctionResult.
inline constexpr char kAdRenderUrlHash[] = "adRenderURLHash";  // length: 15
inline constexpr char kReportingIdHash[] = "reportingIdHash";  // length: 15
inline constexpr char kAdComponentRenderUrlsHash[] =
    "adComponentRenderURLsHash";  // length: 25

// Constants for subfields returned in kAnonGhostWinners response object in
// AuctionResult.
inline constexpr char kOwner[] = "owner";  // length: 5
// kInterestGroupName is defined previously.
inline constexpr char kInterestGroupIndex[] =
    "interestGroupIndex";  // length: 18
inline constexpr char kKAnonJoinCandidates[] =
    "kAnonJoinCandidates";  // length: 19
inline constexpr char kGhostWinnerForTopLevelAuction[] =
    "ghostWinnerForTopLevelAuction";  // length: 29
inline constexpr char kGhostWinnerPrivateAggregationSignals[] =
    "ghostWinnerPrivateAggregationSignals";  // length: 36

// Constants for subfields returned in ghostWinnerForTopLevelAuction
// (under kAnonGhostWinners) in AuctionResult.
// adMetadata  (constant previously defined)           // length: 10
// adRenderUrl (constant previously defined)           // length: 11
// bidCurrency (constant previously defined)           // length: 11
inline constexpr char kModifiedBid[] = "modifiedBid";  // length: 11
// buyerReportingId (constant previously defined) // length: 16
inline constexpr char kAdComponentRenderUrls[] =
    "adComponentRenderURLs";  // length: 21
inline constexpr char kBuyerAndSellerReportingId[] =
    "buyerAndSellerReportingId";  // length: 25
inline constexpr char kSelectedBuyerAndSellerReportingId[] =
    "selectedBuyerAndSellerReportingId";  // length: 33

// Constants for subfields returned in contributions in AuctionResult.
inline constexpr char kValue[] = "value";    // length: 5
inline constexpr char kBucket[] = "bucket";  // length: 6

// Constants for subfields returned in event contributions in AuctionResult.
inline constexpr char kEvent[] = "event";                  // length: 5
inline constexpr char kContributions[] = "contributions";  // length: 13

// Constants for subfields returned in igContributions
inline constexpr char kIgIndex[] = "igIndex";  // length: 7
inline constexpr char kEventContributions[] =
    "eventContributions";  // length: 18

// Constants for subfields returned in paggResponse
inline constexpr char kIgContributions[] = "igContributions";  // length: 15
inline constexpr char kReportingOrigin[] = "reportingOrigin";  // length: 15

inline constexpr std::array<absl::string_view, kNumRequestRootKeys>
    kRequestRootKeys = {kVersion,
                        kPublisher,
                        kInterestGroups,
                        kGenerationId,
                        kDebugReporting,
                        kConsentedDebugConfig,
                        kRequestTimestampMs,
                        kEnforceKAnon,
#if defined(MICROSOFT_AD_SELECTION_BUILD)
                        kMicrosoftGenuineHint
#endif
};
inline constexpr int kNumInterestGroupKeys = 6;
inline constexpr std::array<absl::string_view, kNumInterestGroupKeys>
    kInterestGroupKeys = {kName, kBiddingSignalsKeys, kUserBiddingSignals,
                          kAds,  kAdComponents,       kBrowserSignals};

inline constexpr int kNumBrowserSignalKeys = 5;
inline constexpr std::array<absl::string_view, kNumBrowserSignalKeys>
    kBrowserSignalKeys = {kBidCount, kJoinCount, kRecency, kPrevWins,
                          kRecencyMs};
inline constexpr std::array<absl::string_view, kNumConsentedDebugConfigKeys>
    kConsentedDebugConfigKeys = {
        kIsConsented,
        kToken,
        kIsDebugResponse,
};

inline constexpr std::array<absl::string_view, kNumContributionKeys>
    kPaggContributionKeys = {
        kValue,
        kBucket,
};

inline constexpr std::array<absl::string_view, kNumEventContributionKeys>
    kPaggEventContributionKeys = {
        kEvent,
        kContributions,
};

inline constexpr std::array<absl::string_view, kNumIgContributionKeys>
    kPaggIgContributionKeys = {
        kIgIndex,
        kEventContributions,
};

inline constexpr std::array<absl::string_view, kNumPAggResponseKeys>
    kPaggResponseKeys = {
        kIgContributions,
        kReportingOrigin,
};

inline constexpr int kNumErrorKeys = 2;
inline constexpr char kError[] = "error";
inline constexpr char kMessage[] = "message";
inline constexpr char kCode[] = "code";
inline constexpr char kBuyerReportingUrls[] = "buyerReportingURLs";
inline constexpr char kComponentSellerReportingUrls[] =
    "componentSellerReportingURLs";
inline constexpr char kTopLevelSellerReportingUrls[] =
    "topLevelSellerReportingURLs";
inline constexpr char kReportingUrl[] = "reportingURL";
inline constexpr char kInteractionReportingUrls[] = "interactionReportingURLs";

// Default value for multi_bid_limit for generateBid. Used when multi_bid_limit
// is not set or enforce_k_anon is set to be false.
inline constexpr int kDefaultMultiBidLimit = 2;

inline constexpr std::array<absl::string_view, kNumErrorKeys> kErrorKeys = {
    kMessage,
    kCode,
};
inline constexpr std::array<absl::string_view, kNumWinReportingUrlsKeys>
    kWinReportingKeys = {kBuyerReportingUrls, kComponentSellerReportingUrls,
                         kTopLevelSellerReportingUrls};
inline constexpr std::array<absl::string_view, kNumReportingUrlsKeys>
    kReportingKeys = {kReportingUrl, kInteractionReportingUrls};
inline constexpr std::array<absl::string_view, kNumDebugReportKeys>
    kDebugReportKeys = {kUrl, kIsWinReport, kComponentWin, kIsSellerReport};
inline constexpr std::array<absl::string_view, kNumDebugReportsKeys>
    kDebugReportsKeys = {kReports, kAdTechOrigin};
inline constexpr std::array<absl::string_view, kNumKAnonJoinCandidateKeys>
    kKAnonJoinCandidateKeys = {kAdRenderUrlHash, kAdComponentRenderUrlsHash,
                               kReportingIdHash};
inline constexpr std::array<absl::string_view, kNumKAnonGhostWinnerKeys>
    kKAnonGhostWinnerKeys = {kKAnonJoinCandidates,
                             kInterestGroupIndex,
                             kOwner,
                             kInterestGroupName,
                             kGhostWinnerPrivateAggregationSignals,
                             kGhostWinnerForTopLevelAuction};
inline constexpr std::array<absl::string_view,
                            kNumGhostWinnerPrivateAggregationSignalsKeys>
    kGhostWinnerPrivateAggregationSignalsKeys = {kBucket, kValue};
inline constexpr std::array<absl::string_view,
                            kNumGhostWinnerForTopLevelAuctionKeys>
    kGhostWinnerForTopLevelAuctionKeys = {
        kAdRenderUrl,      kAdComponentRenderUrls,
        kModifiedBid,      kBidCurrency,
        kAdMetadata,       kBuyerAndSellerReportingId,
        kBuyerReportingId, kSelectedBuyerAndSellerReportingId};

enum class AuctionType : std::uint8_t {
  kProtectedAudience,
  kProtectedAppSignals
};

}  // namespace privacy_sandbox::bidding_auction_servers

#endif  // SERVICES_COMMON_UTIL_REQUEST_RESPONSE_CONSTANTS_H_
