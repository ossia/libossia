#!/bin/bash

## copy files from Unity project folder back to the repo folder when they are modified

SCRIPT_FOLDER=`dirname $(greadlink -f $0)`
REPO_ROOT=`realpath ${SCRIPT_FOLDER}/..`
SOURCE_FOLDER="${HOME}/Documents/Max 8/Packages/ossia/help"
DESTINATION_FOLDER=${REPO_ROOT}/help

echo "watch $SOURCE_FOLDER and target $DESTINATION_FOLDER"

if [[ ! -e $SOURCE_FOLDER ]]; then
	echo "Source folder $SOURCE_FOLDER doesn't exist"
	echo "Usage: ${0} <build folder>"
	exit 1
fi

spy run "${SCRIPT_FOLDER}/cp_swaped_args.sh ${DESTINATION_FOLDER}" "${SOURCE_FOLDER}"
