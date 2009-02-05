# Create documentation

doxy: Doxyfile
	rm -f html/*
	cp images/*.png html
	doxygen Doxyfile
	mv -f html/piconomic_fwlib.chm ..

clean: 
	rm -f html/*
	rm -f ../piconomic_fwlib.chm
