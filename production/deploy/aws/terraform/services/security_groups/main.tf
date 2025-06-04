/**
 * Copyright 2022 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

# Setup security groups for various network components.
#
# NOTE that security group rules are managed in "../security_group_rules" module.

# Security group to control ingress and egress traffic for the load balancer.
resource "aws_security_group" "elb_security_group" {
  name   = "${var.operator}-${var.environment}-elb-sg"
  vpc_id = var.vpc_id

  tags = {
    Name        = "${var.operator}-${var.environment}-elb-sg"
    operator    = var.operator
    environment = var.environment
  }
}

# Security group to control ingress and egress traffic for the server ec2 instances.
resource "aws_security_group" "instance_security_group" {
  name   = "${var.operator}-${var.environment}-instance-sg"
  vpc_id = var.vpc_id

  tags = {
    Name        = "${var.operator}-${var.environment}-instance-sg"
    operator    = var.operator
    environment = var.environment
  }
}

# Security group to control ingress and egress traffic to backend vpc endpoints.
resource "aws_security_group" "vpce_security_group" {
  name   = "${var.operator}-${var.environment}-vpce-sg"
  vpc_id = var.vpc_id

  tags = {
    Name        = "${var.operator}-${var.environment}-vpce-sg"
    operator    = var.operator
    environment = var.environment
  }
}
