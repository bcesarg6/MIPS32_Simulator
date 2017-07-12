/* Header utlils */
/* Autor: Bruno Cesar, bcesar.g6@gmail.com */

#ifndef __UTILS_H
#define __UTILS_H

#include "simulador.h"
#include "memoria.h"
#include "processador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLAG_NULL 0

void pause();
void launchError(int e);
void ajuda();
void readProgram();
void printMem();
void printaBinario(word w, char isFile, FILE* dest);
void printaInstrucao(inst instruction);

void insereFila(inst instruction);
inst* removeFila();
void printaFila();

#endif
