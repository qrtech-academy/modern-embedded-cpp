# Build the book: two LuaLaTeX passes, the second for the contents and the references.
#
#   make          build modern-embedded-cpp.pdf
#   make png      redraw the lecture's copy of the priority-inversion figure from figures/
#   make clean    remove everything the build writes
#
# The solutions and the examples are typeset straight from their files under lectures/, so a
# change to one of those is a change to the book on the next build.

ROOT    := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
BOOK    := $(ROOT)/book
BUILD   := $(BOOK)/build
LATEX   := lualatex -interaction=nonstopmode -halt-on-error -file-line-error \
           -output-directory=$(BUILD)

SOURCES := book.tex cppbook.sty cppbook.lua \
           $(wildcard front/*.tex chapters/*/*.tex back/*.tex back/*/*.tex figures/*.tex)
CODE    := $(shell cd $(ROOT) && git ls-files 'lectures/*.cpp' 'lectures/*.hpp' \
             'lectures/*Makefile' 'lectures/*makefile' | sed 's|^|$(ROOT)/|')

.PHONY: all png clean

all: modern-embedded-cpp.pdf

modern-embedded-cpp.pdf: $(SOURCES) $(CODE)
	@mkdir -p $(BUILD)
	cd $(BOOK) && $(LATEX) book.tex >/dev/null || (tail -40 $(BUILD)/book.log; exit 1)
	cd $(BOOK) && $(LATEX) book.tex >/dev/null || (tail -40 $(BUILD)/book.log; exit 1)
	cp $(BUILD)/book.pdf $@
	@grep -E "^(Overfull|Underfull)|Warning" $(BUILD)/book.log | sort | uniq -c | sort -rn | head -20 || true

# L06 shows the same drawing as the book, as a PNG; this is the one command that makes it.
png:
	@mkdir -p $(BUILD)
	cd $(BOOK) && $(LATEX) figures/png.tex >/dev/null || (tail -40 $(BUILD)/png.log; exit 1)
	pdftoppm -r 200 -png -singlefile $(BUILD)/png.pdf \
	    $(ROOT)/lectures/L06/appendix/images/priority_inversion

clean:
	rm -rf $(BUILD) modern-embedded-cpp.pdf
