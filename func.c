/*t
Executado no Ubuntu 12.04
Compilado com gcc version 4.6.3

*/
#include "func.h"
#include "error.h"

/*CONSTANTES INTERNAS */
#define CONST 60
#define CONST1 30
#define OBJ_TAM 94 // Tamanho de todos campos da estrutura OBJ somados
#define CAMPOS_TAM 49 // Tamanho de todos campos da estrutura CAMPOS somados
#define TAM_NOME 40



int qtCampos(int id);
int positionOnFile(int id);
int tamTupla(struct CAMPOS *campos);
int getTupla(char *linha,struct CAMPOS *campos, struct OBJ *tabela, int from);
void setTupla(struct page *buffer,char *tupla, int tam, int pos);
int colocaTuplaBuffer(struct page *buffer, char *tupla, struct CAMPOS *campos);
char *strcop(char *data, int pos, int tam);


//Funcao le os dados da tabela *Table_name no dicionario e seta eles na struct **table.
int leTabela(struct OBJ **table,char *Table_name){ 
	// Retorna a tabela em uma estrutura.
		
		
	if(!METADADOS){
		return FS_COLUNA_NOT_FOUND;
	}else if(!Table_name){
		return TABLE_NOT_FOUND;
	}
	FILE *tabela;
	*table = NULL;
	tabela = fopen(DICIONARIO, "r"); // abre o arquivo dicionario.dat
	
	if(!(tabela )){
		return DICIONARIO_NOT_FOUND;
	}

	*table = (struct OBJ *) malloc(sizeof(struct OBJ)); // aloca espaço para estrutura da tabela
	if(!(table)){
		return OUT_OF_MEMORY;
	}
	
	int endloop = 0;
	long maxTam; // vai receber o tamanho do arquivo
	fseek(tabela,0,SEEK_END); // posiciona ponteiro no fim do arquivo
	maxTam = ftell(tabela); // retorna numero de bytes do arquivo
	rewind(tabela); //volta ponteiro para inicio da tabela
	
	//vai preencher a estrutura TABLE
	// *table vai guardar dados como ID, nome logico, nome fisico e diretorio da tabela requisitada
	do {
		fread(&(*table)->id, sizeof(int), 1, tabela); //le o ID 
		fread((*table)->lnome,sizeof(char), CONST1, tabela); // le o nome logico da tabela
		fread((*table)->fnome,sizeof(char), CONST1, tabela);  // le o nome fisico da tabela
		fread((*table)->dir,sizeof(char), CONST1, tabela);// le o caminho do diretorio da tabela
		//fseek(tabela,-1,SEEK_CUR);
	}while(strcmp((*table)->lnome,Table_name)!=0 && ((endloop+= OBJ_TAM-1) < maxTam)); // enquanto nao achar tabela que ele requisitou(Table_name) e o arquivo nao chegou ao final
	
	if(strcmp((*table)->lnome, Table_name)){
		return TABLE_NOT_FOUND; // Testa se nome logico na estrutura *table é igual ao nome passado, se não a tabela não existe e o erro é retornado
	}

	fclose(tabela); // fecha o arquivo dicionario.dat
	return OKAY;
}
int qtCampos(int id){ // Retorna a quantidade de campos do esquema
    
    FILE *fs_coluna = NULL;
    int qtdCampos = 0;
    int pula = 0;
    int counter1 = 0;
    
   
    if(!(fs_coluna = fopen(METADADOS, "r"))){ // abre arquivo fs_coluna.dat
		return FS_COLUNA_NOT_FOUND;
	}


    fseek(fs_coluna,0,SEEK_END);//posiciona ponteiro no final do arquivo
	long pos = ftell(fs_coluna); //pega o tamanho do arquivo
	rewind(fs_coluna); // volta arquivo para inicio

     
	 //variavel pula vai receber o incremento no numero de bytes passados no arquivo
     int condicao = 1;
     while(condicao && pula < pos){ // enquanto  
			fread(&counter1, sizeof(int),1,fs_coluna); // le o id
			
			if(counter1 == id){ // se o id lido do arquivo for o mesmo da tabela requerida incrementa contador de campos
				qtdCampos++;
				fseek(fs_coluna,CAMPOS_TAM - sizeof(int),SEEK_CUR); // faz ponteiro pular os tres campos seguintes(nome atributo, tipo e tamanho) e passa a apontar pro proximo id na tabela
				pula += CAMPOS_TAM; // pula recebe os bytes lidos e atualiza seu valor

			}
			else{ // se não for o mesmo id, apenas pula para o proximo id(linha) e incrementa bytes passados no arquivo
				fseek(fs_coluna,CAMPOS_TAM - sizeof(int),SEEK_CUR); // pula para proximo id(linha)
				pula += CAMPOS_TAM; // incrementa bytes lidos no arquivo
			}
	 }
	fclose(fs_coluna);
	
    return qtdCampos;
}
int positionOnFile(int id){
	FILE *fs_coluna = NULL;

	int pula = 0;
	int counter1 = 0;
	if(!(fs_coluna = fopen(METADADOS, "r"))){ // abre arquivo fs_coluna.dat
		return OUT_OF_MEMORY;
	}

   fseek(fs_coluna,0,SEEK_END); // vai pro fim do arquivo
	long pos = ftell(fs_coluna); //pega o tamanho do arquivo
	rewind(fs_coluna); // volta arquivo para inicio

	// variavel pula vai recebendo os bytes lidos
	while(pula < pos){ // enquanto o total de bytes lidos forem menores que o tamanho do arquivo
			fread(&counter1, sizeof(int),1,fs_coluna); // le um inteiro(ID) - primeiro campo no arquivo fs_coluna.dat
			
			if(counter1 == id){ // se é o ID desejado
				return ftell(fs_coluna) - sizeof(int); // retorna a posição(em bytes) onde começa o primeiro campo meta da tabela com aquele ID 

			}
			else{ // se nao desloca para proximo id(linha) para leitura do proximo id
			
				fseek(fs_coluna,CAMPOS_TAM - sizeof(int),SEEK_CUR); // pula para proximo id
				pula += CAMPOS_TAM; // atualiza(incrementa) numero de bytes "pulados"
			}
	}

	return GENERIC_ERROR ;
}

