# Makefile for producing STklos documentation (not using autoconf stuff)
#
#           Author: Erick Gallesio [eg@essi.fr]
#    Creation date:  1-Sep-2004 16:19 (eg)
# Last file update:  3-Feb-2022 11:46 (eg)


ADOC=@ASCIIDOCTOR@

SOURCES  = biblio.adoc cond.adoc custom.adoc expr.adoc ffi.adoc      \
           intro.adoc libs.adoc licence.adoc match.adoc object.adoc  \
           progstruct.adoc regexp.adoc scmpkg.adoc slib.adoc         \
           srfi.adoc stdproc.adoc stklos.adoc threads.adoc
DOCDB    = ../DOCDB
SPP      = ../../src/stklos -q -I ../../lib -f ../../utils/stklos-pp.stk --

HTMLMAIN  = stklos-ref.html
HTMLMULTI = stklos-ref-multi.html
MULTI_AUX = FDL.html                                        \
            _bibliography.html                              \
            _customizations.html                            \
            _exceptions_and_conditions.html                 \
            _expressions.html                               \
            _foreign_function_interface.html                \
            _introduction.html                              \
            _pattern_matching.html                          \
            _preface.html                                   \
            _program_structure.html                         \
            _regular_expressions.html                       \
            _srfis.html                                     \
            _standard_procedures.html                       \
            _stklos_object_system.html                      \
            _the_scmpkg_package_system.html                 \
            _threads_mutexes_and_condition_variables.html   \
            _using_the_slib_package.html

PDF       = stklos-ref.pdf
IMG	      = images/hierarchy.png

.PHONY: single multi pdf

all-recursive: all

all: single

# ----------------------------------------------------------------------
# Various entries. Only the single HTML page is done automatically.
# Multiple HTML files and PDF file generations must be called by hand
# since they demand special asciidoctor modules

single : ../html/$(HTMLMAIN)
multi:   ../html/$(HTMLMULTI)
pdf:     ../pdf/$(PDF)
extradoc: multi pdf

# ----------------------------------------------------------------------
# Don't use pipes here, because if we have a Scheme error in the original
# document, it  will not be caught by make (set -o pipefile doesn't work
# everywhere)


# ----------------------------------------------------------------------
../html/$(HTMLMAIN): tmp-html.txt
	mkdir -p ../html/images
	cp -a $(IMG) ../html/images
	$(ADOC) -v -o $@ tmp-html.txt

# ----------------------------------------------------------------------
# For the Multiple file version of the manual, we need to install
# https://github.com/owenh000/asciidoctor-multipage
# The backreferences are alawas on the name of the source file.
# So during the build of this version of the manual we create a link on
# tmp-html.txt 
../html/$(HTMLMULTI): tmp-html.txt
	mkdir -p ../html/images
	cp -a $(IMG) ../html/images
	ln -s tmp-html.txt stklos-ref-multi.txt 2>/dev/stderr || true
	$(ADOC) -v -r asciidoctor-multipage -b multipage_html5 \
			-o $@ stklos-ref-multi.txt && \
		rm -f stklos-ref-multi.txt

# ----------------------------------------------------------------------
../pdf/$(PDF): tmp-pdf.txt
	mkdir -p ../pdf
	$(ADOC) -v -r asciidoctor-pdf -b pdf -o $@ tmp-pdf.txt

# ----------------------------------------------------------------------
tmp-html.txt: $(SOURCES) $(DOCDB)
	$(SPP) -D "doc-fmt: 'html" stklos.adoc > $@

tmp-pdf.txt: $(SOURCES) $(DOCDB)
	$(SPP) -D "doc-fmt: 'pdf" stklos.adoc > $@
# ----------------------------------------------------------------------
$(DOCDB):
	(cd ..; $(MAKE) docdb)

clean:
	rm -f  tmp-html.txt tmp-pdf.txt stklos-ref-multi.txt *~

distclean: clean
	rm -f  ../html/$(HTMLMAIN) ../html/$(HTMLMULTI)
	for i in $(MULTI_AUX); do rm -f ../html/$$i ; done
	rm -f ../html/$(IMG)
	rm -f ../pdf/$(PDF)