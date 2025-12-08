#!/usr/bin/env bash
set -euo pipefail

# Versions (override via env if needed)
BINUTILS_VER=${BINUTILS_VER:-2.45.1}
GCC_VER=${GCC_VER:-15.2.0}
GMP_VER=${GMP_VER:-6.3.0}
MPFR_VER=${MPFR_VER:-4.2.2}
MPC_VER=${MPC_VER:-1.3.1}

# Download directory (override via SRC_ROOT)
SRC_ROOT=${SRC_ROOT:-"$PWD/cross-compiler-setup/.download"}
mkdir -p "$SRC_ROOT"
cd "$SRC_ROOT"

BASE_URL="https://ftp.gnu.org/gnu"

download() {
  local url="$1"
  local file
  file="$(basename "$url")"

  if [ -f "$file" ]; then
    echo "Already have $file"
    return
  fi

  echo "Downloading $file"
  wget -c "$url"
}

# Binutils
download "$BASE_URL/binutils/binutils-$BINUTILS_VER.tar.xz"
download "$BASE_URL/binutils/binutils-$BINUTILS_VER.tar.xz.sig"

# GCC
download "$BASE_URL/gcc/gcc-$GCC_VER/gcc-$GCC_VER.tar.xz"
download "$BASE_URL/gcc/gcc-$GCC_VER/gcc-$GCC_VER.tar.xz.sig"

# Optional: GCC math dependencies (comment out if you use system libs)
download "$BASE_URL/gmp/gmp-$GMP_VER.tar.xz"
download "$BASE_URL/gmp/gmp-$GMP_VER.tar.xz.sig"

download "$BASE_URL/mpfr/mpfr-$MPFR_VER.tar.xz"
download "$BASE_URL/mpfr/mpfr-$MPFR_VER.tar.xz.sig"

download "$BASE_URL/mpc/mpc-$MPC_VER.tar.gz"
download "$BASE_URL/mpc/mpc-$MPC_VER.tar.gz.sig"

echo "All requested sources downloaded into: $SRC_ROOT"

