#!/bin/bash

source baseurl

# this script is intended to be called from radarfusion or fusion machine
SWP_FOLDER="/usr/share/apdtest/ucm/vpm"

# upload artifact and return handle
function uploadArtifact {
    local SOFTWARE_MODULE_ID=$1
    local FILENAME=$2
    echo "Uploading ${FILENAME} to module ${SOFTWARE_MODULE_ID}"
    curl --netrc-file .netrc "${BASE_URL}/softwaremodules/${SOFTWARE_MODULE_ID}/artifacts" -X POST \
    -H 'Content-Type: multipart/form-data' \
    -F "file=@$FILENAME"
}

# create software module and echo id given by server
function createSoftwareModule {
    local SOFTWARE_MODULE_NAME=$1
    local SOFTWARE_MODULE_TYPE=$2
    local SOFTWARE_MODULE_DESCRIPTION=$3
    echo "Creating software module ${SOFTWARE_MODULE_NAME} (${SOFTWARE_MODULE_TYPE})"
    RESPONSE="$(curl --netrc-file .netrc ${BASE_URL}/softwaremodules \
    -H 'Content-Type: application/hal+json' \
    -d '[ {
  "vendor" : "AUTOSAR",
  "name" : "'"${SOFTWARE_MODULE_NAME}"'",
  "description" : "'"${SOFTWARE_MODULE_DESCRIPTION}"'",
  "type" : "'"${SOFTWARE_MODULE_TYPE}"'",
  "version" : "1.0"
} ]')"
    SOFTWARE_MODULE_ID=$(echo $RESPONSE | jq '.[0].id')
    echo $RESPONSE > sw.json
}

function createDistributionManagement {
    local DISTRIBUTION_SET_NAME=$1
    local DISTRIBUTION_SET_SOFTWARE_MODULE_ID=$2
    local DISTRIBUTION_SET_DESCRIPTION=$3
    echo "Creating distribution set ${DISTRIBUTION_SET_NAME} with sw module ${DISTRIBUTION_SET_SOFTWARE_MODULE_ID}, desc ${DISTRIBUTION_SET_DESCRIPTION}"
    RESPONSE="$(curl --netrc-file .netrc "${BASE_URL}/distributionsets/" -X POST \
    -H 'Content-Type: application/json' \
    -d '[ {
  "name" : "'"${DISTRIBUTION_SET_NAME}"'",
  "description" : "'"${DISTRIBUTION_SET_DESCRIPTION}"'",
  "type" : "os",
  "version" : "1.0",
  "modules" : [ {
    "id": "'"${DISTRIBUTION_SET_SOFTWARE_MODULE_ID}"'"
  }]
}]')"
    DISTRIBUTION_MODULE_ID=$(echo $RESPONSE | jq '.[0].id')
    echo $RESPONSE > ds.json
}

function createFilter {
    curl --netrc-file .netrc "${BASE_URL}/targetfilters" -X POST \
      -H 'Content-Type: application/json' \
      -d '{
    "query" : "name==*",
    "name" : "filter1"
  }'
}

function rollout {
  curl --netrc-file .netrc "${BASE_URL}/rollouts" -i -X POST \
    -H 'Content-Type: application/hal+json' \
    -H 'Accept: application/hal+json' \
    -d '{
  "distributionSetId" : "'"${DISTRIBUTION_MODULE_ID}"'",
  "targetFilterQuery" : "name==*",
  "description" : "Rollout for all targets",
  "amountGroups" : 5,
  "type" : "forced",
  "successCondition" : {
    "condition" : "THRESHOLD",
    "expression" : "50"
  },
  "name" : "exampleRollout",
  "forcetime" : 0,
  "errorAction" : {
    "expression" : "",
    "action" : "PAUSE"
  },
  "confirmationRequired" : false,
  "errorCondition" : {
    "condition" : "THRESHOLD",
    "expression" : "80"
  },
  "startAt" : 0
}'
}

createSoftwareModule update_itm_simple os "Campaign used in the ITM setup for RadarFusion"
echo "New module is ${SOFTWARE_MODULE_ID}"
uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/ucm_vpm_simple.arvp
uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/SWP_TEST2_INS.zip
uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/SWP_TEST2_AGL_INS.zip
uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/SWP_TEST2_FUS_INS.zip
createDistributionManagement campaign_itm_simple ${SOFTWARE_MODULE_ID} "distribution set for ITM simple campaign"

createSoftwareModule update_itm_multi os "Campaign used in the ITM setup for Radar and Fusion"
echo "New module is ${SOFTWARE_MODULE_ID}"
uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/ucm_vpm_multi.arvp
uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/SWP_TEST2_INS.zip
uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/SWP_TEST2_AGL_INS.zip
uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/SWP_TEST2_FUS_INS.zip
createDistributionManagement campaign_itm_multi ${SOFTWARE_MODULE_ID} "distribution set for ITM multi campaign"

for mode in dualBank singleBank; do
  for case in depFail goodCase_V1 goodCase_V2 VersionFail; do
    createSoftwareModule classic_${mode}_${case} os "Classic test case ${mode}_${case}"
    uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/classic_${mode}_${case}.arvp
    uploadArtifact ${SOFTWARE_MODULE_ID} ${SWP_FOLDER}/${mode}_${case}.zip
    createDistributionManagement classic_${mode}_${case} ${SOFTWARE_MODULE_ID} "distribution set for classic_${mode}_${case}"
  done
done
