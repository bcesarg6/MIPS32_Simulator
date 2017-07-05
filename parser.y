/* Bison: tradutor (Simulador MIPS32) */
/* Autor: Bruno Cesar, bcesar.g6@gmail.com */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simulador.h"
#include "parser.h"
#include "utils.h"
#include "tradutor.h"

extern FILE *yyin;

void set_input();
%}

/* Declaração dos tokens */
%token reg
/* 0 a 31 respectivamente, bison gera com +258, logo $zero = 258 */
%token number virg EOL END_OF_FILE
%token opcoder opcoders opcoderd opcodel opcodei opcodelui
%token opcodeb opcodej opcodejr opcodet opcodemf opcodemt
%token abreparents fechaparents colon
%token data text id

%code requires{
	typedef struct{
  		unsigned int code;
		unsigned int rs;
		unsigned int rt;
		unsigned int rd;
		unsigned int aux; //shift amount | imm | address | offset
  		unsigned int func;
		char* label;
	} strcinst; //struct instrucao
}

%defines "parser.h"

%union{
	int val;
	strcinst op;
	char* text;
}

%%
saida: secao_data secao_text END_OF_FILE { return 0;}
| secao_data EOL secao_text END_OF_FILE{ return 0;}
| secao_text END_OF_FILE {fprintf(log_t_file, "\nsem secao de dados\n"); return 0;}
;

secao_data: data EOL var
| data
;

secao_text: text EOL instrucao
;

instrucao: linha EOL instrucao {}
| labelid linha EOL instrucao {}
| /*Vazio*/
;

linha: operacao {line++;}

operacao: /* nada */
| R {fprintf(log_t_file,"Op tipo R: %d %d %d %d %d %d\n",
 						  		$<op.code>1, $<op.rs>1, $<op.rt>1,
						  		$<op.rd>1, $<op.aux>1, $<op.func>1);

    node n = malloc(sizeof(node_t));
	n->tipo = 1;
	n->op   = $<op.code>1;
 	n->rs   = $<op.rs>1;
	n->rt   = $<op.rt>1;
    n->rd   = $<op.rd>1;
    n->aux  = $<op.aux>1;
	n->func = $<op.func>1;
    insereLista(n); }

| RD {fprintf(log_t_file,"Op tipo RD: %d %d %d %d %d\n",
 								  $<op.code>1, $<op.rs>1, $<op.rt>1,
							  	  $<op.aux>1, $<op.func>1);

				node n = malloc(sizeof(node_t));
				n->tipo = 1;
				n->op   = $<op.code>1;
				n->rs   = $<op.rs>1;
				n->rt   = $<op.rt>1;
				n->rd   = 0;
				n->aux  = 0;
				n->func = $<op.func>1;
				insereLista(n);}

| I { if ($<op.label>1 == NULL){
		fprintf(log_t_file,"Op tipo I: %d %d %d %d\n",
	  						        $<op.code>1, $<op.rs>1, $<op.rt>1,
								    $<op.aux>1);

		node n = malloc(sizeof(node_t));
		n->tipo = 2;
		n->op   = $<op.code>1;
		n->rs   = $<op.rs>1;
		n->rt   = $<op.rt>1;
		n->aux  = $<op.aux>1;
		insereLista(n);}
	else {
		fprintf(log_t_file, "Op de Load: %d %d %s\n", $<op.code>1, $<op.rt>1,
		 										   $<op.label>1);
		node n 	 = malloc(sizeof(node_t));
		n->tipo  = 8;
		n->label = $<op.label>1;
 		n->rt    = $<op.rt>1;
		insereLista(n);}
}

| B {fprintf(log_t_file,"Op tipo B: %d %d %d %s\n",
 								  $<op.code>1, $<op.rs>1, $<op.rt>1, $<op.label>1);

				node n = malloc(sizeof(node_t));
  				n->tipo = 8;
  				n->op   = $<op.code>1;
  				n->rs   = $<op.rs>1;
				n->rt  = $<op.rt>1;
				n->label = $<op.label>1;
  				insereLista(n);}

