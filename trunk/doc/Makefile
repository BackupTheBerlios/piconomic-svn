# Create documentation

SVN_REVISION = $(shell svnversion -n)
	
doxy: Doxyfile
	rm -f html/*
	cp images/*.png html
	sed -e "s/\(^PROJECT_NUMBER.*$$\)/\1.$(SVN_REVISION)/" Doxyfile | doxygen -
#	doxygen Doxyfile
	mv -f html/piconomic_fwlib.chm ..

clean: 
	rm -f html/*
	rm -f ../piconomic_fwlib.chm
