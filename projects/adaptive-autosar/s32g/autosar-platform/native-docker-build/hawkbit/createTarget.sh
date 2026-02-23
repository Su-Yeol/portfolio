#!/bin/bash

source baseurl

function createTarget {
  curl --netrc-file .netrc "${BASE_URL}/targets" -i -X POST \
    -H 'Content-Type: application/json' \
    -d '[ {
  "securityToken" : "bhVahL1Il1shie2aj2poojeChee6ahShu",
  "address" : "",
  "controllerId" : "ota-client",
  "name" : "apdOtaClient",
  "description" : "test"
} ]'
}

createTarget
