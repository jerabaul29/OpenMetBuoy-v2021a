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

while true
do
        echo "generate now..."
        date

        bash script_regenerate_all.sh > /dev/null 2>&1

        echo "...done for now"

        # generate every 1 hour
        echo "sleep 1 hour"
        sleep 3600
done

