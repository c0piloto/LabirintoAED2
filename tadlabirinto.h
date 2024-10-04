// Estruturas usadas

typedef struct grafo {
  int eh_ponderado; // Sim 1 Não 0
  int nro_vertices; // Número de vértices do grafo
  int grau_max;  // Quantidade máxima de arestas que podem sair dos vértices
  int **arestas; // Lista de adjacências: arestas[i][j] = k -> existe aresta de
                 // i para k. j é o índice do item na lista
  int **pesos;   // Lista de peso da aresta: pesos[i][j] = k -> existe aresta
                 // partindo de i. O índice na lista é j (mesmo da lista de
                 // arestas). k é peso. Ex: arestas[i][j] = k. peso[i][j] = k2.
  int *grau;     // quantidade de arestas que o vértice possui
} Grafo;

struct NO {
  Grafo *grafo;
  float tempo_area[5];
  struct NO *esq;
  struct NO *dir;
};

typedef struct NO *ArvBin;

typedef struct jogador {
  char nome[50];
  double tempo_total;
  double tempo_area[5];
  int pontuacao;
} jogador;

// Funções utilizadas

// Cria uma arvore binária
ArvBin criar_no(Grafo *grafo);
// Insere na arvore binária
ArvBin inserir_no(ArvBin raiz, Grafo *grafo);
// Apaga a arvore binária
void finalizar_arvore_binaria(ArvBin raiz);

// Cria o grafo
Grafo *criar_grafo(int eh_ponderado, int nro_vertices, int grau_max);
// Insere uma aresta no grafo
int inserirAresta(Grafo *grafo, int origem, int destino);
// Imprime o grafo
int imprimirGrafo(Grafo *grafo);
// Apaga o grafo
void finalizar_grafo(Grafo *grafo);

// Gera as areas do labirinto
int geraAreas(Grafo *grafo);
// Verifica se a aresta já existe
int existeAresta(Grafo *grafo, int origem, int destino);
// Insere uma aresta no grafo
int inserirAresta(Grafo *grafo, int origem, int destino);
// Imprime o grafo
int imprimirGrafo(Grafo *grafo);

jogador *criarJogador(char *nome);
// Ordenar o vetor pelo nome dos jogadores
void insertionSortNome(struct jogador *vetor, int tamanho);

// Ordenar o vetor pelo tempo total dos jogadores
void insertionSortTempo(struct jogador *vetor, int tamanho);

// Ordenar o vetor pelo tempo de cada area dos jogadores
void insertionSortArea(struct jogador *vetor, int tamanho, int area);

// Ordenar o vetor pelo pontuacao dos jogadores
void insertionSortPontuacao(struct jogador *vetor, int tamanho);

// Imprime os dados dos 10 últimos jogadores
void imprimirJogadores(jogador vetor[], int tamanho, int opcao, int area);

// Função que faz com que o jogador jogue o labirinto e também nos dá a
// pontuação final e o tempo levado para completar de cada area
int jogarLabirinto(ArvBin raiz, Grafo *grafo, jogador *jogador, int *indice,
                   int *area_atual);

int posFixa(ArvBin raiz, jogador *jogador, int *indice, int *area_atual);

int importarBancoDeDados(jogador *jogadores);

int salvarBancoDeDados(jogador *jogadores);