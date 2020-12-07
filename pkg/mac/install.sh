#!/bin/sh

set -e -x

export PATH=/usr/bin:/bin:/usr/sbin:/sbin

make -f sys/unix/GNUmakefile PREFIX="$1" install
