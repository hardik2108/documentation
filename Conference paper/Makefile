
.PHONY: all
all:
	#doing this twice so we get all references and links correctly
	pdflatex report.tex
	biber report
	pdflatex report.tex

.PHONY: all
clean:
	/bin/rm -rf *.aux *.lof *.lol *.lot *~ *.sync* *.bbl *.blg *.toc *.brf *.log *.out *.nlo *.idx *.nls *.bcf *.run.xml
