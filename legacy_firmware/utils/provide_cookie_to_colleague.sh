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
FILE_SOURCE="apirequest"

########################################################################################################################
# get a cookie to be able to download data, that can be shared to a colleague
# CAREFUL, this contains the password for the account

if [[ "${FILE_SOURCE}" == "apirequest" ]]; then
  icp "use api request method..."

  icp "get password..."

  if [[ -f "./password.secret" ]]; then
    icp "WARNING storing unencrypted secrets on disk is not recommended!"
    source "./password.secret"
  else
    # if you have another way to store your secret, implement here; I use pass the linux password manager
    PASSWORD="$(pass show Rock7/password | head -c -1  | jq -sRr @uri)"
  fi

  icp "get username..."

  if [[ -f "./username.secret" ]]; then
    icp "WARNING storing unencrypted secrets on disk is not recommended!"
    source "./username.secret"
  else
    # if you have another way to store your secret, implement here; I use pass the linux password manager
    USERNAME="$(pass show Rock7/username | head -c -1  | jq -sRr @uri) "
  fi

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
fi
