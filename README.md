# Evaluation of Biological, RegEx with Korotkov NFA
This app constructs a Korotkov Automata from an input Regular Expression. A Korotkov Automata is an NFA with Q-gram/K-mer transitions. It is based on this [lecture] (https://wiki.postgresql.org/images/6/6c/Index_support_for_regular_expression_search.pdf)

A matrix is generated from the automata, which can be used for pattern matching. The regex need to be written in reverse polish notation and supports the following operations:

1. "|" - or
2. "*" - kleene star
3. "+" - min. 1
4. "?" - 0 or 1
5. "." - concatination


## Installation

1. Clone the repository with ``git clone --recurse-submodules git@github.com:remyschwab/kBioReg.git``
2. Descend into the home directory and input: ``mkdir build && cd build``
3. Configure with cmake ``cmake ..``
4. Build with make ``make``

## Example
``./bin/kbioreg "ab+c+|.d." 3 "Matrix"(optional .txt file) "Automat"(optional .dot file)``

Outputs:
```
abb bbb bbd
abb bbd
abd
acc ccc ccd
acc ccd
acd
```

the .dot file can be transformed int an png file with the following command.
``dot -Tpng out.dot > out.png``
