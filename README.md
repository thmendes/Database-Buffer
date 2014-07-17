Universidade Federal da Fronteira Sul
Ciência da Computação - Banco de Dados II
Implementação de Buffer

PRIMEIRA PARTE:
Por Gabrielle Souza e Lais Borin

SEGUNDA PARTE:
Edson Lemes da Silva
Lucas Cezar Parnoff

TERCEIRA PARTE:
Caciano de Marco
Mateus Trebien
Thiago Mendes

BDII-2


--------- Resumo das Alterações e Novas Funcionalidades feitas na 3ª versão -----------  

Novas Funcionalidades: Criadas funções:
						createTable - Para criação de novas tabelas.
						insertTable - Para inserção de novos registros nas tabelas.
						cardinalidade - Para retorno da quantidade de tuplas de determinada tabela.
						returnTuple - Para retornar uma tupla em uma estrutura para vir a ser impressa pelo usuário.
						returnPage - Para retornar em uma Lista Encadeada todas as tuplas de uma página no buffer.
						
						Criadas Estruturas:
						 TpTupla - Estrutura para receber uma Tupla do buffer para posteriormente ser impressa pelo usuário.
						 TpLista - Estrutura em forma de lista para receber todas as tuplas de uma página do buffer.
						 TpNodo  - Estrutura de cada nó da lista. Cada nó armazenará uma tupla de uma pagina do buffer.
						Inseridos novos erros:
						 TABLE_EXISTS 
						 TABLE_NOT_EXISTS
						 
Funcionalidade Alteradas: - Alteração em funções pré-existentes:
						   carregaDados - Função agora insere um registro por vez no buffer ao invés da tabela inteira. Adicionado parâmetro int reg ao escopo da função, onde este vai receber um inteiro indicando qual 
									   tupla da tabela deverá ser carregada no buffer. OBS: Primeiro registro da Tabela tem o valor 0.
							
						  - Nome da Tabela, arquivo de dicionario de dados e de metadados não são mais passados por parâmetro em tempo de execução.
						  
						  - Dentro do arquivo do dicionario de dados, o nome físico das tabelas agora passa a ser composto pelo nome da tabela acrescido da extensão .dat 

Estruturas:
	Para utilização, inclua "func.h" no programa.

	struct CAMPOS : Representa um atributo de uma tabela, deve-se declarar um ponteiro do tipo CAMPOS para o funcionamento 
	do programa.

	struct page : Refere-se a estrutura do buffer, deve-se declarar um ponteiro do tipo page. A constante BP representa
	o número de página que terá o buffer, e o SIZE, tamanho de cada página.

	struct OBJ : Esta estrutura armazena uma tabela, para o funcionamento do programa, deve-se declarar um ponteiro para este tipo.

	TpTupla: A estrutura vai armazenar uma tupla da memória para impressão pelo usuário. A estrutura terá 4 campos, um char para indicar o tipo do dado, um para um valor Inteiro, um para um valor 	
			 Double e outro para uma String. Apena um dos campos(além do tipo) será preenchido, quem indicará o campo a ser impresso da Struct é o char tipo(assume um dos caracteres - 'S' , 'I' ou 'D').
			 Deve ser declarado um ponteiro do tipo TpTupla.
	
	TpLista: A estrutura lista vai encadear n nodos cada um com uma tupla de uma pagina do buffer. A estrutura TpLista possui três campos, um int nNodo com o total de nós da lista e dois ponteiros do tipo TpNodo
			 first e last. Os ponteiros irão armazenar o primeiro e o último nó da lista respectivamente. Para utilzá - la e usar a função returnPage deve se declarar um ponteiro do tipo TpLista.

	TpNodo : A estrutura TpNodo vai representar um nó da lista. A lista terá o mesmo número de nós que o número de tuplas presentes na página do buffer. Cada nó vai conter uma tupla da página do buffer. Para isso terá dois campos,
			 um ponteiro do tipo char(char *tupla) para armazenar a tupla do buffer e o segundo do tipo struct_nodo(struct_nodo *next) que vai apontar para o nó seguinte. 
	
