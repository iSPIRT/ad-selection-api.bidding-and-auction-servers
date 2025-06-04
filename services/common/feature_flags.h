//  Copyright 2024 Google LLC
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

#ifndef SERVICES_COMMON_FEATURE_FLAGS_H_
#define SERVICES_COMMON_FEATURE_FLAGS_H_

#include <stdbool.h>

#include "absl/flags/declare.h"

ABSL_DECLARE_FLAG(int, limited_egress_bits);
ABSL_DECLARE_FLAG(bool, enable_temporary_unlimited_egress);
ABSL_DECLARE_FLAG(bool, enable_cancellation);
ABSL_DECLARE_FLAG(bool, enable_kanon);
ABSL_DECLARE_FLAG(bool, enable_buyer_private_aggregate_reporting);
ABSL_DECLARE_FLAG(int, per_adtech_paapi_contributions_limit);

#endif  // SERVICES_COMMON_FEATURE_FLAGS_H_
