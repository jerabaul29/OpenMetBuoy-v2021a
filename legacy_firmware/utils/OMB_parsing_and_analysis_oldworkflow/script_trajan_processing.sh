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

source "ic.sh"

########################################################################################################################
# which kind of update method should we use?

# FILE_SOURCE="downloads"
FILE_SOURCE="apirequest"
# FILE_SOURCE="all.csv"  # use the "all.csv" file already in the folder

########################################################################################################################
# update csv file from downloads folder

if [[ "${FILE_SOURCE}" == "downloads" ]]; then

  icp "use last file from downloads folder method..."

  LAST_FILE=$(ls -rt ~/Downloads | tail -1)
  ic LAST_FILE

  PREFIX="${LAST_FILE:0:9}"
  SUFFIX="${LAST_FILE: -4:4}"

  ic PREFIX
  ic SUFFIX

  IS_VALID="FALSE"

  if [[ "${SUFFIX}" == ".csv" ]]; then
    if [[ "${PREFIX}" == "messages-" ]]; then
      icp "valid rock7 file"
      IS_VALID="TRUE"
    fi
  fi

  ic IS_VALID

  if [[ "${IS_VALID}" == "TRUE" ]]; then
    icp "copy rock7 file..."
    cp "${HOME}/Downloads/${LAST_FILE}" all.csv
  else
    icp "no rock7 file, exit..."
    exit 1
  fi

fi

########################################################################################################################
# update csv files from API request
# CAREFUL, this contains the password for the account

if [[ "${FILE_SOURCE}" == "apirequest" ]]; then
  icp "use api request method..."

  icp "get password..."

  if [[ -f "./password.secret" ]]; then
    icp "WARNING storing unencrypted secrets on disk is not recommended!"
    source "./password.secret"
    ic PASSWORD
    # PASSWORD=$(urlencode "$PASSWORD")
    PASSWORD=$(echo "$PASSWORD" | head -c -1 | jq -sRr @uri)

  else
    # if you have another way to store your secret, implement here; I use pass the linux password manager
    PASSWORD="$(pass show Rock7/password | head -c -1  | jq -sRr @uri)"
  fi

  ic PASSWORD

  icp "get username..."

  if [[ -f "./username.secret" ]]; then
    icp "WARNING storing unencrypted secrets on disk is not recommended!"
    source "./username.secret"
    ic USERNAME
    # USERNAME="$(urlencode $USERNAME)"
    USERNAME="$(echo $USERNAME | head -c -1 | jq -sRr @uri)"

  else
    # if you have another way to store your secret, implement here; I use pass the linux password manager
    USERNAME="$(pass show Rock7/username | head -c -1  | jq -sRr @uri) "
  fi

  ic USERNAME

  icp "getting Rock7 session ID..."
  curl -c cookie.jar https://rockblock.rock7.com/Operations -o /dev/null

  icp "log in to Rock7..."

  icp "prepare request..."
  curl 'https://rockblock.rock7.com/Operations' \
    -H 'Connection: keep-alive' \
    -H 'Pragma: no-cache' \
    -H 'Cache-Control: no-cache' \
    -H 'Origin: https://rockblock.rock7.com' \
    -H 'Upgrade-Insecure-Requests: 1' \
    -H 'DNT: 1' \
    -H 'Content-Type: application/x-www-form-urlencoded' \
    -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/87.0.4280.66 Safari/537.36' \
    -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9' \
    -H 'Sec-Fetch-Site: same-origin' \
    -H 'Sec-Fetch-Mode: navigate' \
    -H 'Sec-Fetch-User: ?1' \
    -H 'Sec-Fetch-Dest: document' \
    -H 'Referer: https://rockblock.rock7.com/Operations' \
    -H 'Accept-Language: nb-NO,nb;q=0.9,no;q=0.8,nn;q=0.7,en-US;q=0.6,en;q=0.5' \
    -b cookie.jar \
    --data-raw "sticky=false&u=${USERNAME}&p=${PASSWORD}&Submit=Log+in" \
    --compressed \
    -o /dev/null

  if [[ -f "./all.csv" ]]; then
    icp "remove old ./all.csv Rock7 data..."
    rm "./all.csv"
  fi

  # TODO: make it easier to select some specific parts of the data
  
  icp "fetching Rock7 data..."
  curl "https://rockblock.rock7.com/RockBlockAdmin?filterDeviceAssignmentId=&filterDirection=&filterDateFrom=22%2FJul%2F2022+00%3A00%3A00&filterDateTo=30%2FJun%2F2033+23%3A59%3A59&format=csv&page=messages&action=getMessages" \
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

fi

########################################################################################################################
# run the python scripts

if [[ "${IS_VALID}" == "TRUE" ]]; then
  icp "process the OMB data with trajan and pack in netCDF-CF compliant file..."
  python3 script_process_with_trajan.py
fi
