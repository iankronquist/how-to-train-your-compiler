
.. How To Train Your Compiler: The Dragon Book, condensed slides file, created by
   hieroglyph-quickstart on Fri Apr 17 10:25:20 2015.

hack
====

How To Train Your Compiler
--------------------------
*The Dragon Book, condensed*

.. figure:: /_static/DragonMedium.png
	:align: center

What is a compiler or interpreter?
----------------------------------
* Almost all of you use compilers, every day.
* Even those of you who aren't programmers probably wind up using a special
  kind of compiler in your web browser.

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

Source code
-----------

.. code-block:: c

	#include <stdio.h>

	int main() {
		puts("Hello world!");
	}

.. figure:: /_static/three_headed.jpg
	:align: center
	:width: 30%

Machine code
------------

::

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
	00000a0 07 00 00 00 05 00 00 00 06 00 00 00 00 00 00 00
	00000b0 5f 5f 74 65 78 74 00 00 00 00 00 00 00 00 00 00
	00000c0 5f 5f 54 45 58 54 00 00 00 00 00 00 00 00 00 00
	00000d0 50 0f 00 00 01 00 00 00 21 00 00 00 00 00 00 00
	00000e0 50 0f 00 00 04 00 00 00 00 00 00 00 00 00 00 00
	00000f0 00 04 00 80 00 00 00 00 00 00 00 00 00 00 00 00
	0000100 5f 5f 73 74 75 62 73 00 00 00 00 00 00 00 00 00
	0000110 5f 5f 54 45 58 54 00 00 00 00 00 00 00 00 00 00
	0000120 72 0f 00 00 01 00 00 00 06 00 00 00 00 00 00 00
	0000130 72 0f 00 00 01 00 00 00 00 00 00 00 00 00 00 00
	0000140 08 04 00 80 00 00 00 00 06 00 00 00 00 00 00 00
	0000150 5f 5f 73 74 75 62 5f 68 65 6c 70 65 72 00 00 00
	0000160 5f 5f 54 45 58 54 00 00 00 00 00 00 00 00 00 00

What happened?
--------------

* Preprocessing.
* Compiler Front end:
	* Lexing/tokenizing.
	* Parsing.
	* Compilation to an intermediate representation.
	* Optimization.
* Compiler Back end:
	* Platform specific optimizations.
	* Compilation to target machine code (Mach-O 64-bit executable x86_64)
* Linking. Putting together multiple compiled files. We'll ignore this today.

Lexing
------

The code needs to be split into tokens.

::

	int a = 1 + 3;
	func(a,b,c);

Becomes something like:

::

	int, a, =, 1, +, 3, ;
	func, (, a, b, c, ), ;

Parsing and Grammars
--------------------
Programming languages are like natural languages. They follow a set of rules
called a grammar.

Parsing happens according to a grammar. Grammars need to specify what happens
in otherwise ambiguous situations.

::

	c = a+++++b
	c = ((a++)++)+b
	c = (a++)+(++b)
	c = a+(++(++b))

Sample grammar for Lua
----------------------

::

	var ::=  Name | prefixexp `[´ exp `]´ | prefixexp `.´ Name 

	varlist ::= var {`,´ var}

	statement ::=  varlist `=´ explist | 
		 functioncall | 
		 while expression do block end | 
		 function funcname funcbody

	binop ::= `+´ | `-´ | `*´ | `/´ | `^´ | `%´ | `..´ | 
		 `<´ | `<=´ | `>´ | `>=´ | `==´ | `~=´ | 
		 and | or

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



*... is worth two in the bush*

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


Compilation to Intermediate Representation
------------------------------------------

* Halfway between source code and machine code.
* Has detailed type information and annotations about what the
  source code means
* Also has machine details which can be used by the back end.
* IR is machine agnostic.

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

Optimization
------------

* Make the program go faster! Remove redundant work.
* Some optimizations are safe, others are dangerous and can affect how the
  program runs.
* Code is often transformed into single static assignment form, where variables
  are only used once, and transformations are assigned to a new variable. This
  makes code easier to reason about and optimize.

.. nextslide::

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

	Module:(
		body=[
			Expr(
				value=Attribute(
					value=Name(
						id='test', ctx=Load()
					),
					attr='py',
					ctx=Load()
				)
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

	G1Uc
	hello worldN(
	test.pyt
	<module>

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

Saved by the bell: Just In Time compilers
-----------------------------------------

* JITs combine the best parts of both worlds.
* JITs often have both compilers and interpreters. They are very complicated.
* JITs can be broken down into two groups, whole method JITs and Tracing JITs.

Bail out!
---------

When working on dynamic languages like Python or Javascript, sometimes an
object will change or an assumption the JIT made will become
invalid, and the JIT compiler will need to "bail out" to the interpreter.

Whole Method JITs
-----------------

* Chuck each method into a compiler and execute it.
* There is a large up-front cost.
* Requires frequent recompilation when the structure of objects change.

Examples:

* Google's V8 Javascript Engine
* The WebKit Javascript Engine
* Mozilla's IonMonkey Javascript Engine (with caveats)
* Rubinius for Ruby
* Pyston for Python


Tracing JITs
------------

* Interpret the code. If a loop is used a lot, stop, compile it and
  continue execution.
* Can compile a 'hot loop' without compiling the rest of the function.
* Need really smart heuristics to figure out how often the code will be run. It
  would suck to run a loop four times, stop and compile the loop, and
  then run it just one more time.

Examples:

* Mozilla's old TraceMonkey Javascript JIT
* LuaJIT
* PyPy for Python, and many other languages

Pros of JITs
------------

* The best way to make dynamic languages fast.
* Allow the same code to be executed across different systems.
* Allow some optimization of code.


Cons of JITs
------------

* High startup cost.
* Really complicated.
* Will never be as fast as compiled code.
* Can't do all optimizations - some are very time consuming.


Questions? Feedback?
--------------------
*Thank you*

.. figure:: /_static/black_baby_dragon.jpg
	:align: center
	:width: 50%