Funções para o funcionamento:

	Ordem de execução das funções:
	
	** Nesta nova versão, foram criadas três novas funções que podem ser executadas antes ou após a ordem definida anteriormente e duas para apenas após o preenchimento do buffer, dependendo do usuário em questão. 
	   A ordem seguinte do funcionamento será descrita logo após a explicação destas novas funções:
	   
	   1) int createTable(char *novaTabela,int campos, ...):
	    A função vai receber dois parâmetros fixos, sendo eles o nome da nova tabela(novaTabela) e a quantidade de campos que a nova tabela terá(int campos).
		Todos parametros na sequência destes vão ser os campos da nova tabela na seguinte ordem: nome do campo, tipo e tamanho para cada campo(Se string valor variável, se int tamanho é 4 e se double tamanho é 8). 
		Como exemplo vamos supor que seria criada uma tabela UNIVERSIDADES(está inserido na variavel novaTabela)
		com três campos, sendo eles o nome da universidade, a sigla e o ano de fundação. Para a criação dessa tabela o comando seria:
								        |nro   
										|campos    |tipo
			createTable("universidade", 3, "Nome",'S',40, "Sigla", 'S', 7, "Ano Fundação", 'I', 4);
							|nome tabela 	 |nome	  |tamanho
														campo|
		O comando vai criar a tabela com os 3 campos e atualizar o arquivo de dicionario de dados e também de metadados. Deve se respeitar o numero de campos informado na variavel campos com os campos passados em sequencia.
		Lembrando que os campos podem ser do tipo String('S'), Inteiro('I') e double('D').
		
	    2) int insertTable(char *tabela, ...):
		  A função vai receber apenas um parâmetros fixo que será o nome da tabela a inserir a tupla(tabela).
		  Os parâmetros na sequência serão os dados que vão ser inseridos. Assim, usando o mesmo exemplo utilizado na criação da tabela, para inserir uma tupla na tabela UNIVERSIDADES a chamada da função seria:
		   
												|nome tabela      |primeiro campo                       |segundo campo
			insertTable("universidade", "Universidade Federal da Fronteira Sul", "UFFS", 2010);
																												|terceiro campo
																												
		  Esse comando vai inserir na tabela Universidade o nome Universidade Federal da Fronteira Sul, Sigla UFFS e Ano de Fundação 2010. Sempre deve se respeitar o numero de campos que a tabela possui, campos passados após 
		  o número que a tabela possui serão desconsiderados.
		  
		3) int cardinalidade(char *tabela):
		  A função vai receber apenas um parâmetro que é o nome da tabeça.
		  A função vai retornar a cardinalidade da tabela que foi passado o nome por parâmetro. 
	
	OBS: No exemplo main.c antes de seguir os passos que seguem a sequencia(itens de 1 à 6) abaixo, foram usadas as funções de criação de tabela e de inserção de uma tupla na tabela. A execução do exemplo main.c vai criar a tabela
		 universidade com três campos: Nome do tipo String com 40 caracteres, Estado do tipo String com 25 caracteres e Ano de Fundacao do tipo Int. 
		 Logo após vai inserir na tabela universidade a seguinte tupla: UFFS Santa Catarina 2010. Assim o campo Nome vai receber a string UFFS, o campo Estado a string Santa Catarina e o Ano de Fundacao o inteiro 2010.
		 
	Na sequência o programa segue conforme as seguintes instruções:
	
	1.Primeiramente, faz-se necessário a declaração de um buffer da seguinte maneira:
	inicializaBuffer(&[PONTEIRO page]);

	Onde a função "inicializaBuffer()" aloca memória para o ponteiro passado pelo parâmetro, definido pelas constantes BP (páginas) 
	com tamanho SIZE de dados.
	*BP e SIZE podem ser modificados no define da biblioteca func.h.
	A função retorna um inteiro, sendo ele negativo, que representa um status. As definições dos status possíveis,estão no arquivo
	"error.h".

	2.leTabela(struct OBJ **table,char *dicionario, char *Table_name):
	recebe um ponteiro do tipo OBJ ( deve ser declarado antes);
	recebe uma string contendo o nome da tabela a ser lida.
    A função retorna um inteiro negativo, que representa um status(informações em "error.h").

	3.leMetaDados(struct CAMPOS **campos, struct OBJ *table);
	recebe um ponteiro do tipo CAMPOS (deve ser declarado antes). OBJ table é a estrutura criada com a função leTabela (primeiro parâmetro).
	O retorna é um inteiro negativo, que representa um status(informações em "error.h").

	4.carregaDados(struct page *buffer, struct CAMPOS *campos, struct OBJ *tabela, int reg):
	A função recebe uma estrutura do tipo page (criada anteriormente com a função "inicializaBuffer()").
	Recebe uma estrutura do tipo campos ( criada na função "leMetaDados()");
	e uma função tipo OBJ ( criada em "leTabela");
	Por fim, ainda recebe um parametro do tipo int(int reg) que indica qual registro da tabela que será carregado para o buffer. O primeiro registro da tabela tem numero de registro = 0.
	
	A função retorna um inteiro negativo, que representa um status, consulte : "error.h".

	5. Nesta terceira versão, foram adicionadas duas novas funções, uma para extrair um tupla do buffer em uma estrutura para que o usário possa imprimir e outra que retorna em uma estrutura do tipo lista todas as tuplas de
       uma determinada página do buffer. Assim, temos:

		1 - int returnTuple(struct CAMPOS *campos,struct page *buffer, TpTupla **new, int pg, int reg):
				  A função vai extrair todos os dados e inserir na Estrutura TpTupla *new os dados. A estrutura vai funcionar como um vetor, onde cada posição vai conter o tipo do campo e a variavel de seu tipo associado setada.
				  
		2 - int returnPage(TpLista **l, struct page *buffer, struct CAMPOS *campos,int pg):
				  A função todas as tuplas da pagina pg do buffer e colocar todas as tuplas numa estrutura em forma de lista encadeada. A Lista vai ser do tipo TpLista e conterá um inteiro nNodo com a quantidade de nós na lista e
				  mais dois tipos TpNodo, um first que será o primeiro nó da lista e last que será o último. Todos os nós da lista serão do tipo TpNodo e conterão um ponteiro de char tupla onde estarão todos dados da tupo e ainda
				  um outro struct _nodo next que vai apontar para o nó seguinte.
		
		Ambas as funções retornam um inteiro, sendo ele negativo, que representa um status. As definições dos status possíveis,estão no arquivo "error.h".

	Utilização:

	A partir desta versão, não são mais inseridos os caminhos dos arquivos de dicionario de dados e de metadados em tempo de execução. O caminho até os arquivos de dicionario e metadados estão definidos no arquivo func.h em DICIONARIO e METADADOS respectivamente.
	O usuário não informa mais estes campos, apenas vai entrar com o nome da tabela que deseja manipular no programa na variável char *Table_name como no exemplo main.c. 
	
	Também foram definidos o caminho até a pasta onde estão os arquivos .dat na variável CAMINHO e também a extensão das tabelas físicas na variável EXTENSAO.
	Se for alterada a pasta onde ficam os arquivos de dados, deve ser alterado o caminho da variável CAMINHO no arquivo func.h .