int leMetaDados(struct CAMPOS **campos,struct OBJ *table){

	FILE *metadados;
	*campos = NULL;
	int i, j = 0, tam;
	char c;
	int error;
	int qtdCampos; 
	if(!table->id){
		return TABLE_NOT_FOUND;
	}
	
	qtdCampos = qtCampos(table->id); //qtdCampo vai reber qntos campos tera a tabela com aquele ID
	
	if(!(qtdCampos)){
		error = qtdCampos;
		return error;
	}
		
	
	metadados = fopen(METADADOS, "r");  // Abre os metadados armazenados em fs_coluna
	if(!(metadados)){
		
		return FS_COLUNA_NOT_FOUND;
	}
		

	int posi = positionOnFile(table->id); // retorna a posição onde vai constar o primeiro campo meta do ID daquela tabela requisitada
	if((posi) < 0){
		return GENERIC_ERROR;
	}
   fseek(metadados, posi, SEEK_CUR); // posiciona arquivo fs_coluna.dat para a posicao do primeiro campo meta a ser lido
	
    if(qtdCampos) // Lê o primeiro inteiro que representa a quantidade de campos da tabela.
    {
	   int t;
	  
	   *campos =(struct CAMPOS *)malloc(sizeof(struct CAMPOS)*qtdCampos); // estrutura para guardar os campos que a tabela vai ter (id,nome,tipo e tamanho)
	   // Cria uma estrutura para armazenar os dados dos campos.
	   if(!(*campos)){
		    
		    return OUT_OF_MEMORY;
		}
	   for( i = 0; i < qtdCampos ; i ++) // Laço para ler o nome, tipo e tamanho de cada campo.
	   {
		   
		   (*campos)->id = table->id; // seta ID com id que já foi pego na estrurura tabla anteriormente
		   fread(&t, sizeof(int),1,metadados); // ?
		   fread(&c, sizeof(char),1,metadados); // le primeiro caractere do nome do campo
			while (c != '\0') // Laço para ler o nome do campo.
			{
				(*campos)->nome[j] = c; // concatena o caractere no nome do campo
				fread(&c, sizeof(char),1,metadados); // le o proximo caractere
				j++;
			}
			fseek(metadados, TAM_NOME - j -1, SEEK_CUR); // ponteiro agora passa a apontar para proximo campo 
			fread(&c, 1, 1, metadados); // Lê o tipo do campo.
			(*campos)->tipo = c; // seta o campo na estrutura campos
			fread(&tam , sizeof(int), 1, metadados); // Lê o tamanho do campo.
			(*campos)->tamanho = tam; // seta o tamanho na estrutura campos
			j = 0;
			(*campos)++; // passa pra proximo campo a ser preenchido do vetor campos
		}
	   fclose(metadados);// Fecha o arquivo meta
	   (*campos) -= i;

	   return OKAY;
    }

	return GENERIC_ERROR;
}
int tamTupla(struct CAMPOS *campos){
	// Retorna o tamanho total da tupla da tabela.

    int qtdCampos, i, tamanhoGeral = 0;
    qtdCampos = qtCampos(campos[0].id);
    if(!(qtdCampos)){
		return GENERIC_ERROR;
	}

   for(i = 0; i < qtdCampos; i++)
		tamanhoGeral += campos[i].tamanho ; // Soma os tamanhos de cada campo da tabela.

	return tamanhoGeral;
}