| J { if ($<op.label>1 != NULL){
		fprintf(log_t_file,"Op tipo J: %d %s\n", $<op.code>1, $<op.label>1);

		node n = malloc(sizeof(node_t));
		n->tipo  = 4;
		n->op    = $<op.code>1;
		n->label = $<op.label>1;
		insereLista(n);}

	  else {
		fprintf(log_t_file,"Op tipo J: %d %d\n", $<op.code>1, $<op.aux>1);

		node n = malloc(sizeof(node_t));
		n->tipo = 4;
		n->op   = $<op.code>1;
		n->aux  = $<op.aux>1;
		insereLista(n);}
}

| JR {fprintf(log_t_file,"Op tipo JR: %d %d %d %d\n",
 								  $<op.code>1, $<op.rs>1,
								  $<op.aux>1, $<op.func>1);}

| T {fprintf(log_t_file, "Op tipo T: %d %d %d %d %d\n", $<op.code>1,
 						       $<op.rs>1,$<op.rt>1, $<op.aux>1, $<op.func>1);}

| M {fprintf(log_t_file, "Op tipo M: %d %d %d %d %d\n",
 								  $<op.code>1, $<op.rs>1, $<op.rt>1, $<op.rd>1,
							      $<op.func>1);

			  node n = malloc(sizeof(node_t));
			  n->tipo = 7;
			  n->op   = $<op.code>1;
			  n->rs   = $<op.rs>1;
			  n->rt   = $<op.rt>1;
			  n->rd   = $<op.rd>1;
			  n->func = $<op.func>1;
			  insereLista(n);}

| MT {fprintf(log_t_file, "Op tipo MT: %d %d %d %d\n",
 						  	       $<op.code>1, $<op.rs>1,
						           $<op.rd>1, $<op.func>1);

			    node n = malloc(sizeof(node_t));
   				n->tipo = 6;
   				n->op   = $<op.code>1;
   				n->rs   = $<op.rs>1;
   				n->aux  = $<op.aux>1;
   				n->func = $<op.func>1;
   				insereLista(n);}

| MF {fprintf(log_t_file, "Op tipo MF: %d %d %d %d %d\n",
 						           $<op.code>1, $<op.rs>1, $<op.rd>1,
								   $<op.aux>1, $<op.func>1);

				node n = malloc(sizeof(node_t));
				n->tipo = 5;
				n->op   = $<op.code>1;
				n->rs   = $<op.rs>1;
				n->rd   = $<op.rd>1;
				n->aux  = $<op.aux>1;
				n->func = $<op.func>1;
				insereLista(n);}

| MS {fprintf(log_t_file, "Op tipo M Especial: %d %d %d %d %d %d %d\n",
 						  $<op.code>1, $<op.rs>1, $<op.aux>1, $<op.func>1,
					   	  $<op.rd>1, 0, 1);}
;


R: opcoder reg virg reg virg reg {
	$<op.rs>$  = $<val>2;
	$<op.rt>$  = $<val>4;
	$<op.rd>$  = $<val>6;
	$<op.aux>$ = 0;}

| opcoder reg virg reg {
	$<op.rs>$ = $<val>4;
	$<op.rt>$ = 0;
	$<op.rd>$ = $<val>2;
	$<op.aux>$ = 0;}

|  opcoders reg virg reg virg reg {
	$<op.rd>$  = $<val>2;
	$<op.rt>$  = $<val>4;
	$<op.rs>$  = $<val>6;
	$<op.aux>$ = 0;}

|  opcoders reg virg reg virg shamt {
	$<op.rd>$    = $<val>2;
	$<op.rt>$    = $<val>4;
	$<op.aux>$   = $<val>6; //shift amount
	$<op.rs>$    = 0;}

| opcodej reg virg reg {
	$<op.rs>$   = $<val>2;
	$<op.rt>$   = 0;
	$<op.rd>$   = $<val>4;
	$<op.aux>$  = 0;}
;

RD: opcoderd reg virg reg {
	$<op.rs>$ = $<val>2;
	$<op.rt>$ = $<val>4;
	$<op.aux>$ = 0;}
;

