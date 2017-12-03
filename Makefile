all:
	echo "Make arguments: build, gendocs, countlines"
	
build:
	gcc --help

gendocs:
	doxygen doxygen.cfg
	
countlines:
	find src -name '*.*' | xargs wc -l

	