int getTupla(char *linha,struct CAMPOS *campos, struct OBJ *tabela, int from){ 
	//Pega uma tupla do disco a partir do valor de from
	

    int tamTpl = tamTupla(campos); // recebe tamanho que a tupla tem em bytes

    FILE *dados; // arquivo da tabela
    char *arquivo; // vai ter o caminho/nomefisico da tabela em disco
    if(!(arquivo = (char*)malloc(sizeof(char)* CONST))){ // aloca um vetor de char com 60 posições que vai receber a concatenação do diretorio e do nome fisico da tabela
		 return OUT_OF_MEMORY;
	 }
		 //const 60, porque TAM de dir e nome fisico  somados terá no max 60 bytes

    arquivo[0] = '\0';
    strcat(arquivo, tabela->dir);
    strcat(arquivo, tabela->fnome);
    
    if(!(dados = fopen(arquivo, "r"))){ // abre arquivo
		return DATA_FILE_NOT_FOUND;
	}
	
	fseek(dados, from, 1);
    if(fgetc (dados) != EOF){
        fseek(dados, -1, 1);
        fread(linha, sizeof(char), tamTpl, dados); //Traz a tupla inteira do arquivo...linha agora vai conter uma tupla do arquivo .dat
    }
    else{       //Caso em que o from possui uma valor inválido para o arquivo de dados
        fclose(dados);
        return GENERIC_ERROR;
    }
    fclose(dados);
    return OKAY;
 }
 
 
 
 //Funcao cria tabela passada no parametro *novaTabela com o numero de campos definido pela variavel campos. Argumentos nome do campo, tipo e tamanho são passados via va_list em sequencia. 
 int createTable(char *novaTabela,int campos, ...) {
	va_list ap;
	int i,j=0,leInt=0,id;
	char nomeLogico[30] = "", nomeFisico[30] = "", diretorio[30] = CAMINHO, leString[40]="", *leTipo, *temp = "", caminho[30] = "";
	FILE *arquivo;
	strcat(nomeLogico,novaTabela);
	strcat(nomeFisico,novaTabela);
	strcat(nomeFisico,EXTENSAO);
	
	strcat(caminho,CAMINHO);
	strcat(caminho,novaTabela);
	strcat(caminho,EXTENSAO);
	
	if ((arquivo = fopen(caminho,"r"))) // verifica se tabela já existe
	return TABLE_EXISTS;
	
	arquivo = fopen(caminho,"wb+"); // criado arquivo .dat da nova tabela
	fclose(arquivo);
	
	//BUSCA NOVO ID
	arquivo  = fopen(DICIONARIO,"ab+");
	fseek(arquivo,0,SEEK_END);//posiciona ponteiro no final do arquivo
	int tamanho = ftell(arquivo); //pega o tamanho do arquivo
	rewind(arquivo); // volta arquivo para inicio
	fseek(arquivo,tamanho-((CONST1*3)+sizeof(int)),0);
	fread(&id,sizeof(int),1,arquivo);
	id++;
	
	//ATUALIZA fs_tabela.dat
	arquivo = fopen(METADADOS,"ab+");
	fseek(arquivo,0,SEEK_END);//posiciona ponteiro no final do arquivo para gravar nova tabela e caminhos
	campos = campos * 3;
	va_start(ap, campos);
	for (i=0;i<campos;i++) {
		// J de 0 a 3 pois sempre vai ter 3 campos (NOME,TIPO E TAMANHO)
		if (j==0) { //PARA NOME DO CAMPO
			temp = va_arg(ap, char*);
			strcpy(leString,temp);
		} else if (j==1) { //PARA TIPO DO CAMPO
			leTipo = va_arg(ap,char*);
		} else if (j==2) { //PARA TAMANHO DO CAMPO E GRAVAR
			leInt = va_arg(ap, int); 
			fwrite(&id,sizeof(int),1,arquivo);
			fwrite(leString,sizeof(leString),1,arquivo);
			fwrite(&leTipo,sizeof(char),1,arquivo);
			fwrite(&leInt,sizeof(leInt),1,arquivo);
			j=-1;
		}
		j++;
	}
	va_end(ap);
	fclose(arquivo);
	
	//ATUALIZA DICIONARIO.DAT
	arquivo  = fopen(DICIONARIO,"ab+");
	fseek(arquivo,0,SEEK_END);//posiciona ponteiro no final do arquivo para gravar nova tabela e caminhos
	fwrite(&id,sizeof(int),1,arquivo);
	fwrite(&nomeLogico,sizeof(nomeLogico),1,arquivo);
	fwrite(&nomeFisico,sizeof(nomeFisico),1,arquivo);
	fwrite(&diretorio,sizeof(diretorio),1,arquivo);
	fclose(arquivo);
	
	return OKAY;
}