FUNÇÕES INTERNAS:

	int positionOnFile(int id): Essa função uma posição x no arquivo fs_coluna.dat
	Usada para saber onde começa os atributos de uma tabela no arquivo fs_coluna.dat com id informado como parametro;

	int tamTupla(struct CAMPOS *campos): A função conta os tamanhos de cada atributo
	em uma tabela, e retorna a quantidade total de bytes.

	int qtCampos(int id): Retorna a quantidade de atributos em uma tabela com o id informado.

	getTupla(char *linha,struct CAMPOS *campos, struct OBJ *tabela, int from):
	Procura uma tupla no arquivo de dados "Sua localização está em tabela.dir" e seu nome
       em"tabela.fnome".

	setTupla(struct page *buffer,char *tupla, int tam, int pos): Função auxiliar que coloca
	numa página do buffer o conteúdo da tupla.

	colocaTuplaBuffer(struct page *buffer, char *tupla, struct CAMPOS *campos):
	Procura uma página livre para colocar a tupla.

	char *strcop(char *data, int pos, int tam): Copia uma tupla do buffer, usada nas funções de
	impressão.  
		
	int cardinalidade(char *tabela); // retorna a quantidade de tuplas da tabela, passado o nome da tabela como argumento

=========

Sobre os arquivos de exemplos:

Temos 3 exemplos de tabelas :

	O arquivo dicionario.dat agora está organizado deste modo:

	id | nome Lógico  | nome físico     | diretório
	-------------------------------------------
	1  |  cliente     |  cliente.dat    | files/data/
	2  |  personagem  |  personagem.dat | files/data/
	3  |  carro       |  carro.dat      | files/data/

	O arquivo fs_coluna.dat esta organizado assim :

	id | nome do atributo  |  tipo  | tamanho
	-----------------------------------------
	 1 |       nome       |   S     | 20
	 1 |       ender      |   S     | 30
     1 |       idade      |   I     |  4
     2 |       nome       |   S     |  30
     2 |       origem     |   S     |  15
     2 |       level      |   I     |  4
     2 |       Hplevel    |   I     |  4
     2 |       Classe     |   S     |  20
     3 |       nome       |   S     |  20
     3 |       chassi     |   S     |  50
     3 |       fabricante |   S     |  20
     3 |       anofab.    |   I     |  4
     3 |       potencia   |   D     |  8

	Arquivo cliente.dat  :

	nome | ender  | idade
	---------------------
	Iron  | R. Oito| 450
	Steel | R. Nove| 670 
	Silver| R. Dez | 552	
	
	Arquivo personagem.dat

	nome         | origem     | level | hplevel | classe
	-------------------------------------------------
	The killer   |Wilderness  |30     | 500     | Guerreiro
	Monster B.   |South       |100    |1595     | Paladino
	Hellhound    |Underworld  |150    |2010     | Beast
	Abyssal Demon|Underworld  |150    |2009     | Demon
	Mister Mage  |Land of Snow| 51    |753      | Human

	Arquivo carro.dat

	nome    | chassi       | fabricante  | anofab | potencia
	--------------------------------------------------------
	Golf    | 66ddf555www62| Volkswagen  | 2010   | 1.8
	Sandero | kk8080dfww101| Renault     | 2013   | 1.6
	Hb20    | mon65dd202758| Hyundai     | 2013   | 1.8
	Civic   | 2255gh3qw22rt| Honda       | 2012   | 2.0
	Corolla | oo2021vb26741| Toyota      | 2012   | 1.4
	Ka      | nn65df555512s| Ford        | 2010   | 1.0
	Classic | 23125la55vo11| Chevrolet   | 2013   | 1.0
