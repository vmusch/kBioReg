
## Variables:
- Genome size
- kmer size
- Pattern Size and Complexity

## Also...
- Bin Size
- Hash Count
- FPR from Bloom Filter
- Number of occurences

## Steps:
1. Make three genomes of different sizes
	- 31kb as a viral genome
	- 420 Megabases for bacteria
	- 3.1 Gigabases for mammals/human genome
2. Index each genome for different values of k (5, 7, 11, 17, 21, 31)
3. Test a set of queries on each index (6x3=18 different runs)

## Analysis
- We probably won't end up needing to look at all the kmer sizes
- Start by simply calculating and comparing the number of positives we get from the tool and the actual number we get

## Queries
```
"AC+G+|.T."
```

