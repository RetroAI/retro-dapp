# syntax=docker.io/docker/dockerfile:1.4
################################################################################
#
#  Copyright (C) 2024 retro.ai
#  This file is part of retro3 - https://github.com/retroai/retro3
#
#  This file is derived from Sunodo under the Apache 2.0 License
#  Copyright (C) 2023-2024 Sunodo (https://docs.sunodo.io)
#
#  SPDX-License-Identifier: AGPL-3.0-or-later AND Apache-2.0
#  See the file LICENSE.txt for more information.
#
################################################################################

#
# builder
#

FROM --platform=linux/riscv64 riscv64/ubuntu:22.04 as builder

RUN <<EOF
set -e
apt-get update
apt-get install -y --no-install-recommends \
  autoconf \
  automake \
  build-essential \
  ca-certificates \
  curl \
  libtool \
  wget
rm -rf /var/lib/apt/lists/*
EOF

COPY --from=sunodo/sdk:0.2.0 /opt/riscv /opt/riscv
WORKDIR /opt/cartesi/dapp
COPY src/backend .
RUN make

#
# host
#

FROM --platform=linux/riscv64 riscv64/ubuntu:22.04 as host

LABEL io.sunodo.sdk_version=0.2.0
LABEL io.cartesi.rollups.ram_size=128Mi

ARG MACHINE_EMULATOR_TOOLS_VERSION=0.12.0
RUN <<EOF
set -e
apt-get update
apt-get install -y --no-install-recommends busybox-static=1:1.30.1-7ubuntu3 ca-certificates=20230311ubuntu0.22.04.1 curl=7.81.0-1ubuntu1.15
curl -fsSL https://github.com/cartesi/machine-emulator-tools/releases/download/v${MACHINE_EMULATOR_TOOLS_VERSION}/machine-emulator-tools-v${MACHINE_EMULATOR_TOOLS_VERSION}.tar.gz \
  | tar -C / --overwrite -xvzf -
rm -rf /var/lib/apt/lists/*
EOF

ENV PATH="/opt/cartesi/bin:${PATH}"

WORKDIR /opt/cartesi/dapp
COPY --from=builder /opt/cartesi/dapp/dapp .

ENTRYPOINT ["/opt/cartesi/dapp/dapp"]
