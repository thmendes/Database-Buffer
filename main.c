#include "func.h"
#include "error.h" //incluir "error.h" aqui é Opcional
int main(){
    //main demonstrativa, para mais detalhes consulte arquivo README.md e func.c

   char *Table_name = "carro"; // nome da tabela que será manipulada no buffer

    struct page *buffer;
    struct CAMPOS *campos;
    struct OBJ *table;
    TpTupla *tp;
    TpNodo *n;
    TpLista *lista;
    int error,i;
	
	error = createTable("universidade",3,"Nome",'S',40,"Estado",'S',25,"Ano de Fundacao",'I',4); // "universidade" será a tabela que será criada
	if(error != OKAY){
		printf("erro > %d  .CRIA TABELA fs_tabela\n", error);
		return -1;
	}
	
	error = insertTable("universidade","UFFS","Santa Catarina",2010); // "universidade será a tabela onde os dados vão ser inseridos
	if(error != OKAY){
		printf("erro > %d  .INSERE TABELA fs_tabela\n", error);
		return -1;
	}
	
	if(!inicializaBuffer(&buffer)== OKAY){
		printf("erro > %d  .SEM MEMORIA\n", error);
		return -1;
	}
	
	error = leTabela(&table,Table_name);	
	if(error != OKAY){
		printf("erro > %d  .LEITURA fs_tabela\n", error);
		return -1;
	}
	
	error = leMetaDados(&campos,table);
	if(error != OKAY){
		printf("erro > %d   .LEITURA fs_coluna\n", error);
		return -1;
	}
	
	//AQUI POSSUI O META CARREGADO em *campos e DADOS DA tabela em *table
	
	error = carregaDados(buffer,campos,table,0); // Carrega tupla 0 da tabela carro no buffer
		if(error != OKAY){
			printf("erro > %d   .LOAD DATA\n",error);
			return -1;
		}
		
	error = returnTuple(campos,buffer,&tp,0,0);	
	if(error != OKAY){
		printf("erro > %d   .TUPLA BUFFER\n", error);
		return -1;
	}

	error = returnPage(&lista,buffer,campos, 0);
	if(error != OKAY){
		printf("erro > %d   .TUPLA BUFFER\n", error);
		return -1;
	}
	
    return 0;
}
