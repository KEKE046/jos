sync:
	git add . && git commit -am "update `date`" && git push


handin-%:
	cd .. && git archive $* -o note/$*/code.tar.gz

clean:
	rm -rf $(wildcard **/*.tar.gz)