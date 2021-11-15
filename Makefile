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

PFX:=$(shell pwd)/handin/1900013008-周可行-

sync:
	git add . && git commit -am "update `date`" && git push

%/report.pdf: %/report.md
	cd $* && $(PANDOC) report.md -o report.pdf

handin-%: %/report.pdf
	cd .. && git archive $* -o $(PFX)code-$*.tar.gz
	tar cvf $(PFX)report-$*.tar.gz $*
	cp $*/report.pdf $(PFX)report-$*.pdf
	dolphin handin

clean:
	rm -rf $(wildcard **/*.tar.gz) $(wildcard *.tar.gz)