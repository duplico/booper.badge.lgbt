.PHONY = all_pdfs clean
.DEFAULT_GOAL = badge_docs_combined.pdf

all_pdfs:
	for file in $(shell ls *.drawio.svg); do drawio --embed-diagram --export --format pdf $$file; done

regionals_docs_combined.pdf: all_pdfs
	pdfunite *.drawio.pdf regionals_docs_combined.pdf

clean:
	rm *.drawio.pdf regionals_docs_combined.pdf
