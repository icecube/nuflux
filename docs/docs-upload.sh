#!/bin/bash

PROJECT_NAME='nuflux'
BRANCH_NAME='main'
HTML_DIR='build/docs/html'
GIT_BRANCH=`git rev-parse --abbrev-ref HEAD`
GIT_REVISION=`git rev-parse --verify --short HEAD`
TARBALL_NAME="${PROJECT_NAME}-docs-${GIT_BRANCH}-${GIT_REVISION}.tar.gz"

if [ -z "${ICECUBE_PASSWORD}" ]; then
    echo ICECUBE_PASSWORD not set, skipping upload
    exit 0
fi

if [ "${GIT_BRANCH}" != "${BRANCH_NAME}" ]; then
    echo "not on ${BRANCH_NAME} branch, skipping upload"
    exit 0
fi

TARCMD="tar -czvf $TARBALL_NAME -C${HTML_DIR} ."
echo running $TARCMD
${TARCMD}

URL="https://docs.icecube.aq/api/upload?path=${PROJECT_NAME}/${BRANCH_NAME}"
UPLOADCMD="curl -XPUT -i --data-binary @${TARBALL_NAME} ${URL} -u icecube:${ICECUBE_PASSWORD}"
echo running ${UPLOADCMD}
${UPLOADCMD}
exit $?
