#!/bin/ bash

set -Eeuxo pipefail

OUT_DIR="/home/vincent/Desktop/kBioReg/build" 

REGEX="AA.A.GA.G.CT.C.|+.T.T.T."
NUMBER=100;

./benchmark $REGEX $NUMBER $OUT_DIR/words.txt
bash ../bash_scripts/simulate.sh
