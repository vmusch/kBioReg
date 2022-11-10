#!/usr/bin/env bash

# --------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/raptor/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------

set -Eeuxo pipefail

BINARY_DIR="/home/vincent/Desktop/kBioReg/data/bin" # Dir containing "mason_genome", "split_sequence", etc.
OUT_DIR="/home/vincent/Desktop/kBioReg/build" # Where simulated data should be stored
LENGTH=419430400 # 4*2^30 =  4GiB
SEED=42 # was 20181406 before, but was hardcoded to 42 in seqan
BIN_NUMBER=64
ERRORS=2
READ_LENGTHS="100 150 250"
READ_COUNT=1048576
HAPLOTYPE_COUNT=1

output_dir=$OUT_DIR/$BIN_NUMBER
bin_dir=$output_dir/bins
info_dir=$output_dir/info

mkdir -p $output_dir
mkdir -p $bin_dir
mkdir -p $info_dir

bin_length=$((LENGTH / BIN_NUMBER))
echo "Simulating $BIN_NUMBER bins with reference length of $LENGTH and bin_length of $bin_length"

# Simulate reference
echo "Simulating genome"
$BINARY_DIR/mason_genome -l $LENGTH -o $bin_dir/ref.fasta -s $SEED &>/dev/null
# Evenly distribute it over bins
echo "Splitting genome into bins"
$BINARY_DIR/split_sequence --input $bin_dir/ref.fasta --length $bin_length --parts $BIN_NUMBER
# We do not need the reference anymore
rm $bin_dir/ref.fasta
#add words to fasta file
echo "add words"
> bins.txt
occ=0
all=0
for i in $bin_dir/*.fa
do
    rand=$((1 + $RANDOM % 100))
    if [ $rand -le $1 ]
        then
            occ=$((occ+1))
            shuf -n 1 $OUT_DIR/words.txt >> $i
            echo "$i," >> bins.txt
    fi
    all=$((all+1))
done
echo "Occurence: $occ"
echo "all: $all"
echo "rate: $(($occ*100 / $all))"

cat $bin_dir/*.fa >> $bin_dir/all_bins.fa
<< not_necessery
for read_length in $READ_LENGTHS
do
    echo "Generating $READ_COUNT reads of length $read_length with $ERRORS errors"
    read_dir=$output_dir/reads_e$ERRORS\_$read_length
    mkdir -p $read_dir
    list_file=$read_dir/all_bins.txt
    seq -f "$output_dir/bins/bin_%0${#BIN_NUMBER}.0f.fa" 0 1 $((BIN_NUMBER-1)) > $list_file     #.
    $BINARY_DIR/generate_reads \
        --output $read_dir \
        --max_errors $ERRORS \
        --number_of_reads $READ_COUNT \
        --read_length $read_length \
        --number_of_haplotypes $HAPLOTYPE_COUNT \
        $list_file > /dev/null
    rm $list_file
    find $read_dir -type f -name "*.fastq" -print0 | sort -zV | xargs -0 cat > $read_dir/all
    mv $read_dir/all $read_dir/all.fastq
    for i in $(seq 0 9); do cat $read_dir/all.fastq >> $read_dir/all_10.fastq; done
done
not_necessery