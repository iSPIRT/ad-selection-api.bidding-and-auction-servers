// Copyright 2024 Google LLC
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

#include "services/bidding_service/inference/model_fetcher_metric.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/status/status.h"

namespace privacy_sandbox::bidding_auction_servers::inference {
namespace {

using ::testing::Pair;
using ::testing::UnorderedElementsAre;

class ModelFetcherMetricTest : public ::testing::Test {
 protected:
  void SetUp() override { ModelFetcherMetric::ClearStates(); }
};

TEST_F(ModelFetcherMetricTest, GetCloudFetchSuccessCount) {
  EXPECT_THAT(ModelFetcherMetric::GetCloudFetchSuccessCount(),
              UnorderedElementsAre(Pair("cloud fetch", 0)));

  ModelFetcherMetric::IncrementCloudFetchSuccessCount();
  EXPECT_THAT(ModelFetcherMetric::GetCloudFetchSuccessCount(),
              UnorderedElementsAre(Pair("cloud fetch", 1)));
}

TEST_F(ModelFetcherMetricTest, GetCloudFetchFailedCountByStatus) {
  EXPECT_THAT(ModelFetcherMetric::GetCloudFetchFailedCountByStatus(),
              UnorderedElementsAre());

  ModelFetcherMetric::IncrementCloudFetchFailedCountByStatus(
      absl::StatusCode::kUnavailable);
  ModelFetcherMetric::IncrementCloudFetchFailedCountByStatus(
      absl::StatusCode::kInternal);

  EXPECT_THAT(
      ModelFetcherMetric::GetCloudFetchFailedCountByStatus(),
      UnorderedElementsAre(Pair("UNAVAILABLE", 1), Pair("INTERNAL", 1)));

  ModelFetcherMetric::IncrementCloudFetchFailedCountByStatus(
      absl::StatusCode::kUnavailable);

  EXPECT_THAT(
      ModelFetcherMetric::GetCloudFetchFailedCountByStatus(),
      UnorderedElementsAre(Pair("UNAVAILABLE", 2), Pair("INTERNAL", 1)));
}

TEST_F(ModelFetcherMetricTest, GetModelDeletionSuccessCount) {
  EXPECT_THAT(ModelFetcherMetric::GetModelDeletionSuccessCount(),
              UnorderedElementsAre(Pair("model deletion", 0)));

  ModelFetcherMetric::IncrementModelDeletionSuccessCount();
  EXPECT_THAT(ModelFetcherMetric::GetModelDeletionSuccessCount(),
              UnorderedElementsAre(Pair("model deletion", 1)));
}

TEST_F(ModelFetcherMetricTest, GetModelDeletionFailedCountByStatus) {
  EXPECT_THAT(ModelFetcherMetric::GetModelDeletionFailedCountByStatus(),
              UnorderedElementsAre());

  ModelFetcherMetric::IncrementModelDeletionFailedCountByStatus(
      absl::StatusCode::kUnavailable);
  ModelFetcherMetric::IncrementModelDeletionFailedCountByStatus(
      absl::StatusCode::kInternal);

  EXPECT_THAT(
      ModelFetcherMetric::GetModelDeletionFailedCountByStatus(),
      UnorderedElementsAre(Pair("UNAVAILABLE", 1), Pair("INTERNAL", 1)));

  ModelFetcherMetric::IncrementModelDeletionFailedCountByStatus(
      absl::StatusCode::kUnavailable);

  EXPECT_THAT(
      ModelFetcherMetric::GetModelDeletionFailedCountByStatus(),
      UnorderedElementsAre(Pair("UNAVAILABLE", 2), Pair("INTERNAL", 1)));
}

TEST_F(ModelFetcherMetricTest, GetRecentModelRegistrationSuccess) {
  EXPECT_THAT(ModelFetcherMetric::GetRecentModelRegistrationSuccess(),
              UnorderedElementsAre());

  ModelFetcherMetric::UpdateRecentModelRegistrationSuccess(
      {"model1", "model2"});

  EXPECT_THAT(ModelFetcherMetric::GetRecentModelRegistrationSuccess(),
              UnorderedElementsAre(Pair("model1", 1), Pair("model2", 1)));

  ModelFetcherMetric::UpdateRecentModelRegistrationSuccess({"model1"});

  EXPECT_THAT(ModelFetcherMetric::GetRecentModelRegistrationSuccess(),
              UnorderedElementsAre(Pair("model1", 1)));
}

TEST_F(ModelFetcherMetricTest, GetRecentModelRegistrationFailure) {
  EXPECT_THAT(ModelFetcherMetric::GetRecentModelRegistrationFailure(),
              UnorderedElementsAre());

  ModelFetcherMetric::UpdateRecentModelRegistrationFailure(
      {"model1", "model2"});

  EXPECT_THAT(ModelFetcherMetric::GetRecentModelRegistrationFailure(),
              UnorderedElementsAre(Pair("model1", 1), Pair("model2", 1)));

  ModelFetcherMetric::UpdateRecentModelRegistrationFailure({"model1"});

  EXPECT_THAT(ModelFetcherMetric::GetRecentModelRegistrationFailure(),
              UnorderedElementsAre(Pair("model1", 1)));
}

TEST_F(ModelFetcherMetricTest, GetModelRegistrationFailedCountByErroCode) {
  EXPECT_THAT(ModelFetcherMetric::GetModelRegistrationFailedCountByStatus(),
              UnorderedElementsAre());

  ModelFetcherMetric::IncrementModelRegistrationFailedCountByStatus(
      absl::StatusCode::kUnavailable);
  ModelFetcherMetric::IncrementModelRegistrationFailedCountByStatus(
      absl::StatusCode::kInternal);

  EXPECT_THAT(
      ModelFetcherMetric::GetModelRegistrationFailedCountByStatus(),
      UnorderedElementsAre(Pair("UNAVAILABLE", 1), Pair("INTERNAL", 1)));

  ModelFetcherMetric::IncrementModelRegistrationFailedCountByStatus(
      absl::StatusCode::kUnavailable);

  EXPECT_THAT(
      ModelFetcherMetric::GetModelRegistrationFailedCountByStatus(),
      UnorderedElementsAre(Pair("UNAVAILABLE", 2), Pair("INTERNAL", 1)));
}

TEST_F(ModelFetcherMetricTest, GetAvailableModels) {
  EXPECT_THAT(ModelFetcherMetric::GetAvailableModels(), UnorderedElementsAre());

  ModelFetcherMetric::UpdateAvailableModels({"model1", "model2"});

  EXPECT_THAT(ModelFetcherMetric::GetAvailableModels(),
              UnorderedElementsAre(Pair("model1", 1), Pair("model2", 1)));
}

TEST_F(ModelFetcherMetricTest, GetAvailableModelsShouldRemoveZeroValueEntries) {
  EXPECT_THAT(ModelFetcherMetric::GetAvailableModels(), UnorderedElementsAre());
  ModelFetcherMetric::UpdateAvailableModels({"model1"});
  EXPECT_THAT(ModelFetcherMetric::GetAvailableModels(),
              UnorderedElementsAre(Pair("model1", 1)));
  ModelFetcherMetric::UpdateAvailableModels({});
  EXPECT_THAT(ModelFetcherMetric::GetAvailableModels(),
              UnorderedElementsAre(Pair("model1", 0)));
  // No need to report the zero value entry twice since cloud monitoring will
  // persist the last receieved zero value.
  EXPECT_THAT(ModelFetcherMetric::GetAvailableModels(), UnorderedElementsAre());
}

TEST_F(ModelFetcherMetricTest, GetModelPrewarmLatencySuccess) {
  EXPECT_THAT(ModelFetcherMetric::GetModelRegistrationPrewarmLatency(),
              UnorderedElementsAre());
  ModelFetcherMetric::UpdateModelRegistrationPrewarmLatency({{"model1", 25}});
  EXPECT_THAT(ModelFetcherMetric::GetModelRegistrationPrewarmLatency(),
              UnorderedElementsAre(Pair("model1", 25)));
}

TEST_F(ModelFetcherMetricTest,
       GetAvailableModelsShouldAlsoRemoveZeroValueModelPreWarmLatency) {
  ModelFetcherMetric::UpdateAvailableModels({"model1"});
  ModelFetcherMetric::UpdateModelRegistrationPrewarmLatency({{"model1", 25}});
  EXPECT_THAT(ModelFetcherMetric::GetAvailableModels(),
              UnorderedElementsAre(Pair("model1", 1)));
  EXPECT_THAT(ModelFetcherMetric::GetModelRegistrationPrewarmLatency(),
              UnorderedElementsAre(Pair("model1", 25)));
  ModelFetcherMetric::UpdateAvailableModels({});
  EXPECT_THAT(ModelFetcherMetric::GetAvailableModels(),
              UnorderedElementsAre(Pair("model1", 0)));
  EXPECT_THAT(ModelFetcherMetric::GetModelRegistrationPrewarmLatency(),
              UnorderedElementsAre());
}

}  // namespace
}  // namespace privacy_sandbox::bidding_auction_servers::inference
