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

// IPC sidecar for testing.

#include "absl/log/check.h"
#include "proto/inference_sidecar.pb.h"

#include "ipc_sidecar.h"

int main(int argc, char** argv) {
  privacy_sandbox::bidding_auction_servers::inference::
      InferenceSidecarRuntimeConfig config;
  CHECK(!privacy_sandbox::bidding_auction_servers::inference::Run(config).ok())
      << "Unsuccessful run of the inference sidecar.";
  return 0;
}
