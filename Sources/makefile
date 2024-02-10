CC = gcc
OBJ = Calculs.o Initialisation.o Util.o listeSC.o main.o 
DEBUG = 

ifeq (yes,${DEBUG})
CC_OPTIONS = -g -Wall
else
CC_OPTIONS =
endif


Install : main.out
	@echo "${CC} ${CC_OPTIONS}" 

main.out : ${OBJ}
	${CC} ${CC_OPTIONS} $^ -o $@

main.o : main.c
	${CC} ${CC_OPTIONS} -c $< -o $@

listeSC.o : listeSC.c
	${CC} ${CC_OPTIONS} -c $< -o $@

Initialisation.o : Initialisation.c
	${CC} ${CC_OPTIONS} -c $< -o $@

Calculs.o : Calculs.c
	${CC} ${CC_OPTIONS} -c $< -o $@

Util.o : Util.c
	${CC} ${CC_OPTIONS} -c $< -o $@


clean :
	rm ${OBJ}
	rm main.out

cleandb:
	rm -r data

cleanall: clean cleandb