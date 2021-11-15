LABS=lab1 lab2 lab3 lab4
PDFS=$(addsuffix /report.pdf,$(LABS))

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

sync:
	git add . && git commit -am "update `date`" && git push

%.pdf: %.md
	$(PANDOC) $@.md -o $@.pdf

handin-%: %/report.pdf
	cd .. && git archive $* -o note/$*/code.tar.gz
	tar cvf $*.tar.gz $*
	cp $*/report.pdf ./
	dolphin .

clean:
	rm -rf $(wildcard **/*.tar.gz) $(wildcard *.tar.gz)