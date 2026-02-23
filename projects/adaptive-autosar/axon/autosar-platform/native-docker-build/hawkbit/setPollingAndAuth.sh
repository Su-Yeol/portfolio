#!/bin/bash

source baseurl

function setPollingTime {
    TARGET="${BASE_URL}/system/configs/pollingTime"
    HTTP_RESPONSE="$(curl --netrc-file .netrc -i -X PUT $TARGET -H 'Content-Type: application/json;charset=UTF-8' \
-d ' {
  "value" : "00:00:30"
} ')"
    echo "Response Code:" ${HTTP_RESPONSE}

    TARGET="${BASE_URL}/system/configs/pollingOverdueTime"
    HTTP_RESPONSE="$(curl --netrc-file .netrc -i -X PUT $TARGET -H 'Content-Type: application/json;charset=UTF-8' \
-d ' {
  "value" : "00:00:30"
} ')"
    echo "Response Code:" ${HTTP_RESPONSE}
}

function setAuthenticationToken {
    TARGET="${BASE_URL}/system/configs/authentication.targettoken.enabled"
    HTTP_RESPONSE="$(curl --netrc-file .netrc -i -X PUT $TARGET -H 'Content-Type: application/json;charset=UTF-8' \
-d ' {
  "value" : true
} ')"
    echo "Response Code:" ${HTTP_RESPONSE}
}

setAuthenticationToken
setPollingTime
