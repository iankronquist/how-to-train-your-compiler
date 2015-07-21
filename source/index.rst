
.. How To Train Your Compiler: The Dragon Book, condensed slides file, created by
   hieroglyph-quickstart on Fri Apr 17 10:25:20 2015.

How To Train Your Compiler
==========================

How To Train Your Compiler
--------------------------
*The Dragon Book, condensed*

.. figure:: /_static/DragonMedium.png
	:align: center

Ian Kronquist
-------------

* iankronquist@gmail.com
* Senior studying Computer Science at Oregon State University
* Developer at the OSU Open Source Lab
* Summer 2015 intern at Puppet Labs on the language team

.. figure:: /_static/osllogo.png
	:align: center

The Talk
--------

* **Part 1: Compilers**

  - Examples in simple C
  - The front end vs. the back end
  - Lexing and Parsing
  - Analysis and optimization
  - Pros and Cons

* **Part 2: Interpreters**

  - Examples in Python
  - Builds on part 1
  - Walking the Abstract Syntax Tree vs. Bytecode VMs
  - Pros and cons

The Dragon Book
---------------

* The grand old textbook on the subject.

* Writing a compiler is as dangerous and complicated as slaying a dragon.

.. figure:: /_static/dragon_book.jpg
	:width: 30%
	:align: center


The difference between compilers and interpreters
-------------------------------------------------

* Compilers take in human readable source code and emit machine executable
  instructions or byte code, usually as an executable file.
* Interpreters take in human readable source code and execute it directly.
* It's not just black and white. There are lots of things in between, often
  called Just In Time compilers.

Compilation
-----------

.. code-block:: c

	#include <stdio.h>

	int main() {
		printf("Hello world!");
	}

::

	$ hexdump a.out
	0000000 cf fa ed fe 07 00 00 01 03 00 00 80 02 00 00 00
	0000010 10 00 00 00 10 05 00 00 85 00 20 00 00 00 00 00
	0000020 19 00 00 00 48 00 00 00 5f 5f 50 41 47 45 5a 45
	0000030 52 4f 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	0000040 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00
	0000050 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	0000060 00 00 00 00 00 00 00 00 19 00 00 00 28 02 00 00
	0000070 5f 5f 54 45 58 54 00 00 00 00 00 00 00 00 00 00
	0000080 00 00 00 00 01 00 00 00 00 10 00 00 00 00 00 00
	0000090 00 00 00 00 00 00 00 00 00 10 00 00 00 00 00 00
	...


What happened?
--------------

* Preprocessing and Macro expansion. (Not today)
* Compiler Front end:
	* Lexing/tokenizing.
	* Parsing.
	* Compilation to an intermediate representation.
* Compiler Back end:
	* Platform independent optimization.
	* Platform specific optimizations.
	* Compilation to target machine code (Mach-O 64-bit executable x86_64)
* Linking. Putting together multiple compiled files. (Not today)

Lexical Analysis
----------------

The code needs to be split into tokens.

.. code-block:: c

	// this is a comment
	int a = 1 + 3;
	func ( a, b, c )

Becomes something like:

.. code-block:: python

	['int', 'a', '=', '1', '+', '3']
	['func', '(', 'a', ',', 'b', ',', 'c', ')']

Parsing and Grammars
--------------------

"All your base are belong to us" doesn't make a lot of sense.

Programming languages are like natural languages. They follow a set of rules
called a grammar.

Parsing happens according to a grammar. Grammars need to specify what happens
in otherwise ambiguous situations. Consider this example in C.

.. code-block:: c

	c = a---b

Which does this mean?

.. code-block:: c

	c = (a--)-b
	c = a-(--b)

Backtracking
------------

Sometimes grammars can be ambiguous.

* "The old man the boat."
* "The cotton clothing is made of grows in Mississippi."
* "The florist sent the bouquet of flowers was very flattered."

Language designers should be careful to design grammars and choose parsing
strategies which avoid backtracking. It's expensive and confusing.

Defining a grammar with BNF notation
------------------------------------
Grammars are usually defined in Backus-Naur Form. Here is a small part of the
grammar for C.

::

	if_stat    : 'if' '(' exp ')' stat
			   | 'if' '(' exp ')' stat 'else' stat

::

	stat       : cmpd_stat
			   | if_stat
			   | iter_stat
			   | assgn_stat

