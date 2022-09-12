# Bachelor
The code constructed from a regex to korotkov automata.
A korotkov automat is an nfa with Q-gram/K-mer transitions.
It is based on this lecture https://wiki.postgresql.org/images/6/6c/Index_support_for_regular_expression_search.pdf

A matrix is generated from the automata, which can be used for pattern matching.
The regex need to be written in reverse polish notation and supports the following operations:<br>
1. "|" - or <br>
2. "*" - kleene star<br>
3. "+" - min. 1<br>
4. "?" - 0 or 1<br>
5. "." - concatination<br>

example:<br>
a(b^+|c^+)d => ab+c+|.d.<br>

text in brackets is the explanatory<br>
$ make main<br>
$./main "ab+c+|.d."(regex) "3"(length of q-gram) "Matrix"(optianl .txt file) "Automat"(optional .dot file)<br>

output:<br>
abb bbb bbd<br> 
abb bbd <br>
abd <br>
acc ccc ccd <br>
acc ccd <br>
acd <br>

the .dot file can be transformed int an png file with the following command.<br>
$ dot -Tpng out.dot > out.png
