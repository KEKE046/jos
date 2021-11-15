LABS=lab1 lab2 lab3 lab4
PDFS=$(addsuffix /report.pdf,$(LABS))
$(info $(PDFS))

sync:
	git add . && git commit -am "update `date`" && git push

$(addsuffix /report.pdf,$(LABS)): %.pdf: %.md
	pandoc-md2pdf $@.md -o $@.pdf


handin-%: %/report.pdf
	cd .. && git archive $* -o note/$*/code.tar.gz
	tar cvf $*.tar.gz $*
	cp $*/report.pdf ./
	dolphin .

clean:
	rm -rf $(wildcard **/*.tar.gz) $(wildcard *.tar.gz)