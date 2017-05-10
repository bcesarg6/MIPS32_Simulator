/* Define a estrutura de arvore de instrucoes usada no tradutor */
/* Bruno, bcesar.g6@gmail.com */

#ifndef __INSTRUCOES_H
#define __INSTRUCOES_H

FILE* output;

#define DATA_OFFSET 400
#define INST_SIZE 32

extern int lbl_count;
char** lbl_names;
int* lbl_values;
extern int lbl_tam;

extern int var_count;
char** var_names;
int* var_adress;
int* var_values;
extern int var_tam; //var_tamanho inicial do vetor de variaveis

extern int line;
typedef struct node_t* node;

struct node_t{
    int tipo;
    int op;
    int rs;
    int rt;
    int rd;
    int func;
    int aux; //shift amount | imm | address | offset
    char* label;
    node prox;
};

extern node arvore;

void checkSizes(void);

#endif