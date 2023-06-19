PANDOC=pandoc

.SUFFIXES: .md .html .md .pdf

HTMLFILES=schemer.html
PDFFILES=schemer.pdf

all: $(HTMLFILES) $(PDFFILES)

.md.pdf:
	$(PANDOC) -f gfm -t pdf \
		--toc --standalone --metadata title="$<" \
		-o $@ $<
.md.html:
	$(PANDOC) -f gfm -t html \
		--toc --embed-resources --standalone --css=./style.css \
		-o $@ $<
clean:
	rm -f *.html *.pdf
