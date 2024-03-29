#include <iostream>
#include <stack>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include "nfa_pointer.h"

/*
 * Represents an NFA state plus zero or one or two arrows exiting.
 * if c == Match, no arrows out; matching state.
 * If c == Split, unlabeled arrows to out and out1 (if != NULL).
 * If c < 256, labeled arrow with character c to out.
 */

/* Allocate and initialize State*/

State* state(const int& c, State *out1, State *out2)
{
  State *s = new State;
  s->c_ = c;
  s->out1_ = out1;
  s->out2_ = out2;
  s->lastlist_ = 0;

  return s;
}


/*
 * A partially built NFA without the matching state filled in.
 * Frag.start points at the start state.
 * Frag.out is a list of places that need to be set to the
 * next state for this fragment.
 */

Frag frag(State *start, 	std::vector<State *> out)
{
  Frag n = { start, out };
  return n;
}
//makes one vektor only contains the pointer of the last state
std::vector<State *> getVec(State *input)
{
	std::vector<State *> out{input};

	return out;
}
// concats vec1 & vec2
std::vector<State *> appendVec(const	std::vector<State *>& vec1, const std::vector<State *>& vec2)
{
	std::vector<State *> out = vec1;
	for(auto e : vec2)
	{
		out.push_back(e);
	}
	return out;
}
// every state in "in" points with one pointer on "s"
void patchVec(std::vector<State *>& in, State *s)
{
	for(auto e : in)
	{
		//e->out1_ == nullptr ? e->out1_ = s : e->out2_ = s; //out1_ never nullptr
		e->out1_ = s; 		//out1_ never nullptr //out1_ is 0 in z.128, 135, 142
		//e->out2_ = s; komplement z.81
	}
}

/*
 * Convert postfix regular expression to NFA.
 * Return start state.
 */

State* post2nfaE(const std::string& postfix)
{
  std::stack<Frag> stack;
  Frag e,e1,e2;
  State *s;

	State *matchstate = state(Match, nullptr, nullptr);

	if(postfix.empty()) return nullptr;

  for(size_t i = 0; i < postfix.size(); i++)
  {
    char p = postfix[i];
    switch(p){
      default: //char
              s = state(p, nullptr, nullptr);
							stack.push(frag(s, getVec(s)));
              break;
      case '.':  //concat
              e2 = stack.top();
              stack.pop();
              e1 = stack.top();
              stack.pop();
							patchVec(e1.out, e2.start);
              //patch(e1.out, e2.start);
              stack.push(frag(e1.start, e2.out));
              break;
      case '|':   //or
              e2 = stack.top();
              stack.pop();
              e1 = stack.top();
              stack.pop();
              s = state(Split, e1.start, e2.start);
							stack.push(frag(s, appendVec(e1.out, e2.out)));
              break;
      case '?': //0 or 1
              e = stack.top();
              stack.pop();
              s = state(Split, nullptr, e.start);
							stack.push(frag(s, appendVec(e.out, getVec(s))));
              //stack.push(frag(s, append(e.out, list1(&s->out2_))));
              break;
      case '*': //kleenestar
              e = stack.top();
              stack.pop();
              s = state(Split, nullptr, e.start);
							patchVec(e.out, s);
							stack.push(frag(s, getVec(s)));
              break;
      case '+': //one or more
              e = stack.top();
              stack.pop();
              s = state(Split, nullptr, e.start);
							patchVec(e.out, s);
							stack.push(frag(e.start, getVec(s)));
							break;
    }
  }
  e = stack.top();
  stack.pop();
  if(!stack.empty())
 	{
		std::cerr<<"Somthing went wrong, regex not in postfix?"<<"\n";
		return nullptr;
	}
	patchVec(e.out,  matchstate);
  return e.start;
}

/* 
 * generates a randomized word
 * that are part of the language that the automaton represents 
 */
std::string getRandomWord(State* startptr)
{
	std::string out{};
	State* itptr = startptr;
	int way = rand() % 2+1; // rand nr between 1-2;
	while(itptr->c_ != Match)
	{
		if(itptr->c_ != Split)
		{
			out.push_back(itptr->c_);
			itptr = itptr->out1_;
		}
		else
		{
			way == 1 ? itptr = itptr->out1_ : itptr = itptr->out2_;
			way = rand() % 2+1;
		}
	}
	return out;
}

/* 
 * Helpfunction from deleteGraph
 */
void add(std::vector<State* >& a, State* b)
{
	if(b->c_ != -1)
	{
		a.push_back(b);
		b->c_ = -1;
		if(b->out1_ != nullptr)
			add(a, b->out1_);
		if(b->out2_ != nullptr)
			add(a, b->out2_);
	}
}


/* deletes all pointer of the nfa
 */
void deleteGraph(State* startptr)
{
	std::vector<State* > a{};
	add(a, startptr);
	std::sort(a.begin(), a.end());
 	auto last = std::unique(a.begin(), a.end());
	a.erase(last, a.end());
	for(auto e : a)
	{
		delete e;
	}
}
