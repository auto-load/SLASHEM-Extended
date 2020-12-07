#!/bin/sh
set -eux
cd "${GITHUB_WORKSPACE}"
version="$(./util/version.sh)"
tagname="${GITREF##*/}"
echo TAGNAME="${tagname}" >> "${GITHUB_ENV}"
echo DST_DIR="${HOME}/slex/build" >> "${GITHUB_ENV}"
echo APP_DIR="${HOME}/slex/app" >> "${GITHUB_ENV}"
echo PKG_DIR="${HOME}/slex/archive" >> "${GITHUB_ENV}"
echo SLEX_COMMIT="$(./pkg/mac/commit.sh)" >> "${GITHUB_ENV}"
echo SLEX_VERSION="${version}" >> "${GITHUB_ENV}"
echo SLEX_NAME="slex-${version}-mod-${tagname}" >> "${GITHUB_ENV}"