//Funcao insere dados na tabela *tabela. Os dados a serem inseridos são passados na sequencia atraves de uma va_list.
int insertTable(char *tabela, ...) {
	struct OBJ *table;
	struct CAMPOS *campos;
	va_list ap;
	int i,leInt=0;
	float leFloat=0;
	double leDouble=0; 
	FILE *arquivo;
	char caminho[30] = "";
	char caminho1[30] = "";
	int error;
	char *temp = "";
	
	strcat(caminho1,CAMINHO);
	strcat(caminho1,tabela);
	strcat(caminho1,EXTENSAO);
	//PRIMEIRO VERIFICA SE TABELA EXISTE
	arquivo = fopen(caminho1,"r"); // verifica se tabela existe
	if (arquivo == NULL) return TABLE_NOT_EXISTS;
	fclose(arquivo);
	
	// AGORA LE DADOS DA TABELA
	error = leTabela(&table,tabela);	
	if(error != OKAY) return error;
	
	// AGORA JÁ SABE QUE TABELA EXISTE, ENTÃO PRECISA EXTRAIR OS METADADOS DA TABELA
	error = leMetaDados(&campos, table);
	if(error != OKAY) return error;
	
	//TENDO TODOS CAMPOS AGORA JÁ PODE GRAVAR OS DADOS NA TABELA
	strcat(caminho,table->dir);
	strcat(caminho,table->fnome);
	arquivo  = fopen(caminho,"ab+"); // abre arquivo da tabela
	if (arquivo == NULL) return TABLE_NOT_FOUND;
	fseek(arquivo,0,SEEK_END);//posiciona ponteiro no final do arquivo
	
	//PEGA OS DADOS A SEREM INSERIDOS PASSADOS PELA FUNÇÃO
	int qtdcampos = qtCampos(table->id);
	va_start(ap, tabela);
	for(i=0;i<qtdcampos;i++) {	
		if(campos[i].tipo == 'S') {
			temp = va_arg(ap, char*);
			char *leString = (char *) malloc(campos[i].tamanho*sizeof(char));
			strcpy(leString,temp);
			//GRAVA A STRING NO ARQUIVO
			fwrite(leString,sizeof(char)*campos[i].tamanho,1,arquivo);
			free(leString);
		}
		if(campos[i].tipo == 'I') {
			leInt = va_arg(ap, int);
			//GRAVA O INTEIRO NO ARQUIVO
			fwrite(&leInt,sizeof(leInt),1,arquivo);
		}
		if (campos[i].tipo == 'D') {
			leDouble = va_arg(ap,double);
			//GRAVA DOUBLE NO ARQUIVO
			fwrite(&leDouble,sizeof(leDouble),1,arquivo);
			
		}
		
	}
	va_end(ap);
	fclose(arquivo);
	free(table);
	free(campos);
	return OKAY;
}
 
 
// Funcao vai retornar a quantidade de tuplas contidas na tabela *tabela passada como parametro.
int cardinalidade(char *tabela) {
	FILE *arquivo;
	char caminho[30]="";
	int tamanho, i, tamTupla=0;
	struct OBJ *table;
	struct CAMPOS *campos;
	int error;
	
	strcat(caminho,CAMINHO);
	strcat(caminho,tabela);
	strcat(caminho,EXTENSAO);
	
	//VERIFICA SE TABELA EXISTE
	if (!(arquivo = fopen(caminho,"r")))
	return TABLE_NOT_EXISTS;
	
	// AGORA LE DADOS DA TABELA
	error = leTabela(&table,tabela);	
	if(error != OKAY) return error;
	
	// AGORA JÁ SABE QUE TABELA EXISTE, ENTÃO PRECISA EXTRAIR OS METADADOS DA TABELA
	error = leMetaDados(&campos, table);
	if(error != OKAY) return error;
	
	//PEGA OS DADOS A SEREM INSERIDOS PASSADOS PELA FUNÇÃO
	int qtdcampos = qtCampos(table->id);
	
	//TENDO TODOS CAMPOS AGORA JÁ PODE GRAVAR OS DADOS NA TABELA
	arquivo  = fopen(caminho,"r"); // abre arquivo da tabela
	if(!(arquivo)) return TABLE_NOT_FOUND;
	fseek(arquivo,0,SEEK_END);//posiciona ponteiro no final do arquivo
	tamanho = ftell(arquivo); //pega o tamanho do arquivo
	rewind(arquivo); // volta arquivo para inicio
	
	for(i=0;i<qtdcampos;i++) {
		tamTupla+=campos[i].tamanho;
	}
	
	fclose(arquivo);
	free(table);
	free(campos);
	return (int)tamanho/tamTupla;
	
} 
 
 