I: opcodei reg virg reg virg imm{
	$<op.rs>$  = $<val>2;
	$<op.rt>$  = $<val>4;
	$<op.aux>$ = $<val>6;} //imm

|  opcodelui reg virg imm{
	$<op.rs>$  = 0;
    $<op.rt>$  = $<val>2;
	$<op.aux>$ = $<val>4;} //imm

|  opcodel reg virg offset abreparents reg fechaparents {
	$<op.rt>$  = $<val>2;
	$<op.aux>$ = $<val>4; //offset
	$<op.rs>$  = $<val>6;}

|  opcodel reg virg id {
	$<op.rt>$  = $<val>2;
	$<op.label>$ = $<text>4; }

|  opcodet reg virg imm{
	$<op.rs>$  = $<val>2;
	$<op.rt>$  = $<op.func>1;
	$<op.aux>$ = $<val>4;
}
;

B: opcodeb reg virg labelid{
	$<op.rs>$ = $<val>2;
	$<op.rt>$ = $<op.func>1;
	$<op.label>$ = $<text>4;}

| opcodeb reg virg reg virg labelid{
	$<op.rs>$ = $<val>2;
	$<op.rt>$ = $<val>4;
	$<op.label>$ = $<text>6;}
;

J: opcodej address{ $<op.aux>$ = $<val>2; }
|  opcodej id { $<op.label>$ = $<text>2; }
;

JR: opcodejr reg{
	$<op.rs>$ = $<val>2;
	$<op.aux>$ = 0;}
;

T: opcodet reg virg reg {
	$<op.rs>$  = $<val>2;
	$<op.rt>$  = $<val>4;
	$<op.aux>$ = 0;}
;

M: opcodemf reg virg reg {
	$<op.rs>$ = 0;
	$<op.rt>$ = $<val>2;
	$<op.rd>$ = $<val>4;}

| opcodemt reg virg reg {
	$<op.rs>$ = 4;
	$<op.rt>$ = $<val>2;
	$<op.rd>$ = $<val>4;}

| opcodemf reg virg reg virg reg{ //movn & movz
	$<op.rd>$ = $<val>2;
	$<op.rs>$ = $<val>4;
	$<op.rt>$ = $<val>6;}
;

MF: opcodemf reg{
	$<op.rs>$ = 0;
	$<op.rd>$ = $<val>2;
	$<op.aux>$ = 0;}
;

MT: opcodemt reg{
	$<op.rs>$ = $<val>2;
	$<op.aux>$ = 0;}
;

MS: opcodemf reg virg reg virg cc{ //movf & movt
	$<op.rd>$  = $<val>2;
	$<op.rs>$  = $<val>4;
	$<op.aux>$  = $<val>6;}
;

labelid: id colon { checkSizes();
			if ( labelMatch($<text>1) == -1){
				  lbl_names[lbl_count] = $<text>1;
				  lbl_values[lbl_count] = DATA_OFFSET + line * INST_SIZE;
		    	  lbl_count++; line++;
			 } else { launchError(1);}
		 }

| id EOL colon { checkSizes();
		if ( labelMatch($<text>1) == -1){
				 lbl_names[lbl_count] = $<text>1;
		   		 lbl_values[lbl_count] = DATA_OFFSET + line * INST_SIZE;
		   		 lbl_count++; line++;
				 } else { launchError(1);}
			 }
;

var: id colon number EOL var { checkSizes();
							   var_names[var_count] = $<text>1;
							   var_adress[var_count] = var_count * INST_SIZE;
							   var_values[var_count] = $<val>3;
				    		   var_count++;}

| id colon number EOL { checkSizes();
							var_names[var_count] = $<text>1;
							var_adress[var_count] = var_count * INST_SIZE;
							var_values[var_count] = $<val>3;
							var_count++;}
;

address: number;
shamt: number;
offset: number;
imm: number;
cc: number;
%%

void set_input(){
	yyin = input_file;
}

yyerror(char *s) {
	fprintf(stderr, "erro na linha %d: %s\n", line ,s);
	fprintf(log_t_file, "erro na linha %d: %s\n", line ,s);
}
