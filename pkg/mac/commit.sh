#!/bin/sh

cd "${GITHUB_WORKSPACE}"
git rev-list -n 1 HEAD~2