//============================ BUFFER ===========================//

int inicializaBuffer(struct page **buffer){

	*buffer = NULL;
    *buffer = (struct page *)malloc(sizeof(struct page)*BP);
    
    if(!(*buffer)){
		return OUT_OF_MEMORY;
	} //Aloca as páginas do buffer

    int i;
	for (i=0;i<BP;i++)  //Inicializa o buffer
	{
		(*buffer)->db=0;
		(*buffer)->pc=0;
		(*buffer)->nrec=0;
		(*buffer)->position=0;
		(*buffer)++;
	}
	
	(*buffer) -=i;

	return OKAY;
}
void setTupla(struct page *buffer,char *tupla, int tam, int pos){ 
	
	//Coloca uma tupla de tamanho "tam" no buffer e na página "pos"
	
	int i=buffer[pos].position;
	for (;i<buffer[pos].position + tam;i++){
		buffer[pos].data[i] = *(tupla++);
	}
}

int colocaTuplaBuffer(struct page *buffer, char *tupla, struct CAMPOS *campos){
	
	//Define a página que será incluida uma nova tupla
	
    int i=0, found=0;
    
	while (!found && i < BP)//Procura pagina com espaço para a tupla. BP -> Numero de Paginas no Buffer -> Aqui 512
	{
    	if(SIZE - buffer[i].position > tamTupla(campos)){// Se na pagina i do buffer tiver espaço para a tupla, coloca tupla. -> Aqui SIZE  = 1024
            setTupla(buffer, tupla, tamTupla(campos), i); // copia a tupla para  buffer
            found = 1; // achou pagina livre
            buffer[i].position += tamTupla(campos); // Atualiza proxima posição vaga dentro da pagina.
            buffer[i].nrec += 1;
            return OKAY;
    	}
    	i++;// Se não, passa pra proxima página do buffer.
    }
    return BUFFER_CHEIO;
    /*
    if (!found)
    {
        printf("Buffer Cheio! Implementar a política de troca.\n");
		return;
    }
    */
}
char *strcop(char *data, int pos, int tam){
	//Copia registro do buffer
    //pos é a posição inicial do registro a ser copiado e tam é seu tamanho total
    int i;
   
    char *linha = (char *)malloc(sizeof(char)*tam);
    if(!(linha))
		return NULL;

    for(i = 0; i < tam ; i++){
        linha[i] = data[(tam*pos) + i];// Copia a tupla para uma variável auxiliar.
		
    }
    
    return linha;
}

