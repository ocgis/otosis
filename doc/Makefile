#
# oTOSis -- doc subdir makefile
#

all: OSis.info OSis.dvi

OSis.info: OSis.texi
	makeinfo $< -o $@

OSis.dvi: OSis.texi
	texi2dvi $<

dep depend .depend:

clean:
	rm -f OSis.log OSis.aux OSis.toc OSis.?? OSis.??s

reallyclean: clean
	rm -f OSis.info osis.info OSis.dvi

