#
#  Makefile.in 	-- Makefile for Curl Extension
#
# Copyright © 2021-2023 Erick Gallesio <eg@stklos.net>
#
#           Author: Erick Gallesio [eg@unice.fr]
#    Creation date: 18-Jun-2021 08:18
#

SUBDIRS=lib/stklos demos

clean-am:
	rm -f *~

distclean-am: clean-am
	(cd demos && make distclean)
	rm -f Makefile
