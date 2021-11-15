
pdf: $(subst .md,.pdf,$(wildcard */*.md))

PANDOC:=pandoc \
	--standalone \
	--from=gfm+pipe_tables \
	--to=pdf \
	-V geometry:margin=1in \
	--shift-heading-level-by=-1 \
	--table-of-contents \
	--listings \
	--template eisvogel \
	-V titlepage:true \
	-V code-block-font-size:\\tiny

%.pdf: %.md
	cd $(shell dirname $*) && $(PANDOC) $(shell basename $*).md -o $(shell basename $*).pdf

sync: 
	git add . && git commit -am "update `date`" && git push

PFX:=$(shell pwd)/handin/1900013008-周可行-

handin-%: %/report.pdf
	cd .. && git archive $* -o $(PFX)$*-code.tar.gz
	cp $*/report.pdf $(PFX)$*-report.pdf
	tar cvf $(PFX)$*-report.tar.gz $*
	dolphin handin; true

clean:
	rm -rf handin/*

realclean:
	rm -rf */*.pdf