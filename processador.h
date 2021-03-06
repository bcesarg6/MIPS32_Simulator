/* Header Processador                      */
/* Autor: Bruno Cesar, bcesar.g6@gmail.com */

#ifndef __PROCESSADOR_H
#define __PROCESSADOR_H

#define ADD_T  1 //ADD já é o nome de uma OP
#define MUL_T  2 //MUL já é o nome de uma OP
#define BRANCH 3
#define LOAD   4
#define STORE  5
#define ADDI_T   6
#define MULTI_T  7

#define ADD_1  1
#define ADD_2  2
#define ADD_3  3
#define MUL_1  4
#define MUL_2  5
#define LOAD_1 6
#define LOAD_2 7
#define LOAD_3 8
#define LOAD_4 9
#define LOAD_5 10
#define STORE_1 11
#define STORE_2 12
#define STORE_3 13
#define STORE_4 14
#define STORE_5 15

#define NUM_ER 15

#define FLAG_BUFFER_VAZIO -1

typedef union Inst inst;
typedef struct node_ti* fila_inst;
typedef struct er_t estacao_reserva;

#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"
#include "simulador.h"
#include "utils.h"
#include "registradores.h"
#include "ula.h"
#include "cache.h"
#include "pcComponent.h"

extern int jump;

/* Tipo de instruções Declaradas com bit-fields */
typedef struct{
    unsigned int op   : 6;
    unsigned int rs   : 5;
    unsigned int rt   : 5;
    unsigned int rd   : 5;
    unsigned int aux  : 5;
    unsigned int func : 6;
} inst_R;

typedef struct{
    unsigned int op  : 6;
    unsigned int rs  : 5;
    unsigned int rt  : 5;
    unsigned int imm : 16;
} inst_I;


typedef struct{
    unsigned int op     : 6;
    unsigned int target : 26;
} inst_J;

/* União de diferentes tipos de instrução, generaliza a instrução */
union Inst {
    inst_R R;
    inst_I I;
    inst_J J;
};

/* Estações de reserva */
struct er_t{
    unsigned int busy; //Usado também para contar a qtd de ciclos de cada Operação
    unsigned int op;
             int vj;
             int vk;
    unsigned int qj;
    unsigned int qk;
    unsigned int A;
};

extern estacao_reserva* er_add1;
extern estacao_reserva* er_add2;
extern estacao_reserva* er_add3;
extern estacao_reserva* er_mult1;
extern estacao_reserva* er_mult2;
extern estacao_reserva* er_load1;
extern estacao_reserva* er_load2;
extern estacao_reserva* er_load3;
extern estacao_reserva* er_load4;
extern estacao_reserva* er_load5;
extern estacao_reserva* er_store1;
extern estacao_reserva* er_store2;
extern estacao_reserva* er_store3;
extern estacao_reserva* er_store4;
extern estacao_reserva* er_store5;

unsigned int* Qi;

typedef struct{
    int dado;
    char validade;
} ula_output;

ula_output* buffer_resultados;
int* last_op;

typedef struct {
    int dado;
    unsigned int er_id;
} barr_aux;


extern barr_aux* barr_Lo;
extern barr_aux* barr_Hi;

struct node_ti{
    inst instruction;
    fila_inst next;
} node_ti;

fila_inst fila;

unsigned int pipeline(int ciclo);
void processadorInit();
estacao_reserva* checa_er(int er_type);
int emissao(inst instruction);
void decodifica();
void busca();

#endif /* __PROCESSADOR_H */
