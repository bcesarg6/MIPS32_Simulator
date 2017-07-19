/* Processador do simulador MIPS32         */
/* Autor: Bruno Cesar, bcesar.g6@gmail.com */

#include "processador.h"
#include <stdio.h>
#include <stdlib.h>

unsigned int pc = 0;
static word IR; // instruction Register
unsigned int ativ;

/* Etapa de busca                                              */
/* Nesta etapa do pipeline uma instrução, na forma de palavra, */
/* é buscada na memoria na posição do PC atual                 */
/* Ela é então copiada para o registrador de instrução IR      */
void busca(){
    read(IR, pc, 1);
    pc+=4; //Fazer componente PC+4
}

/* Etapa de decoficação                                       */
/* Decodifica uma palavra identificando seu tipo e atribuindo */
/* seus respectivos dados aos atributos da estrutura inst     */
/* Params = word w : A palavra a ser decodificada             */
/* Return = inst : A instrução decodificada e preenchida      */
void decodifica(){
    /* TODO Identificar Jump inconcidional na Decodificação, */
    /* chamar função auxiliar que altera PC e pular o restante das etapas do pipeline neste ciclo */
    inst instruction;
    unsigned int opcode = 0;
    unsigned int func = 0;

    word aux = malloc(sizeof(unsigned int));

    // Move pra trás e pra frente os bits para pegar o campo desejado... não tem jeito melhor?
    memcpy(aux, IR, 4);
    *aux = *aux >> 26;
    opcode = *aux;

    switch (opcode) {
        case 0:
        case 28:
            memcpy(aux, IR, 4);
            *aux = *aux << 26;
            *aux = *aux >> 26;
            func = *aux;

            //Instrução R //VERIFICAR SE PRECISA OU NÃO DAR FREE STEP-BY-STEP
            instruction.R.op = opcode;
            instruction.R.func = func;

            memcpy(aux, IR, 4);
            *aux = *aux << 6;
            *aux = *aux >> 27;
            instruction.R.rs = *aux;

            memcpy(aux, IR, 4);
            *aux = *aux << 11;
            *aux = *aux >> 27;
            instruction.R.rt = *aux;

            memcpy(aux, IR, 4);
            *aux = *aux << 16;
            *aux = *aux >> 27;
            instruction.R.rd = *aux;

            memcpy(aux, IR, 4);
            *aux = *aux << 21;
            *aux = *aux >> 27;
            instruction.R.aux = *aux;
            break;

        case 2:
        case 3:
            instruction.J.op = opcode;

            memcpy(aux, IR, 4);
            *aux = *aux << 6;
            *aux = *aux >> 6;
            instruction.J.target = *aux;

            break;

        case 1:
        case 4 ... 15:
        case 32 ... 38:
        case 40 ... 43:
        case 46:
        case 48:
        case 56:
            //Instrução I
            instruction.I.op = opcode;

            memcpy(aux, IR, 4);
            *aux = *aux << 6;
            *aux = *aux >> 27;
            instruction.I.rs = *aux;

            memcpy(aux, IR, 4);
            *aux = *aux << 11;
            *aux = *aux >> 27;
            instruction.I.rt = *aux;

            memcpy(aux, IR, 4);
            *aux = *aux << 16;
            *aux = *aux >> 16;
            instruction.I.imm = *aux;
            break;

        default:
            break;
    }

    //if(get_flag(FLAG_DEBUG)) printaInstrucao(instruction);

    insereFila(instruction);
    free(aux);
}

/* Função auxiliar, verifica se existe uma ER livre para a operação op             */
/* Params = int er_type : O tipo da Operação a ser verificada                      */
/* return = estacao_reserva* : O ponteiro para a ER ou NULL se não existe ER livre */
estacao_reserva* checa_er(int er_type){
    switch(er_type){
        case ADD_T:
            if (er_add1->busy == 0) return er_add1;
            if (er_add2->busy == 0) return er_add2;
            if (er_add3->busy == 0) return er_add3;
            break;

        case MUL_T:
            if (er_mult1->busy == 0) return er_mult1;
            if (er_mult2->busy == 0) return er_mult2;
            break;

        case LOAD:
        case STORE:
            if (er_load1->busy == 0) return er_load1;
            if (er_load2->busy == 0) return er_load2;
            break;

        default:
            break;
    }

    return NULL;

}

/* Etapa de emissão do pipeline                         */
/* Args = inst instruction : Instrução retirada da fila */
/* return : 1 Se a emissão ocorreu normalmente          */
/*          0 Se a emissão falhou (cria stall)          */
/*            e a instrução deve voltar para a fila     */
int emissao(inst instruction){

    /* Se existe uma ER do tipo de OP livre :                */
    /*     Se operandos estiverem nos registradores :        */
    /*         Emite op -> ER com operandos em Vj e Vk       */
    /*     Se não:                                           */
    /*           Emite op -> ER com valores de Qi em Qj e Qk */
    /* Se não:                                               */
    /*     Cria um stall (pula etapa de emissão)             */

    operation op = getOp(instruction);
    estacao_reserva* er = NULL;
    er = checa_er(op.er_type);

    if(er == NULL) return 0;

    /* Carrega a operação na ER */
    er->op = op.op;
    er->busy = op.cycles;

    switch(op.er_type){
        case ADD_T:
        case MUL_T:

            if(Qi[instruction.R.rs] != 0){
                er->qj = Qi[instruction.R.rs];
            } else{
                er->vj = regs[instruction.R.rs];
                er->qj = 0;
            }

            if(Qi[instruction.R.rt] != 0){
                er->qk = Qi[instruction.R.rt];
            } else{
                er->vk = regs[instruction.R.rt];
                er->qk = 0;
            }
            break;

        case LOAD:

            if(Qi[instruction.I.rs] != 0){
                er->qj = Qi[instruction.I.rs];
            } else{
                er->vj = regs[instruction.I.rs];
                er->qj = 0;
            }

            er->A = instruction.I.imm;

            Qi[instruction.I.rt] = identificaER(er);

            printf("\nChecando: Qi[rt] = %d (er_defined)\n", Qi[instruction.I.rt]);
            break;

        case STORE:
            if(Qi[instruction.I.rs] != 0){
                er->qj = Qi[instruction.I.rs];
            } else{
                er->vj = regs[instruction.I.rs];
                er->qj = 0;
            }

            er->A = instruction.I.imm;

            if(Qi[instruction.R.rt] != 0){
                er->qk = Qi[instruction.R.rt];
            } else{
                er->vk = regs[instruction.R.rt];
                er->qk = 0;
            }
            break;

    }

    if(get_flag(FLAG_DEBUG)) printaER(er, 0, NULL);
    return 1;
}

