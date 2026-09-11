# The Book

The course typeset as a book with LuaLaTeX: six chapters, one per lecture, then the solutions to
the exercises and the two self-assessment papers as appendices.

---

## Building it

```bash
sudo apt -y install make texlive-luatex texlive-latex-extra fonts-texgyre fonts-texgyre-math \
                    fonts-dejavu-core poppler-utils
make -C book                   # Writes book/modern-embedded-cpp.pdf, dated today.
make -C book VERSION=book-v2   # The same, with the version on the title page.
make -C book png               # Redraws the lecture's priority-inversion PNG from the figure.
make -C book clean             # Removes book/build/ and the PDF.
```

The build runs LuaLaTeX twice, so the contents and the cross-references settle, then prints any
overfull or underfull lines and LaTeX warnings it found, and fails if a reference is left undefined.
A clean build prints nothing after the two `lualatex` lines except a handful of mildly underfull
ones.

---

## Releasing a new edition

The PDF is not committed; it is published as a GitHub release. Push a tag named `book-v*`:

```bash
git tag book-v2
git push origin book-v2
```

The [Book workflow](../.github/workflows/book.yml) then builds the PDF with the tag on its title
page and attaches it to a release of the same name. The README's download link always points at
the newest release, so it never needs updating.

---

## What is where

```text
book.tex                The book: front matter, six chapters, appendices, in order.
cppbook.sty             Every visual decision: page, type, colours, code blocks, exercises.
cppbook.lua             How \code{...} typesets inline C++ (#, \n and line breaks).
front/                  Title pages and preface.
chapters/NN/            Chapter NN: chapter.tex (the opener), one file per appendix of lecture
                        LNN, summary.tex (the review) and exercises.tex.
back/solutions/         Appendix A, one file per chapter.
back/exam/              Appendices B to F: the papers and their model answers.
figures/                Figures drawn in TikZ, and png.tex, which renders one as the lecture's PNG.
```

Every `.tex` file typeset from course material starts with a comment naming its source, for
example:

```tex
% Section 1.2, from lectures/L01/appendix/b_from_c_to_cpp.md.
```

---

## Updating the content

The course material is the source of truth, and the book follows it. **Two kinds of content behave
differently:**

* **Code in the solutions and examples updates itself.** Appendix A and §3.3 do not contain code;
  they include the files under `lectures/` directly (`\cppfile{lectures/...}`), so a change to a
  solution is in the book on the next build, with nothing to edit here.
* **Prose and the code snippets in the text do not.** A chapter's text is a typeset copy of its
  lecture's markdown. When you change a lecture appendix, make the same change in the `.tex` file
  whose header names it. The same holds for `exam/*.md` and `back/exam/`.

A few conventions, so an edit reads like the rest of the book:

* Code blocks: `cppcode` (C++), `ccode` (C), `shell`, `makecode` (Makefiles; recipe lines keep their
  tab), and `console` (program output, directory trees, plain text).
* Inline code: `\code{...}`, written exactly as in the source. Inside it, write `\%` for `%`,
  `\{` or `\}` for an unbalanced brace, `\\` for a backslash, and `\#` for `#` in a heading or
  caption.
* References: `\secref{c2:app:b}` is the section typeset from appendix B of L02, and
  `Exercise~\ref{c4:ex:3.1}` is exercise 3.1 of L04 (the book renumbers exercises within each
  chapter, so always refer to them by label).
* Exercises: `\exerciseset{Title}` for an exercise set, `\exercise{Title}{Code}` or
  `\exercise[fillaccent2]{Title}{Reflection}` for one exercise, `\task{a) ...}` for a titled part.
* A new lecture appendix is a new file in `chapters/NN/`, `\input` from that chapter's
  `chapter.tex`; a new solution file is one more `\cppfile` line in `back/solutions/NN.tex`.

The priority-inversion figure in Chapter 6 is also the course's PNG. Edit
`figures/priority_inversion.tex`, then run `make -C book png` to redraw
`lectures/L06/appendix/images/priority_inversion.png`, so the lecture and the book stay the same
drawing.

---

## License

The book, its text and figures and the PDF built from them, is licensed under
[CC BY 4.0](./LICENSE). The code in it, and this directory's build files (`cppbook.sty`,
`cppbook.lua`, `Makefile`), are released under the repository's [MIT License](../LICENSE), like
all the code in the course.
