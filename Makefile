CC=gcc
EX=compiler

all: latex bison
	$(CC) latexp3c.tab.c -lfl -g -o $(EX)
	cat Documentation/Documentation.tex | ./$(EX) &>/dev/null 
	mv latexout Documentation/Documentation.tex
	mv latextoc Documentation/Documentation.toc.tex
	rm latexlog

bison: latexp3c.y
	bison -vd latexp3c.y
latex: latex.l
	flex latex.l
test: all
	./$(EX) < OurTests/latex.input.txt; printf "\n________\n" ;  cat latexout 

clean:
	rm -rf lex.yy.c *.tab.* a.out latexp3c.output latexlog latextoc latexout $(EX)
