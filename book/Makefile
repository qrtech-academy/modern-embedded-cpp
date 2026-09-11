# Build the book: two LuaLaTeX passes, the second for the contents and the references.
#
#   make                   build modern-embedded-cpp.pdf, dated today
#   make VERSION=book-v2   the same, with the release's version on the title page
#   make png               redraw the lecture's copy of the priority-inversion figure from figures/
#   make clean             remove everything the build writes
#
# The example program in §3.3 is typeset straight from its files under lectures/, so a change to
# one of those is a change to the book on the next build.

ROOT    := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
BOOK    := $(ROOT)/book
BUILD   := $(BOOK)/build
VERSION ?=
LATEX   := lualatex -interaction=nonstopmode -halt-on-error -file-line-error \
           -output-directory=$(BUILD)
# The book, with its version defined before anything is read; -jobname keeps the output book.pdf.
BOOKTEX := $(LATEX) -jobname=book '\def\bookversion{$(VERSION)}\input{book.tex}'

SOURCES := book.tex cppbook.sty cppbook.lua \
           $(wildcard front/*.tex chapters/*/*.tex back/*.tex back/*/*.tex figures/*.tex)
CODE    := $(shell cd $(ROOT) && git ls-files 'lectures/*.cpp' 'lectures/*.hpp' \
             'lectures/*Makefile' 'lectures/*makefile' | sed 's|^|$(ROOT)/|')

.PHONY: all png clean FORCE

all: modern-embedded-cpp.pdf

# The version the last build was made for. The file is rewritten only when VERSION changes, so a
# new version, or none after one, rebuilds the title page and nothing else does.
$(BUILD)/version: FORCE
	@mkdir -p $(BUILD)
	@echo '$(VERSION)' | cmp -s - $@ || echo '$(VERSION)' > $@

modern-embedded-cpp.pdf: $(SOURCES) $(CODE) $(BUILD)/version
	@mkdir -p $(BUILD)
	cd $(BOOK) && $(BOOKTEX) >/dev/null || (tail -40 $(BUILD)/book.log; exit 1)
	cd $(BOOK) && $(BOOKTEX) >/dev/null || (tail -40 $(BUILD)/book.log; exit 1)
	@grep -E "^(Overfull|Underfull)|Warning" $(BUILD)/book.log | sort | uniq -c | sort -rn | head -20 || true
	@! grep -q "There were undefined references" $(BUILD)/book.log || \
	    (echo "error: undefined references; see $(BUILD)/book.log"; exit 1)
	cp $(BUILD)/book.pdf $@

# L06 shows the same drawing as the book, as a PNG; this is the one command that makes it.
png:
	@mkdir -p $(BUILD)
	cd $(BOOK) && $(LATEX) figures/png.tex >/dev/null || (tail -40 $(BUILD)/png.log; exit 1)
	pdftoppm -r 200 -png -singlefile $(BUILD)/png.pdf \
	    $(ROOT)/lectures/L06/appendix/images/priority_inversion

clean:
	rm -rf $(BUILD) modern-embedded-cpp.pdf
