#!/bin/bash
if (( $# == 1 )); then
  source $1
else
  source /home/pi/Brewer/env_var.sh
fi
/home/pi/Brewer/BrewerEmbeddedServer
