# Create documentation

SVN_REVISION = $(shell svnversion -n)
	
doxy: Doxyfile
	rm -f html/*
	cp images/*.png html
	sed -e 's/\(^PROJECT_NUMBER.*$$\)/\1 " SVN $(SVN_REVISION)"/' Doxyfile | doxygen -
#	doxygen Doxyfile

clean: 
	rm -f html/*
