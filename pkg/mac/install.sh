#!/bin/sh

set -eux

export PATH=/usr/bin:/bin:/usr/sbin:/sbin

cd "${GITHUB_WORKSPACE}"

mkdir -p "${DST_DIR}"

cflags='-g -O2 -std=c89 -Wno-error=implicit-function-declaration'
# missing symbol ___chkstk_darwin ?
cflags="${cflags} -mmacosx-version-min=10.6 -fno-stack-check -fno-stack-protector"

make -f ./sys/unix/GNUmakefile \
     CC=clang CXX=clang++ \
     CFLAGS="${cflags}" \
     PREFIX="${DST_DIR}" \
     install
