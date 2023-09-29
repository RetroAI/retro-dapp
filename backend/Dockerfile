# syntax=docker.io/docker/dockerfile:1.4
FROM --platform=linux/riscv64 cartesi/python:3.10-slim-jammy

# Install build dependencies
RUN <<EOF
apt update
apt install -y \
  autoconf \
  build-essential \
  cmake \
  libopenblas-dev \
  ninja-build \
  python3-pkgconfig
rm -rf /var/lib/apt/lists/*
EOF

# Update Python dependencies
RUN pip3 install --upgrade \
  Cython \
  pip \
  setuptools \
  wheel

# Cache numpy installation from requirements.txt (takes > 10mins)
RUN pip3 install numpy==1.26.0

# Enter directory for building dapp
WORKDIR /opt/cartesi/dapp

# Install Python dependencies
COPY requirements.txt .
RUN <<EOF
pip install -r requirements.txt --no-cache
find /usr/local/lib -type d -name __pycache__ -exec rm -r {} +
EOF

# Remove packages not needed for runtime
RUN <<EOF
apt remove --purge \
  autoconf \
  build-essential \
  cmake \
  libopenblas-dev \
  ninja-build \
  python3-pkgconfig
apt autoremove -y --purge
EOF

COPY ./entrypoint.sh .
COPY ./retroai.py .
