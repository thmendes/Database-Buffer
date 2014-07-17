/*
Executado no Ubuntu 13.04
Compilado com gcc version 4.7.3

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define BP 512
#define SIZE 1024
#define CAMINHO "files/data/" // aqui vai inserido o caminho até a pasta onde estao os arquivos de dados
#define EXTENSAO ".dat" // extensao dos arquivos de dados
#define DICIONARIO "files/dicionario.dat"
#define METADADOS "files/fs_coluna.dat"

struct CAMPOS
 {
   unsigned int id;  //identificador
   char nome[40];
   char tipo;
   int tamanho;
};

struct page {
   unsigned int nrec;            // numero de registros
   char data[SIZE];              //armazenamento das tuplas da página
   unsigned int pc;              // pin_count -> contador de uso da página
   unsigned int db;              // dirty_bit -> se dados foram modificados
   unsigned int position;        // posição livre da página
};

struct OBJ {
	unsigned int id; //identificador
	char lnome[30]; //nome lógico
	char fnome[30]; //nome físico
	char dir[30];  //diretório
};

typedef struct _tupla {
	char tipo;
	double vDouble;
	int vInt;
	char *string;
}TpTupla;

typedef struct _nodo {
	char *tupla;
	struct _nodo * next;
}TpNodo;

typedef struct _cabLista {
	int nNodo;
	TpNodo * first;
	TpNodo * last;
}TpLista;


//FUNÇÕES DISPONIVEL PARA USO
int leTabela(struct OBJ **table,char *Table_name); //Identifica a estrutura da tabela.
int leMetaDados(struct CAMPOS **campos, struct OBJ *table); //Identifica a estrutura do metadados.
int inicializaBuffer(struct page **buffer); //Valores default para cada pagina.
int carregaDados(struct page *buffer,struct CAMPOS *campos, struct OBJ *tabela, int reg); // Carrega dados para o buffer - tupla por tupla, onde reg é o numero da tupla na tabela.
int cardinalidade(char *tabela); // retorna a quantidade de tuplas da tabela representada pela variavel *tabela.
int createTable(char *novaTabela,int campos, ...); // Criar uma nova tabela com nome representado pela variavel *novaTabela. Necessário informar quantia de campos e em sequencia informar nome do campo,tipo e tamanho todos separados por vírgula.
int insertTable(char *tabela, ...); // Inserir dados em uma Tabela - vai inserir uma tupla na tabela representada pela variavel tabela. No local dos 3 pontos vão os dados a serem inseridos separados por vírgula.
int returnTuple(struct CAMPOS *campos, struct page *buffer, TpTupla **new, int pg, int reg);//Retorna uma tupla na estrutura TpTupla new. pg e reg representam a pagina e o registro do buffer a ser carregado.
int returnPage(TpLista **l, struct page *buffer, struct CAMPOS *campos,int pg); // Preenche lista l com tuplas da pagina pg do buffer, se página tem 3 tuplas vai retornar lista com 3 nós.	
