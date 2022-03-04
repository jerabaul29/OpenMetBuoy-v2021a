#!/bin/bash

########################################################################################################################
# PREAMBLE

# for any of these " set " commands, use " +o " instead of " -o " to unset instead of set
# exit if a command fails; to circumvent, can add specifically on commands that can fail safely: " || true "
set -o errexit
# make sure to show the error code of the first failing command
set -o pipefail
# do not overwrite files too easily
set -o noclobber
# exit if try to use undefined variable
set -o nounset
# on globbing that has no match, return nothing, rather than return the dubious default ie the pattern itself
# see " help shopt "; use the -u flag to unset (while -s is set)
shopt -s nullglob

source "${HOME}/Desktop/Git/IceCream-Bash/src/ic.sh"

icp "fetching Rock7 data..."
curl "https://rockblock.rock7.com/RockBlockAdmin?filterDeviceAssignmentId=&filterDirection=&filterDateFrom=04%2FMar%2F2022+00%3A00%3A00&filterDateTo=31%2FDec%2F2022+23%3A59%3A59&format=csv&page=messages&action=getMessages" \
  -H 'Connection: keep-alive' \
  -H 'Pragma: no-cache' \
  -H 'Cache-Control: no-cache' \
  -H 'Accept: application/json, text/javascript, */*; q=0.01' \
  -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.66 Safari/537.36' \
  -H 'X-Requested-With: XMLHttpRequest' \
  -H 'Sec-Fetch-Site: same-origin' \
  -H 'Sec-Fetch-Mode: cors' \
  -H 'Sec-Fetch-Dest: empty' \
  -H 'Referer: https://rockblock.rock7.com/Operations' \
  -H 'Accept-Language: en-US,en;q=0.9,no;q=0.8' \
  -b cookie.jar \
  --compressed \
  -o "./all.csv"

IS_VALID="TRUE"
