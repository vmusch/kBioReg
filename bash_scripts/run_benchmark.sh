

ceil() {                                                                       
  echo "define ceil (x) {if (x<0) {return x/1} \
        else {if (scale(x)==0) {return x} \
        else {return x/1 + 1 }}} ; ceil($1)" | bc
}


#ceil((n * log(p)) / log(1 / pow(2, log(2))));
#n=$1 p=$2 
f(){
    echo "($1*(l($2)/l(10)))/(l(0.6185)/l(10))" | bc -l
}

g(){
    echo "$1/$2" | bc
}

#100 kB, 1MB, 10 MB, 100 MB, 1 GB;
declare -a SizeArray=("1048576" "2097152" "4194304" "6291456" "8388608" "10485760" "20971520" "41943040" "62914560"  "83886080" "104857600") #("102400" "1048576" "10485760" "104857600" "1073741824")
declare -a BinArray=("256" "512" "1024" "2048" "4096" "8192" "16384")
declare -a RegArray=("AAAAAAAAAATGAAAAAAAAAA" "AAAAAAAAAA(TG)*AAAAAAAAAA" "AAAAAAAAAA(TG)+AAAAAAAAAA" "AAAAAAAAAA(TG)?AAAAAAAAAA" "AAAAAAAAAA(TG|CG)AAAAAAAAAA")
#R="AAAAAAGGGGGG(TTTTTT|AAAAAA)CCCCCCAAAAAA"
R="AAAAAAAAAA(A|C|T|G)(A|C|T|G)(A|C|T|G)AAAAAAAAAA"
declare -a kArray=(2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)
I="0"
#for R in ${RegArray[@]}; do
S="104857600"
K=23

  > benchmark_table.txt


  B="1024"   
  #echo "path,set"  >> benchmark_table.txt
  #echo "path,null,eins,set"  >> benchmark_table.txt
  echo "index_time,load_index,construct_NFA,construct_KNFA,compute_paths,compute_binVec,query_time,search_time,global_time,genom_size,querry_hits,bins,std_time,std_hits" >> benchmark_table.txt
    #for S in ${SizeArray[@]}; do
        #for K in ${kArray[@]}; do

            IBF_SIZE=$(g $S $B)
            echo $IBF_SIZE
            IBF_SIZE=$(f $IBF_SIZE 0.001)
            echo $IBF_SIZE
            IBF_SIZE=$(ceil $IBF_SIZE)
            echo $IBF_SIZE
            ./bin/kbioreg benchmark -k $K -w 100 -b $B -j $S -s $IBF_SIZE -m na $R 
            #J=($(wc -l graph.dot | cut -d " " -f 1))
            # let "J -= 2"
            # let "J = J/2"
            # echo "$J" >> benchmark_table.txt
            # sed '$a\n' benchmark_table.txt
            rm -rf $B/
            #rm -rf graph.dot
        #done
    #done
  #mv benchmark_table.txt /home/vincent/new_bench/fp/$I.txt
  let "I+=1"
#done