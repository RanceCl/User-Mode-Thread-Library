
comp = gcc
comp_flags = -g -Wall 
comp_libs = -lm  

trc : test_region_counts.o region.o
	$(comp) $(comp_flags) test_region_counts.o region.o -pthread -o trc $(comp_libs)

trma : test_region_malloc.o region.o
	$(comp) $(comp_flags) test_region_malloc.o region.o -pthread -o trma $(comp_libs)

trmu : test_region_multiplex.o region.o
	$(comp) $(comp_flags) test_region_multiplex.o region.o -pthread -o trmu $(comp_libs)

trp : test_region_prodcon.o region.o
	$(comp) $(comp_flags) test_region_prodcon.o region.o -pthread -o trp $(comp_libs)

tre : test_region_errors.o region.o
	$(comp) $(comp_flags) test_region_errors.o region.o -pthread -o tre $(comp_libs)


test_region_counts.o : test_region_counts.c region.h
	$(comp) $(comp_flags) -c test_region_counts.c

test_region_malloc.o : test_region_malloc.c region.h
	$(comp) $(comp_flags) -c test_region_malloc.c

test_region_multiplex.o : test_region_multiplex.c region.h
	$(comp) $(comp_flags) -c test_region_multiplex.c

test_region_prodcon.o : test_region_prodcon.c region.h
	$(comp) $(comp_flags) -c test_region_prodcon.c

test_region_errors.o : test_region_errors.c region.h
	$(comp) $(comp_flags) -c test_region_errors.c

region.o : region.c region.h
	$(comp) $(comp_flags) -c region.c

clean :
	rm -f *.o trc trma trmu trp tre core