int carregaDados(struct page *buffer, struct CAMPOS *campos, struct OBJ *tabela, int registro){
	//Traz todos os registros para o buffer
	
    int tamTpl = (tamTupla(campos)); //tamTpl representa o tamanho total dos atributos
    char *linha;
     if(!(tamTpl)){
	  return GENERIC_ERROR;
     }

     if(!(linha =(char *)malloc(sizeof(char)*tamTpl))){ // aloca um vetor de char do tamanho da tupla
		 return OUT_OF_MEMORY;
     }
    int error;
	
    error = getTupla(linha,campos, tabela, registro*tamTpl);
    if(error == OKAY){
		error = colocaTuplaBuffer(buffer, linha, campos);
		free(linha);
		if( error != OKAY){
			return OKAY;
		}else return error;
	}
		
	return OKAY;
}

// Função para retornar uma Tupla em uma Estrutura na Memória(TpTupla *new)
int returnTuple(struct CAMPOS *campos, struct page *buffer, TpTupla **new, int pg, int reg) { // pg = pagina do buffer , reg = registro na pagina
	int tamanho = tamTupla(campos);
	if(buffer[pg].nrec < reg) return TUPLE_NOT_FOUND;
	char *tupla = strcop(buffer[pg].data, reg*tamanho, tamanho); // retorna  a tupla para string tupla;
	char *auxStr; //Variável auxiliar para leitura de string

    if(tupla == NULL)
        return ERRO_LEITURA;

    int qtdCampos, j, k=0 ; // k é a posição do byte dentro da tupla
    qtdCampos = qtCampos(campos[0].id);
    
	*new = (TpTupla *)malloc(sizeof(TpTupla)*qtdCampos); // aloca espaço para a estrutura para a quantidade de campos da tupla
	
	if(!(*new)){
		return OUT_OF_MEMORY;
	}
    for(j=0 ; j<qtdCampos; j++){
        if(campos[j].tipo == 'S'){	
            auxStr = tupla + k;    //acesso a posição de inínio de uma string
            (*new)->tipo = 'S'; // indica que o campo é uma String
            (*new)->string = auxStr; // copia a string
            k += campos[j].tamanho;     //Atualição de leitura do número de bytes para char
        }else if(campos[j].tipo == 'D'){
            double *n = (double *)&tupla[k];
            (*new)->tipo = 'D'; // indica que o campo é um Double
            (*new)->vDouble = *n;
            k += sizeof(double);   //Atualição de leitura do número de bytes para double
        }else if(campos[j].tipo == 'I'){
            int *n = (int *)&tupla[k];
            (*new)->tipo = 'I'; // indica que o campo é um Int
            (*new)->vInt = *n;
            k += sizeof(int); //Atualição de leitura do número de bytes para int
        }
        (*new)++;
    }
    (*new) -=j;
    return OKAY;
}

//Função para retornar uma lista contendo todos os registros de uma página - se página possui quatro registros irá retornar lista com quatro nós.
int returnPage(TpLista **l, struct page *buffer, struct CAMPOS *campos, int pg) {
	int i;
	int tamanho = tamTupla(campos);
	if (buffer[pg].nrec == 0) return EMPTY_PAGE;
	
	*l = (TpLista *)malloc(sizeof(TpLista));
	if(!l) return OUT_OF_MEMORY;
	(*l)->nNodo = 0;
	(*l)->first = NULL;
	(*l)->last = NULL;
	
	for (i=0; i < buffer[pg].nrec; i++) {
		char *tupla = strcop(buffer[pg].data, i*tamanho, tamanho); // retorna  a tupla para string tupla;	
		TpNodo * nodo = (TpNodo*)malloc(sizeof(struct _nodo)); // cria novo nodo
		
		if (!nodo) return OUT_OF_MEMORY;
		nodo->tupla = tupla;
		if((*l)->nNodo == 0) { // se lista vazia
			(*l)->first = nodo;
			(*l)->last = nodo;
			nodo->next = NULL;
		} else { // se lista não vazia
			(*l)->last->next = nodo;
			nodo->next = NULL;
			(*l)->last = nodo;
		}
		(*l)->nNodo+=1; // incrementa numero de nodos na lista
	}
    
	return OKAY;
}
