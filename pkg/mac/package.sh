#!/bin/sh

set -eux

cd "${GITHUB_WORKSPACE}"

mkdir -p "${APP_DIR}" "${PKG_DIR}"

upx --ultra-brute "${DST_DIR}/slex-${SLEX_VERSION}/slex"

./pkg/makeself/makeself.sh \
    --bzip2 --complevel 9 --noprogress --nooverwrite \
    --target "${SLEX_NAME}" \
    "${DST_DIR}/slex-${SLEX_VERSION}" \
    "${APP_DIR}/${SLEX_NAME}.bzip2.command" \
    "SLASH'EM Extended (git commit: ${SLEX_COMMIT})" \
    ./play

(cd "${APP_DIR}" && zip -9 -r --symlinks "${PKG_DIR}/${SLEX_NAME}.zip" *)

echo
echo "::set-output name=filepath::${PKG_DIR}/${SLEX_NAME}.zip"
echo "::set-output name=filename::${SLEX_NAME}.zip"
echo "::set-output name=tagname::${TAGNAME}"
