/*
 * Copyright 2024 Google LLC
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
#ifndef SERVICES_INFERENCE_SIDECAR_MODULES_TENSORFLOW_V2_14_0_VALIDATOR_H_
#define SERVICES_INFERENCE_SIDECAR_MODULES_TENSORFLOW_V2_14_0_VALIDATOR_H_

#include <string>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "model/validator.h"
#include "tensorflow/core/public/session.h"

namespace privacy_sandbox::bidding_auction_servers::inference {

class TensorFlowGraphValidator
    : public GraphValidatorInterface<tensorflow::NodeDef> {
 public:
  explicit TensorFlowGraphValidator(const tensorflow::GraphDef& graph);

 protected:
  bool IsOpAllowlisted(const tensorflow::NodeDef& node) const override;

 private:
  std::vector<tensorflow::NodeDef> GetAllNodes() const override;
  bool IsOpDenylisted(const tensorflow::NodeDef& node) const override;
  bool IsOpStateful(const tensorflow::NodeDef& node) const override;

  tensorflow::GraphDef graph_;
  absl::flat_hash_set<std::string> stateful_ops_;
};

}  // namespace privacy_sandbox::bidding_auction_servers::inference

#endif  // SERVICES_INFERENCE_SIDECAR_MODULES_TENSORFLOW_V2_14_0_VALIDATOR_H_
