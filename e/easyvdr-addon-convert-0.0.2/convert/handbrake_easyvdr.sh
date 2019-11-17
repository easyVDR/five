#!/bin/bash
#
# Script aus VDR-Portal, keine Ahnung wer es veröffentlicht hat.
# mango 14.08.2015 Script an easyVDR angepasst
# mblaster4711 16.10 2015 Startparameter "nice -n 19" hinzugefügt
#
# convert mit HanbrakeCLI Profile --preset=
# https://trac.handbrake.fr/wiki/BuiltInPresets
#
OUTDIRDEFAULT=/media/easyvdr01/convert/divx
SVDRPSEND="/usr/bin/svdrpsend -p 6419"

# DO NOT CHANGE ANYTHING FROM HERE!
HANDBRAKEBIN="$(which HandBrakeCLI)"

PROFILE=${1}
DIRECTORY=${2}

OUTDIRDEFAULT=${OUTDIRDEFAULT}/$(echo ${PROFILE} | sed "s/ /_/g" | sed 's/[^a-zA-Z0-9_-]//g')

mkdir -p "${OUTDIRDEFAULT}"

${SVDRPSEND} MESG \"Start converting\"
for recording in $(find "${DIRECTORY}" -name "*.rec")
do
    	recpath=$(cd "${recording}"; pwd)
    	recordingname="$(basename $(dirname "${recpath}"))"
    	newrecname=$(echo ${recordingname} | sed "s/ /_/g" | sed 's/[^a-zA-Z0-9_-]//g')
    	vdrfilename="${OUTDIRDEFAULT}/${newrecname}.vdr"
    	handbrakeoutfilename="${OUTDIRDEFAULT}/${newrecname}.mp4"
    	echo "Start converting to ${vdrfilename}"
    	${SVDRPSEND} MESG \"Start converting ${newrecname}\"
    	echo "add files to ${vdrfilename}"
    	for videofile in $(find "${recording}" \( -name "[0-9][0-9][0-9].vdr" -o -name "[0-9][0-9][0-9][0-9][0-9].ts" \)|sort)
    	do
            	echo "  ${videofile}"
            	cat "${videofile}" >> "${vdrfilename}"
    	done
    	echo " done"
    	echo "Convert with ${HANDBRAKEBIN}"
    	CMD="nice -n 19 ${HANDBRAKEBIN} -d -i \"${vdrfilename}\" -o  \"${handbrakeoutfilename}\" --preset=\"${PROFILE}\" "
    	echo ${CMD}
    	eval ${CMD}
    	rm "${vdrfilename}"
    	echo " done"
done
${SVDRPSEND} MESG \"Finished converting\"
