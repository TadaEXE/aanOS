#!/usr/bin/env bash
set -euo pipefail

# These might be set to something, which might disrupt the build.
# So we just remove them for the duration of this script.
TMP_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
TMP_CFLAGS=$CFLAGS
TMP_CXXFLAGS=$CXXFLAGS
unset LD_LIBRARY_PATH
unset CFLAGS
unset CXXFLAGS

# Args can be changed via cmd args or env
CROSS_TC_BINUTILS_VER=${CROSS_TC_BINUTILS_VER:-2.45.1}
CROSS_TC_GCC_VER=${CROSS_TC_GCC_VER:-15.2.0}
CROSS_TC_GMP_VER=${CROSS_TC_GMP_VER:-6.3.0}
CROSS_TC_MPFR_VER=${CROSS_TC_MPFR_VER:-4.2.2}
CROSS_TC_MPC_VER=${CROSS_TC_MPC_VER:-1.3.1}
CROSS_TC_ROOT=${CROSS_TC_ROOT:-$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/cross-compiler}
CROSS_TC_TARGET=${CROSS_TC_TARGET:-x86_64-elf}
CROSS_TC_JOBS=${CROSS_TC_JOBS:-$(($(command -v nproc >/dev/null 2>&1 && nproc || echo 1)/2))}

# ---- parse args -------------------------------------------------------------
while [[ $# -gt 0 ]]; do
  case "$1" in
    --root) CROSS_TC_ROOT="$2"; shift 2 ;;
    --target) CROSS_TC_TARGET="$2"; shift 2 ;;
    --jobs) CROSS_TC_JOBS="$2"; shift 2 ;;

    --binutils-ver) CROSS_TC_BINUTILS_VER="$2"; shift 2 ;;
    --gcc-ver) CROSS_TC_GCC_VER="$2"; shift 2 ;;
    --gmp-ver) CROSS_TC_GMP_VER="$2"; shift 2 ;;
    --mpfr-ver) CROSS_TC_MPFR_VER="$2"; shift 2 ;;
    --mpc-ver) CROSS_TC_MPC_VER="$2"; shift 2 ;;

    *) echo "Unknown arg: $1" >&2; exit 1 ;;
  esac
done

CROSS_TC_PREFIX=${CROSS_TC_PREFIX:-${CROSS_TC_ROOT}/${CROSS_TC_TARGET}-toolchain}
CROSS_TC_SRC_ROOT=${CROSS_TC_SRC_ROOT:-"${CROSS_TC_ROOT}/.download"}
CROSS_TC_BUILD_ROOT=${CROSS_TC_BUILD_ROOT:-"${CROSS_TC_ROOT}/.build"}

require_tarball() {
  local f="$1"
  if [ ! -f "$CROSS_TC_SRC_ROOT/$f" ]; then
    echo "Missing source tarball: $CROSS_TC_SRC_ROOT/$f" >&2
    echo "Run the download script first or place it there manually." >&2
    exit 1
  fi
}

extract_if_missing() {
  local tar="$1"
  local dir="$2"
  if [ ! -d "$CROSS_TC_SRC_ROOT/$dir" ]; then
    echo "Extracting $tar"
    (cd "$CROSS_TC_SRC_ROOT" && tar xf "$tar")
  else
    echo "Using existing directory: $CROSS_TC_SRC_ROOT/$dir"
  fi
}

echo "Using CROSS_TC_SRC_ROOT=$CROSS_TC_SRC_ROOT"
echo "Using CROSS_TC_BUILD_ROOT=$CROSS_TC_BUILD_ROOT"
echo "Using CROSS_TC_PREFIX=$CROSS_TC_PREFIX"
echo "Using CROSS_TC_TARGET=$CROSS_TC_TARGET"

require_tarball "binutils-$CROSS_TC_BINUTILS_VER.tar.xz"
require_tarball "gcc-$CROSS_TC_GCC_VER.tar.xz"
require_tarball "gmp-$CROSS_TC_GMP_VER.tar.xz"
require_tarball "mpfr-$CROSS_TC_MPFR_VER.tar.xz"
require_tarball "mpc-$CROSS_TC_MPC_VER.tar.gz"

extract_if_missing "binutils-$CROSS_TC_BINUTILS_VER.tar.xz" "binutils-$CROSS_TC_BINUTILS_VER"
extract_if_missing "gcc-$CROSS_TC_GCC_VER.tar.xz" "gcc-$CROSS_TC_GCC_VER"
extract_if_missing "gmp-$CROSS_TC_GMP_VER.tar.xz" "gmp-$CROSS_TC_GMP_VER"
extract_if_missing "mpfr-$CROSS_TC_MPFR_VER.tar.xz" "mpfr-$CROSS_TC_MPFR_VER"
extract_if_missing "mpc-$CROSS_TC_MPC_VER.tar.gz" "mpc-$CROSS_TC_MPC_VER"

echo "Preparing in-tree GMP/MPFR/MPC in GCC source"

cd "$CROSS_TC_SRC_ROOT/gcc-$CROSS_TC_GCC_VER"

if [ ! -d gmp ]; then
  ln -s "../gmp-$CROSS_TC_GMP_VER" gmp
fi

if [ ! -d mpfr ]; then
  ln -s "../mpfr-$CROSS_TC_MPFR_VER" mpfr
fi

if [ ! -d mpc ]; then
  ln -s "../mpc-$CROSS_TC_MPC_VER" mpc
fi

echo "Building binutils"

BUILD_BINUTILS="${CROSS_TC_BUILD_ROOT}/build-binutils-${CROSS_TC_TARGET}"
mkdir -p "$BUILD_BINUTILS"
cd "$BUILD_BINUTILS"

"$CROSS_TC_SRC_ROOT/binutils-$CROSS_TC_BINUTILS_VER/configure" \
  --target="$CROSS_TC_TARGET" \
  --prefix="$CROSS_TC_PREFIX" \
  --with-sysroot \
  --disable-nls \
  --disable-werror

make -j"$CROSS_TC_JOBS"
make install

echo "Building GCC (freestanding, no libc, with freestanding libstdc++)"

BUILD_GCC="${CROSS_TC_BUILD_ROOT}/build-gcc-${CROSS_TC_TARGET}"
mkdir -p "$BUILD_GCC"
cd "$BUILD_GCC"

"$CROSS_TC_SRC_ROOT/gcc-$CROSS_TC_GCC_VER/configure" \
  --target="$CROSS_TC_TARGET" \
  --prefix="$CROSS_TC_PREFIX" \
  --disable-nls \
  --enable-languages=c,c++ \
  --without-headers \
  --disable-hosted-libstdcxx

make all-gcc -j"$CROSS_TC_JOBS"
make all-target-libgcc -j"$CROSS_TC_JOBS"
make all-target-libstdc++-v3 -j"$CROSS_TC_JOBS"

make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3

echo
echo "Done."
echo "Cross compiler installed to: $CROSS_TC_PREFIX"

export LD_LIBRARY_PATH=$TMP_LD_LIBRARY_PATH
export CFLAGS=$TMP_CFLAGS
export CXXFLAGS=$TMP_CXXFLAGS