::

	stat_list  : stat
			   | stat_list stat

Parsing
-------

* Possibly the hardest part.
* Build a special kind of tree from the code called an Abstract Syntax Tree

.. figure:: /_static/ast.png
	:align: center
	:width: 90%

Abstract Syntax Trees
---------------------

.. code-block:: c

	int main() {
		int a = 0;
		int b = 21;
		a = a/b;
		return a;
	}


.. figure:: /_static/worth_two_in_the_bush.jpg
	:align: left
	:width: 40%


.. nextslide::

An AST dumped from clang

::

	`-FunctionDecl main 'int ()'
	  `-CompoundStmt
	    |-DeclStmt
	    | `-VarDecl used a 'int' cinit
	    |   `-IntegerLiteral 'int' 0
	    |-DeclStmt
	    | `-VarDecl used b 'int' cinit
	    |   `-IntegerLiteral 'int' 21
	    |-BinaryOperator 'int' '='
	    | |-DeclRefExpr 'int' lvalue Var 'a' 'int'
	    | `-BinaryOperator 'int' '/'
	    |   |-ImplicitCastExpr 'int' <LValueToRValue>
	    |   | `-DeclRefExpr 'int' lvalue Var 'a' 'int'
	    |   `-ImplicitCastExpr 'int' <LValueToRValue>
	    |     `-DeclRefExpr 'int' lvalue Var 'b' 'int'
	    `-ReturnStmt
	      `-ImplicitCastExpr 'int' <LValueToRValue>
	        `-DeclRefExpr 'int' lvalue Var 'a' 'int'

Parsing Strategies
------------------

Parsing can be divided into two families

Bottom Up Parsing:

* Recursive descent
* LL: Leftmost, Left to right.

Top Down Parsing:

* Operator precedence
* LR: Left to Right
  - SLR: Simple Left to Right
  - Look Ahead Left to Right
* Shift-reduce parsing

There are parser generators such as ``bison`` which take a formal grammar
and emit C code which will parse it.

Look Ahead Left to Right Parsing
--------------------------------

* Most modern languages use a LALR parsing strategy.
* Build the AST from bottom to top, left to right.
* No guessing or backtracking.
* A type of Shift-Reduce parser, shifting through the list of tokens, and
  reducing the tree to use smaller symbols.

Single Static Assignment
------------------------

Original code:

.. code-block:: c

	a = 1
	b = a
	a = b + 1

SSA transformation:

.. code-block:: c

	a_1 = 1
	b_1 = a_1
	a_2 = b_1 + 1

Optimized result:

.. code-block:: c

	a_1 = 1
	a_2 = a_1 + 1


Compilation to Intermediate Representation
------------------------------------------

* Halfway between source code and machine code.
* Has detailed type information and annotations about what the
  source code means
* Also has machine details which can be used by the back end.
* IR is machine agnostic.


Three Address Code
------------------

.. code-block:: c

	a = b * c + d

.. code-block:: c

	t_1 = b * c
	t_2 = t_1 + d
	a = t_2

.. code-block:: asm

	mov    -0xc(%rbp), %eax
	imul   -0x10(%rbp), %eax

	mov    -0x14(%rbp), %ecx
	add    %ecx, %eax

	mov    %eax, -0x8(%rbp)

.. nextslide::

::

	; ModuleID = 'test.c'
	target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
	target triple = "x86_64-apple-macosx10.10.0"

	@.str = private unnamed_addr constant [13 x i8] c"Hello world!\00", align 1

	; Function Attrs: nounwind ssp uwtable
	define i32 @main() #0 {
	  %1 = call i32 @puts(i8* getelementptr inbounds ([13 x i8]* @.str, i32 0, i32 0))
	  ret i32 0
	}

	declare i32 @puts(i8*) #1

Building a Control Flow Graph
-----------------------------

A graph of the flow of the code through the program via function calls,
if statements, loops, etc.

.. figure:: /_static/control_flow_graph.png
	:align: center
	:width: 80%

Optimization
------------

* Make the program go faster! Remove redundant work.
* Some optimizations are safe, others are dangerous and can affect how the
  program runs.
* Code is often transformed into single static assignment form, where variables
  are only used once, and transformations are assigned to a new variable. This
  makes code easier to reason about and optimize.


Optimization
------------

Examples of optimizations:

* Function inlining: Jumping into functions is expensive. Sometimes a function
  can be stuck inside another function without affecting the program.
* Eliminating unused variables or return values.
* Eliminating dead code: Why compile it if you never use it?
* Transforming tail recursion: Transform tail recursion into a simple loop.
* Unrolling small loops: Sometimes multiple statements are faster than loops.
* Many many more.

Compilation to machine code
---------------------------

* There are more things to cover and this is sort of boring.

.. code-block:: asm

	_main:                                  ## @main
		.cfi_startproc
	Ltmp2:
		.cfi_def_cfa_register %rbp
		subq	$16, %rsp
		leaq	L_.str(%rip), %rdi
		callq	_puts
		xorl	%ecx, %ecx
		movl	%eax, -4(%rbp)          ## 4-byte Spill
		movl	%ecx, %eax
		addq	$16, %rsp
		popq	%rbp
		retq
		.cfi_endproc
		.section	__TEXT,__cstring,cstring_literals
	L_.str:                                 ## @.str
		.asciz	"Hello world!"


Cons of compilers
-----------------
* Compilation is slow
* It's a pain to wait around for the compiler
* Binaries are the opposite of human readable
* Binaries are system specific

Pros of compilers
-----------------
* Resulting binaries are wicked fast
* Optimization
* Compile time analysis (type checking, lifetime checking)

Let's trade speed for usability
-------------------------------
* Nobody wants to execute a mysterious binary
* Don't want to recompile for every platform under the sun

Source code
-----------

.. code-block:: python

	print 'hello world'

.. figure:: /_static/green_chinese_dragon.png
	:width: 40%
	:align: center

Abstract syntax tree
--------------------

* Parsing and lexing also happen, they look pretty similar
* Some interpreters walk the abstract syntax tree and executes the statements.
  This is slow

::

	Module(
		body=[
			Print(
				dest=None,
				values=[
					Str(s='hello world')
				],
				nl=True
			)
		]
	)

A byte code Virtual Machine
---------------------------

I lied, something does get emitted by Python. Python translates the script into
bytecode. This bytecode is interpreted in a Virtual Machine.
This isn't a VM which emulates hardware. This is more like a state machine
keeps track of the state of the program.

::

	?
	?ӫUc@s	dGHdS(s
				   hello worldN((((stest.py<module>s


How is bytecode different than assembly?
----------------------------------------

::

	$ python
	>>> import dis
	>>> def p(a):
	...    print "hello " + a + "!"
	... 
	>>> dis.dis(p)
				  0 LOAD_CONST               1 ('hello ')
				  3 LOAD_FAST                0 (a)
				  6 BINARY_ADD          
				  7 LOAD_CONST               2 ('!')
				 10 BINARY_ADD          
				 11 PRINT_ITEM          
				 12 PRINT_NEWLINE       
				 13 LOAD_CONST               0 (None)
				 16 RETURN_VALUE        


A closer comparison
-------------------

::

	  0 LOAD_CONST        1 ('hello ')
	  3 LOAD_FAST         0 (a)
	  6 BINARY_ADD   
	  7 LOAD_CONST        2 ('!')
	 10 BINARY_ADD   
	 11 PRINT_ITEM   
	 12 PRINT_NEWLINE
	 13 LOAD_CONST        0 (None)
	 16 RETURN_VALUE 

::

	   0:	push   %rbp            # push stack pointer
	   1:	mov    %rsp,%rbp
	   4:	lea    0x0(%rip),%rdi  # load memory contents into the register
	   b:	mov    $0x0,%eax
	  10:	callq  15 <_main+0x15> # call printf
	  15:	mov    $0x0,%eax       # prepare return code
	  1a:	pop    %rbp            # pop stack base pointer
	  1b:	retq                   # return



Pros of interpreters
--------------------

* Interpreted code will run anywhere the interpreter runs
* Dynamic languages where types and objects can be transformed
  at runtime are really hard to compile.

Cons of interpreters
--------------------

* Slow
* Programming errors are not encountered until runtime. Errors on
  rarely taken code paths may take some time to find.
* You have to rewrite the interpreter for code to run.

Questions? Feedback?
--------------------
*Thank you*

Ian Kronquist

iankronquist@gmail.com

.. figure:: /_static/black_baby_dragon.jpg
	:align: center
	:width: 45%
