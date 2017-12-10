#!/bin/bash 

pd -nogui -lib ossia 2>&1 -send "pd quit;" >/dev/null | grep "Welcome to ossia library"

exit $?