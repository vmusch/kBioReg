#!/bin/ bash

set -Eeuxo pipefail

OUT_DIR="/home/vincent/Desktop/kBioReg/build" 

REGEX="AA.A.GA.G.CT.C.|+.T.T.T."
NUMBER=100
percent=20

./bin/kbioreg words -k $NUMBER words.txt $REGEX
bash ../bash_scripts/simulate.sh $percent