/* Ativa uma estação de reserva para processar sua operação        */
/* Params = estacao_reserva* er : Estação de reserva a ser ativada */
int ativaER_add(estacao_reserva* er){
    unsigned int identificador = identificaER(er) - 1;

    /* Checa se a operação deve ser realizada */
    if(er->busy == 1){
        if(er->qj == 0 && er->qk == 0){
            /* A ULA executára a operação */
            buffer_resultados[identificador] = (*p[er->op]) (2, er->vj, er->vk);
            printf("RESULTADO NA ativaER_add : %d\n", buffer_resultados[identificador]);
            er->busy = 0;

        } else {
            /* Operando não estão prontos */
            if(get_flag(FLAG_DEBUG)) printf("Operação na ER %s não pode ser executada:\n Operandos não estão prontos.", ER_nomes[identificador]);
        }


    } else{
        if(er->busy > 1){
            er->busy = er->busy - 1; //Representa um ciclo gasto na operação
            if(get_flag(FLAG_DEBUG)) printf("\n| ER %s -> Ciclos restantes = %d |\n", ER_nomes[identificador], er->busy );
        } else {
            return 0;
        }

    }

    return 1;
}


int execucao(){
    /* Chamar a execução de cada estacao de reserva */
    int ativ = 0;

    ativ += ativaER_add(er_add1);
    ativ += ativaER_add(er_add2);
    ativ += ativaER_add(er_add3);
    ativ += ativaER_add(er_mult1);
    ativ += ativaER_add(er_mult2);

    if(get_flag(FLAG_DEBUG)){
        if(ativ == 0){
            printf("Pulando etapa de execução (Estações de reserva vazias)\n");
        }
    }

    return ativ;
}


/* Função principal, simula o pipeline*/
unsigned int pipeline(int ciclo){ //pipeline reverso
    inst* ptr_instruction; //Criar uma especie de barramento pra isso?
    ptr_instruction = NULL;
    int verif = 0;
    ativ = 0;

    //if CDB != FLAG_NULL then Escrita();

    ativ = execucao();

    ptr_instruction = pegaDaFila(0);
    if(ptr_instruction != NULL){
        //printaInstrucao(*ptr_instruction);
        ativ = 1;
        verif = emissao(*ptr_instruction);

        /* Se a emissão ocorreu corretamente retira a instrução da fila */
        if (verif){
            pegaDaFila(1);
            verif = 0;
        } else{
            if(get_flag(FLAG_VERBOSE)) printf("Emissão falhou! Stall criado no pipeline\n");
        }

    } else {
        if(get_flag(FLAG_DEBUG)) printf("Pulando etapa de emissão (Fila vazia) \n");
    }

    if (*IR != FLAG_NULL){
        ativ = 1;
        decodifica();
    } else {
        if(get_flag(FLAG_DEBUG)) printf("Pulando etapa de decodicação (IR = NULL)\n");
    }

    if(get_flag(FLAG_DEBUG)) printf("\nPC = %d\n", pc);
    if (&mem[pc] < mem_text_end){
        ativ = 1;
        busca();
    } else {
        if(get_flag(FLAG_DEBUG)) printf("Pulando etapa de busca (PC = END_OF_TEXT)\n");
        *IR = FLAG_NULL;
    }

    if(get_flag(FLAG_DEBUG))        printaFila();

    if(get_flag(FLAG_VERBOSE))      printf("\n======== Fim do ciclo %d ========\n", ciclo);

    return ativ;
}

void processadorInit(){
    er_load1 = malloc(sizeof(estacao_reserva));
    er_load2 = malloc(sizeof(estacao_reserva));
    er_add1  = malloc(sizeof(estacao_reserva));
    er_add2  = malloc(sizeof(estacao_reserva));
    er_add3  = malloc(sizeof(estacao_reserva));
    er_mult1 = malloc(sizeof(estacao_reserva));
    er_mult2 = malloc(sizeof(estacao_reserva));

    er_load1->busy = 0;
    er_load2->busy = 0;
    er_add1->busy  = 0;
    er_add2->busy  = 0;
    er_add3->busy  = 0;
    er_mult1->busy = 0;
    er_mult2->busy = 0;

    fila = malloc(sizeof(fila_inst));
    fila->next = NULL;

    IR = malloc(sizeof(unsigned int));
    *IR = FLAG_NULL;

    Qi = calloc(NUM_REGS, sizeof(unsigned int));
    buffer_resultados = calloc(NUM_ER, sizeof(unsigned int));

}
