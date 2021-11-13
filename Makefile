sync:
	git add . && git commit -am "update `date`" && git push

handin-%:
	cd .. && git archive $* -o note/$*/code.tar.gz
	typora $*/report.md
	tar cvf $*.tar.gz $*
	cp $*/report.pdf ./
	dolphin .

clean:
	rm -rf $(wildcard **/*.tar.gz)