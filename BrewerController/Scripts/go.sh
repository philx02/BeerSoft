#!/bin/bash
if (( $# == 1 )); then
  source $1
else
  source /home/chip/Brewer/env_var.sh
fi
/home/chip/BrewerController/BrewerEmbeddedServer
