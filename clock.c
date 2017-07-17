/* Clock do simulador MIPS 32             */
/* Autor Bruno Cesar, bcesar.g6@gmail.com */

#include "clock.h"

unsigned int ciclo = 0;
unsigned int process = 0;

/* Roda um ciclo de clock */
unsigned int run(){
    process = pipeline(ciclo);
    ciclo++;
    if(get_flag(FLAG_STEP_BY_STEP)) pause();

    return process;
}
