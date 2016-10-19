CC     	= g++
EXEC   	= erathosten trialDivision millerRabin RSA_init
CFLAGS 	= -Wall -Werror -g -L/gmp_install/lib -lgmp
all   	: $(EXEC)










#-----------------------------------------------------------------------------------------------------------
# ----------------------------------------------- Executable -----------------------------------------------
#-----------------------------------------------------------------------------------------------------------
erathosten		: largePrime_test.o largePrime.o largePrime_naive.o list.o
	$(CC) $(CFLAGS) -DERATHOSTEN -c -o largePrime_test.o largePrime_test.cc
	$(CC) $(CFLAGS) -o $@ $^

trialDivision	: largePrime_test.o largePrime.o largePrime_naive.o list.o
	$(CC) $(CFLAGS) -DTRIAL_DIVISION -c -o largePrime_test.o largePrime_test.cc
	$(CC) $(CFLAGS) -o $@ $^

millerRabin		: largePrime_test.o largePrime.o largePrime_millerRabin.o list.o
	$(CC) $(CFLAGS) -DMILLER_RABIN -c -o largePrime_test.o largePrime_test.cc
	$(CC) $(CFLAGS) -o $@ $^

RSA_init		: RSA_init.o largePrime.o largePrime_millerRabin.o list.o
	$(CC) $(CFLAGS) -o $@ $^


#-----------------------------------------------------------------------------------------------------------
# ----------------------------------------------- Modules  -------------------------------------------------
#-----------------------------------------------------------------------------------------------------------
%.o 		: %.cc
		$(CC) $(CFLAGS) -c -o $@ $^

#-----------------------------------------------------------------------------------------------------------
# ------------------------------------------ General Methods -----------------------------------------------
#-----------------------------------------------------------------------------------------------------------
.PHONY	: clean mrproper
clean:
	rm -f *.o $(EXEC)
mrproper: clean
	rm -rf $(EXEC)
