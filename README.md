# sudoku-dlx
Sudoku solver, finds solutions to the exact cover problem using Algorithm X, as described by Donald Knuth in his Dancing Links paper:
https://arxiv.org/pdf/cs/0011047.pdf

Short summary:
1. From a sudoku, create a constraint matrix (a representation of the given clues and all possible candidates).
2. Represent the constraint matrix with a doubly linked list that is circular both horizontally and vertically.
3. Use Algorithm X on the list to find the solution(s).
4. Convert the solution(s) to readable form.

Apologies for the horrible code, this is my first C++ project, I am still learning :D
