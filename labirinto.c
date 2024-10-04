#include "tadlabirinto.h" // Nosso TAD
#include <stdio.h>        // Biblioteca padrão de entrada e saida
#include <stdlib.h>       // Biblioteca padrão
#include <string.h>       // Biblioteca de string
#include <sys/time.h> // Biblioteca de tempo do systema (usada para calcular o tempo de jogo)
#include <sys/types.h> // Biblioteca de tipos do sistema (usado para clock_t e pid_t)
#include <time.h>      // Biblioteca basica te tempo (usada para o time_t)
#include <unistd.h> // Usada para ser possivel usar as funções de sistemas Unix (Não é utilizada no código, porém é inserida para modularidade do código).

// Função para inicializar um novo nó da árvore binária
ArvBin criarNo(Grafo *grafo) {
  ArvBin novo_no = (ArvBin)malloc(sizeof(struct NO));
  if (novo_no != NULL) {
    novo_no->grafo = grafo;
    novo_no->esq = NULL;
    novo_no->dir = NULL;
  }
  return novo_no;
}

// Função para inserir um novo nó na árvore binária
ArvBin inserirNo(ArvBin raiz, Grafo *grafo) {
  if (raiz == NULL)
    return criarNo(grafo);
  if (grafo->nro_vertices < raiz->grafo->nro_vertices)
    raiz->esq = inserirNo(raiz->esq, grafo);
  else if (grafo->nro_vertices > raiz->grafo->nro_vertices)
    raiz->dir = inserirNo(raiz->dir, grafo);
  else
    printf("Erro: Area ja existe no labirinto!\n");
  return raiz;
}

// Função para inicializar um novo grafo
Grafo *criarGrafo(int eh_ponderado, int nro_vertices, int grau_max) {
  Grafo *novo_grafo = (Grafo *)malloc(sizeof(struct grafo));
  if (novo_grafo != NULL) {
    novo_grafo->eh_ponderado = eh_ponderado;
    novo_grafo->nro_vertices = nro_vertices;
    novo_grafo->grau_max = grau_max;
    novo_grafo->arestas = (int **)malloc(nro_vertices * sizeof(int *));
    novo_grafo->pesos = (int **)malloc(nro_vertices * sizeof(int *));
    novo_grafo->grau =
        (int *)calloc(nro_vertices, sizeof(int)); // Inicializa com 0

    for (int i = 0; i < nro_vertices; i++) {
      novo_grafo->arestas[i] = (int *)malloc(grau_max * sizeof(int));
      novo_grafo->pesos[i] = (int *)malloc(grau_max * sizeof(int));
    }
  }
  return novo_grafo;
}

// Função para finalizar um grafo
void finalizarGrafo(Grafo *grafo) {
  if (grafo != NULL) {
    for (int i = 0; i < grafo->nro_vertices; i++) {
      free(grafo->arestas[i]);
      free(grafo->pesos[i]);
    }
    free(grafo->arestas);
    free(grafo->pesos);
    free(grafo->grau);
    free(grafo);
  }
}

// Função para finalizar uma árvore binária
void finalizarArvoreBinaria(ArvBin raiz) {
  if (raiz != NULL) {
    finalizarArvoreBinaria(raiz->esq);
    finalizarArvoreBinaria(raiz->dir);
    finalizarGrafo(raiz->grafo);
    free(raiz);
  }
}

// Verifica se já existe uma aresta entre dois vertices, para que não crie
// arestas duplicadas.
int existeAresta(Grafo *grafo, int origem, int destino) {
  if (grafo == NULL)
    return -1;

  for (int i = 0; i < grafo->grau[origem]; i++) {
    if (grafo->arestas[origem][i] == destino)
      return 0;
  }

  return 1;
}

// Função que gera as salas do grafo, ou seja, gera cada vertice do grafo e as
// conecta com as salas adjacentes.
int geraAreas(Grafo *grafo) {
  if (grafo == NULL)
    return -1;

  // Variaveis usadas durante o programa, saida é sempre o ultimo vertice e
  // conexaoSaida é usada para checar se há algum vertice que se conecta com a
  // saida.
  int destino, origem, grau, saida = grafo->nro_vertices - 1, conexaoSaida = 0;

  // Este for é usado para percorrer do primeiro vértice até o ultimo vértice e
  // assim fazer cada conexão
  for (origem = 0; origem < grafo->nro_vertices; origem++) {

    // Usando o clock como seed pois é um valor que muda com mais frequencia que
    // o tempo do computador, assim fazendo com que o padrão de aleatoriedade
    // seja maior.
    clock_t runtime = clock();
    unsigned int seed = (unsigned int)(runtime);
    srand(seed);

    // Verifica se está atualmente na origem, e faz ela ter grau que seja um
    // menor que o tamanho do grau maximo perimitido, isto é feito para que não
    // seja criado uma origem com apenas uma aresta ou com 0 arestas. O else é
    // feito caso não estejamos na origem o qual gera grau randomico que seja de
    // 1 até o tamanho do grau maximo
    if (origem == 0)
      grau = grafo->grau_max - 1;
    else
      grau = (rand() % grafo->grau_max) + 1;

    // Esta linha é usada para verificar se o grau gerado foi 0 e então o coloca
    // com um minimo de 2, porem essa checagem é uma checagem de precaução pois
    // se a geração anterior ocorrer como esperado nunca será gerado grau 0.
    if (grau <= 0)
      grau = 2;

    // Loop usado enquanto grau > 0, esse é usado para gerar cada aresta daquele
    // vertice.
    do {
      // Clock calculado novamente para que a seed seja novamente calculada,
      // para caso seja preciso gerar o destino sendo diferente da geração
      // anterior. Parece redundante, mas sem pegar o clock novamente a seed não
      // é recalculada. També deve ser pego cada vez aqui dentro para que seja
      // uma seed diferente em cada loop.
      clock_t runtime = clock();
      unsigned int seed = (unsigned int)(runtime);
      srand(seed);

      // Calculando o destino baseado na nova seed.
      destino = rand() % grafo->nro_vertices;

      // Verifica se a origem é 0, ou seja, o primeiro vertice do labirinto, e
      // também se o destino é a saida, caso isso aconteça, gera novamente o
      // destino, pois o vertice de inicio não deve ter conexão com a saida.
      // Como o calculo da seed está dentro do escopo desse do while ela não
      // precisa ser recalculada, diferente de anteriormente.
      if (origem == 0 && destino == saida)
        destino = rand() % grafo->nro_vertices;

      // Apenas checa se o destino é igual a saida, caso seja, marca que uma
      // conexão com a saida existe no labirinto, lembrando que isso nunca
      // acontecerá caso o vertice atual seja 0.
      if (destino == saida)
        conexaoSaida = 1;

      // Se a origem for diferente do destino e não existir aquela aresta já
      // então o código insere a aresta no grafo.
      if (origem != destino && existeAresta(grafo, origem, destino) == 1) {
        int retorno = inserirAresta(grafo, origem, destino);
        int debbug = 0;

        // printf("\nModo debbug ativado, digite -1 para usar qualquer outro
        // numero para manter sem usar\n");
        // scanf("%d", &debbug);

        // Checando o retorno de inserir Aresta para o tratamento de erros.
        // Mostrado na tela apenas durante a criação do labirinto e usada só
        // para que o programador saiba o que está acontecendo. Caso queira
        // testar tire os comentários acima do printf e scanf e use -1 pra
        // ativar o debbug e mostrar as mensagens.
        if (debbug == -1) {
          if (retorno == 0)
            printf("\nA aresta foi inserida com sucesso\n");
          else if (retorno == -2)
            printf(
                "\nA origem estah fora dos limites do grafo! (Menor que 0 ou "
                "maior que o numero de vertices)\n");
          else if (retorno == -3)
            printf(
                "\nO destino estah fora dos limites do grafo! (Menor que 0 ou "
                "maior que o numero de vertices\n");
          else if (retorno == -4)
            printf("\nOrigem jah atingiu seu maximo de arestas\n");
        }

        // Diminui o grau para que o loop continue até que seja 0.
        grau--;
      }
    } while (grau > 0);
  }

  // Checa se existe uma conexão com a saida, caso não exista, gera uma conexão
  // com a saida em dois lugares para garantir que a saida seja conectada com
  // pelo menos uma aresta e seja possivel sair.
  if (!conexaoSaida) {
    for (int i = saida - 1; i > 0; i--)
      inserirAresta(grafo, i, saida);
  }

  return 0;
}

// Insere uma aresta no grafo
int inserirAresta(Grafo *grafo, int origem, int destino) {
  // Checagens de rotina, se o grafo existe, se a origem esta entre os limites
  // do possível, se o destino esta entre os limites do possível e se a origem
  // já não atingiu o grau máximo. A escolha de usar um if para cada é
  // deliberada, pq que tenham codigos de retorno diferentes e assim possam
  // conter mensagens de erro diferentes.
  if (grafo == NULL)
    return -1;
  if (origem < 0 || origem >= grafo->nro_vertices)
    return -2;
  if (destino < 0 || destino >= grafo->nro_vertices)
    return -3;
  if (grafo->grau[origem] == grafo->grau_max)
    return -4;

  // Gera uma seed baseado no clock.
  clock_t runtime = clock();
  unsigned int seed = (unsigned int)(runtime);
  srand(seed);

  // Insere a aresta de destino na origem com indice baseado no grau, já que o
  // grau sempre está incrementando e é uma informaçao consistente entre funções
  // por estar no proprio grafo.
  grafo->arestas[origem][grafo->grau[origem]] = destino;

  // Verifica se é ou não ponderado para caso seja necessário inserir um
  // peso, o qual é gerado aleatoriamente como um numero float.
  if (grafo->eh_ponderado == 1)
    grafo->pesos[origem][grafo->grau[origem]] =
        (int)(rand() / (double)RAND_MAX * 100) + 1;
  grafo->grau[origem]++;

  return 0;
}

// Função feita para imprimir o grafo na tela, usada apenas durante a criação do
// programa para debbug.
int imprimirGrafo(Grafo *grafo) {
  if (grafo == NULL)
    return -1;

  // Verifica se o grafo é ponderado, caso seja, imprime o peso.
  char *eh_pon;
  if (grafo->eh_ponderado == 1)
    eh_pon = "Sim";
  else
    eh_pon = "Nao";

  // Impresões na tela.
  printf("\n\n\nGrafo eh ponderado?\n%s\n", eh_pon);
  printf("Numero de vertices: %d\n", grafo->nro_vertices);
  printf("Grau maximo: %d\n\n", grafo->grau_max);
  for (int i = 0; i < grafo->nro_vertices; i++) {
    printf("Vertice %d tem grau: %d\n", i, grafo->grau[i]);
    printf("Arestas de %d levam aos vertices:", i);
    for (int j = 0; j < grafo->grau[i]; j++)
      printf(" %d", grafo->arestas[i][j]);
    printf("\n\n");
  }

  return 0;
}

// Função que carrega toda a lógica de jogo.
int jogarLabirinto(ArvBin raiz, Grafo *grafo, jogador *jogador, int *indice,
                   int *area_atual) {
  // Checagens de rotina, se o grafo existe e se a raiz é valida.
  if (grafo == NULL || raiz == NULL)
    return -1;

  // Variaveis usadas no programa, assim como os vetores auxiliares também,
  // visitados e ultimoVisitado.
  int origem, destino, i = 0, saida, atual, pontos = 0, indexAux = 0;
  float pontuacaoFinal = 0;
  int *visitados = calloc(grafo->nro_vertices, sizeof(int));
  int *ultimoVisitado = calloc(grafo->nro_vertices, sizeof(int));

  // Clock usado para contar o inicio de uma jogada, feito para contar o tempo
  // gasto pelo jogador para completar o labirinto.
  struct timeval inicio, fim;
  double segundos, milissegundos;
  gettimeofday(&inicio, NULL);

  // Loop para que seja definida a origem do jogador.
  while (1) {
    // Marca qual a origem do grafo e usa para criar o primeiro vertice que o
    // jogador estará, se ele encontrou onde está a origem sai do loop.
    if (grafo->arestas[0][i] == 0) {
      origem = grafo->arestas[0][i];
      break;
    }
    i++;
  }
  // Coloca a saída como o ultimo vertice.
  saida = grafo->nro_vertices - 1;
  // Coloc ao atual como a origem do grafo.
  atual = origem;

  // Garante i = 0 para usos futuros como indice de visitados.
  i = 0;

  // Se o grafo for não ponderado entra nesse if e faz o jogo do grafo não
  // ponderado
  if (grafo->eh_ponderado == 0) {

    // Loop do jogo para grafo não ponderado.
    do {

      // Mostra onde está, é usado atual + 1 pois queremos que o jogador veja o
      // mapa de 1 a nrovertices, mas a area real é 0.
      printf("\nVoce esta na sala %d da area %d.\n", atual + 1,
             *area_atual + 1);
      printf("Suas possibilidades de caminhos sao: ");

      // Inicia um contador
      int cont = 0;
      // Este for é usado para andar dentro do vetor de arestas do grafo, feito
      // para mostrar cada aresta possivel.
      for (int aux = 0; aux < grafo->grau[atual]; aux++) {

        // É iniciado duas variaveis uma que marca o proximo vertice e outra que
        // sera usada pra determinar se já foi visitado.
        int proximo = grafo->arestas[atual][aux];
        int visitado = 0;

        // For que checa no vetor de visitados se o proximo vertice já foi
        // visitado, lembrando que como é usado calloc todos os espaços do vetor
        // tem valor 0.
        for (int aux2 = 0; aux2 < grafo->nro_vertices; aux2++) {
          // Se o proximo vertice já foi visitado, então marca visitado como 1 e
          // sai do for.
          if (visitados[aux2] == proximo) {
            visitado = 1;
            break;
          }
        }
        // Se visitado for 0, então o vertice ainda não foi visitado, então o
        // programa o mostra na tela como um vertice possível. E aumenta o
        // contador, para controlar quantas vezes pasosu no loop usado na
        // proxima instruçao.
        if (visitado == 0) {
          printf("%d ", proximo + 1);
          cont++;
        }
      }

      // Checa se o contador é 0, ou seja, não imprimiu absolutamente nenhuma
      // vertice, isso significa que vocë está em uma sala sem saída, então o
      // jogo recomeça do inicio do grafo atual.
      if (cont == 0) {
        printf(
            "\nVoce nao pode ir para nenhum lugar! Recomecando do inicio!\n");

        // Coloca o destino como 0, e depois coloca o atual como o destino, para
        // que volte do começo.
        destino = 0;
        atual = destino;
        // Coloca i como 0 para voltar no começo de visitados.
        i = 0;
        // Passa por cada espaço de visitados e os reseta para 0, assim zerando
        // o vetor de visitados.
        for (int aux = 0; aux < grafo->nro_vertices; aux++) {
          visitados[aux] = 0;
        }

      }

      // Se cont > 0 entáo há pelo menos um caminho possível.
      else if (cont > 0) {

        // Pega o destino desejado pelo jogador.
        printf("\nDigite para onde quer ir: ");
        scanf("%d", &destino);

        // Coloca o destino como o digitado pelo jogador - 1, pois como
        // mostramos com os indices somados de 1 para mostrar de 1 a
        // nrovertices, então precisamos subtrair 1 pois esse será o numero real
        // no grafo.
        destino -= 1;

        // Coloca o atual na lista de visitados, pois estamos saindo dele.
        visitados[i] = atual;

        // For que checa se o destino escolhido já foi visitado.
        for (int aux = 0; aux < grafo->nro_vertices; aux++) {

          // Caso já tenha sido visitado pede para que o jogador escolha outro
          // destino dentro dos que foram mostrados antes.
          if (visitados[aux] == destino) {
            printf("\nVoce ja passou por aqui, ou esta aqui! Digite um novo "
                   "destino dentre as "
                   "ultimas opcoes apresentadas: ");
            // Pega o destino e transforma o aux em 0 para percorrer todo
            // visitados novamente e verificar se ele já não foi visitado também
            scanf("%d", &destino);
            destino -= 1;
            aux = 0;
          }
          // Caso não tenha sido visitado, então checa se existe, caso nõ exis
          // pede outro e faz as mesmas coisas que anteriormente.
          else if (existeAresta(grafo, atual, destino) != 0) {
            printf("\no caminho desejado nao existe, escolha outro destino que "
                   "exista: ");
            scanf("%d", &destino);
            destino -= 1;
            aux = 0;
          }
        }

        // Caso seja um caminho possível e esteja tudo certo então coloca ele
        // como o atual e aumenta o indice para que possa continuar o jogo.
        atual = destino;
        i++;
      }

      // Condiçao de parada é quando o destino selecionado for a saida.
    } while (destino != saida);

  }
  // Este else if é para caso o grafo seja ponderado.
  else if (grafo->eh_ponderado == 1) {
    do {

      // Mostra na tela exatamente como mostra no não ponderado.
      printf("\nVoce esta em: %d\n", atual + 1);
      printf("Suas possibilidades de caminhos sao: ");

      // Checa se já foi visitado da mesa forma que o não ponderado.
      for (int aux = 0; aux < grafo->grau[atual]; aux++) {
        int proximo = grafo->arestas[atual][aux];
        int visitado = 0;
        for (int aux2 = 0; aux2 < grafo->nro_vertices; aux2++) {
          if (visitados[aux2] == proximo) {
            visitado = 1;
            break;
          }
        }
        if (visitado == 0)
          printf("%d ", proximo + 1);
      }

      // Primeira diferença é a possibilidade de voltar gastando 1 ponto, aqui
      // mostra o vetor de volta.
      printf(" | Voltar para: %d (Custa 1 ponto)",
             (ultimoVisitado[indexAux] + 1));

      printf("\nDigite para onde quer ir (ou -1 para voltar): ");
      scanf("%d", &destino);
      // Se o destino digitado for maior do que 0 o transforma no valor real do
      // grafo.
      if (destino > 0)
        destino -= 1;
      // Caso seja digitado 0 coloca como -2 para tratar erro.
      if (destino == 0)
        destino = -2;

      // Salva o atual no ventro de visitados
      visitados[i] = atual;

      if (destino == -1) {
        if (ultimoVisitado[indexAux] >= 0 &&
            ultimoVisitado[indexAux] < grafo->nro_vertices) {
          if (pontos <= 0) {
            printf("\nVoce nao pode ir para nenhum lugar! Recomecando do "
                   "inicio!\n");

            // Coloca o destino como 0, e depois coloca o atual como o destino,
            // para que volte do começo.
            destino = 0;
            atual = destino;
            // Coloca i, index e pontos como 0 para recomeçar a area.
            i = 0;
            indexAux = 0;
            pontos = 0;
            // Passa por cada espaço de visitados e os reseta para 0, assim
            // zerando o vetor de visitados.
            for (int aux = 0; aux < grafo->nro_vertices; aux++) {
              visitados[aux] = 0;
              ultimoVisitado[aux] = 0;
            }
          } else {

            // Diminui um ponto
            pontos--;

            // Diminui o index aux para representar que voltou uma sala.
            indexAux--;
            printf("\nVoltando para o último vértice visitado: %d\n\n",
                   (ultimoVisitado[indexAux] + 1));

            // Colocando o atual como o ultimo visitado.
            atual = ultimoVisitado[indexAux];
          }
        } else {
          printf("\nNão há último vértice visitado para voltar.\n\n");
        }
      } else {
        // Tratamento do erro de escolher destino 0.
        if (destino == -2 || existeAresta(grafo, atual, destino) != 0) {
          printf("\nO caminho desejado nao existe, escolha outro destino que "
                 "exista!\n\n");
        }

        // Caso seja um caminho possível e esteja tudo certo então coloca ele no
        // proximo e o da 1 ponto.
        else {
          // Salva o atual no ultimo visitado.
          ultimoVisitado[indexAux] = atual;
          // Soma o peso da aresta na pontuação final.
          jogador->pontuacao += grafo->pesos[atual][destino];
          // Da um ponto ao jogador
          pontos++;
          // Coloca o atual como destino e aumenta os indeces auxiliares.
          atual = destino;
          indexAux++;
          i++;
        }
      }

      // Condiçao de parada é quando o destino selecionado for a saida.
    } while (destino != saida);
  }

  // Para de calcular o tempo e o salva como o tempo gasto na área.
  gettimeofday(&fim, NULL);
  segundos = (double)(fim.tv_sec - inicio.tv_sec);
  milissegundos = (double)(fim.tv_usec - inicio.tv_usec) / 1000;
  segundos += milissegundos / 1000;
  jogador->tempo_area[*indice] = segundos;

  // Limpa os vetores anteriormente alocados.
  free(visitados);
  free(ultimoVisitado);

  return 0;
}

// Função que anda na arvore, ela é pos fixa o que significa que ela vai até o
// maior elemento primeiro depois o executa, volta quando encontra um pequeno
// ele entra e vai indo até achar um maior e assim vai sempre fazendo os maiores
// primeiro e os menores depois, por fim ele faz a raiz.
int posFixa(ArvBin raiz, jogador *jogador, int *indice, int *area_atual) {
  if (raiz == NULL) {
    return -1;
  }

  posFixa(raiz->esq, jogador, indice, area_atual);
  posFixa(raiz->dir, jogador, indice, area_atual);
  jogarLabirinto(raiz, raiz->grafo, jogador, indice, area_atual);
  printf("Tempo gasto na área %d: %.3f segundos\n", *area_atual + 1,
         jogador->tempo_area[*area_atual]);
  (*indice)++;
  (*area_atual)++;
  if (*indice != 5) {
    printf("\nVoce ja passou pelas areas: ");
    for (int i = 0; i < *indice; i++) {
      printf("%d ", i + 1);
    }
    printf("\n");
  } else if (*indice == 5) {
    printf("Parabens! Voce completou todas as areas!\n");
  }

  return 0;
}

// Cria um jogador.
jogador *criarJogador(char *nome) {
  jogador *player = (jogador *)malloc(sizeof(jogador));

  // Apenas criar as informações do jogador se for diferente de NULL, ou seja a
  // alocação foi correta.
  if (player != NULL) {
    strcpy(player->nome, nome);
    for (int i = 0; i < 5; i++) {
      player->tempo_area[i] = 0;
    }
    player->tempo_total = 0.0;
    player->pontuacao = 0.0;
    return player;
  } else if (player == NULL) {
    printf("\nErro ao alocar memoria para o jogador.\n");
    return NULL;
  }
}

// InsertionSort feito para ordenar pela pontuação.
void insertionSortPontuacao(jogador vetor[], int tamanho) {
  int i, j;
  jogador temp;
  for (i = 1; i < tamanho; i++) {
    temp = vetor[i];
    j = i - 1;
    // Alteração da condição para ordem crescente
    while (j >= 0 && vetor[j].pontuacao > temp.pontuacao) {
      vetor[j + 1] = vetor[j];
      j--;
    }
    vetor[j + 1] = temp;
  }
}

// InsertionSort feito para ordenar pelo tempo total.
void insertionSortTempoTotal(jogador vetor[], int tamanho) {
  int i, j;
  jogador temp;
  for (i = 1; i < tamanho; i++) {
    temp = vetor[i];
    j = i - 1;
    while (j >= 0 && vetor[j].tempo_total > temp.tempo_total) {
      vetor[j + 1] = vetor[j];
      j--;
    }
    vetor[j + 1] = temp;
  }
}

// InsertionSort feito para ordenar pelo nome.
void insertionSortNome(jogador vetor[], int tamanho) {
  int i, j;
  jogador temp;
  for (i = 1; i < tamanho; i++) {
    temp = vetor[i];
    j = i - 1;
    while (j >= 0 && strcmp(vetor[j].nome, temp.nome) > 0) {
      vetor[j + 1] = vetor[j];
      j--;
    }
    vetor[j + 1] = temp;
  }
}

// InsertionSort feito para ordenar pela area.
void insertionSortArea(jogador vetor[], int tamanho, int area) {
  int i, j;
  jogador temp;
  for (i = 1; i < tamanho; i++) {
    temp = vetor[i];
    for (j = i - 1;
         j >= 0 && vetor[j].tempo_area[area - 1] > temp.tempo_area[area - 1];
         j--)
      vetor[j + 1] = vetor[j];
    vetor[j + 1] = temp;
  }
}

// Função que mostra as informações do jogador na tela.
void imprimirJogadores(jogador vetor[], int tamanho, int opcao, int area) {
  switch (opcao) {
  case 1: {
    printf("Lista de Jogadores em ordem alfabetica:\n");
    for (int i = 0; i < tamanho; i++) {
      printf("Nome: %s\n", vetor[i].nome);
      printf("\n");
    }
    break;
  }
  case 2: {
    printf("Ranking baseado em tempo total:\n");
    for (int i = 0; i < tamanho; i++) {
      printf("Nome: %s\n", vetor[i].nome);
      printf("Tempo total: %.3f\n", vetor[i].tempo_total);
      printf("\n");
    }
    break;
  }
  case 3: {
    printf("Ranking baseado no tempo gasto na area %d:\n", area);
    for (int i = 0; i < tamanho; i++) {
      printf("Nome: %s\n", vetor[i].nome);
      printf("Tempo gasto na area %d: %.3f\n", area,
             vetor[i].tempo_area[area - 1]);
      printf("\n");
    }
    break;
  }
  case 4: {
    printf("Ranking baseado na pontuacao:\n");
    for (int i = 0; i < tamanho; i++) {
      printf("Nome: %s\n", vetor[i].nome);
      printf("Pontuacao: %d\n", vetor[i].pontuacao);
      printf("\n");
    }
    break;
  }
  default: {
    printf("Lista de Jogadores:\n");
    for (int i = 0; i < tamanho; i++) {
      printf("Nome: %s\n", vetor[i].nome);
      printf("Tempo total: %.3f\n", vetor[i].tempo_total);
      printf("Tempo em cada area:\n");
      for (int j = 0; j < 10; j++) {
        printf("   Area %d: %.3f segundos\n", j, vetor[i].tempo_area[j]);
      }
      printf("Pontuacao: %d\n", vetor[i].pontuacao);
      printf("\n");
    }
    break;
  }
  }
}

// Importa um banco de dados os 10 ultimos jogadores
int importarBancoDeDados(jogador *jogadores) {
  FILE *players = fopen("jogadores.txt", "r");
  if (players == NULL)
    return -1;

  for (int i = 0; i < 10; i++) {
    fscanf(players, "%s %lf %lf %lf %lf %lf %lf %d", jogadores[i].nome,
           &jogadores[i].tempo_total, &jogadores[i].tempo_area[0],
           &jogadores[i].tempo_area[1], &jogadores[i].tempo_area[2],
           &jogadores[i].tempo_area[3], &jogadores[i].tempo_area[4],
           &jogadores[i].pontuacao);
  }

  fclose(players);
  return 0;
}

// Salva no banco de dados os 10 ultimos jogadores
int salvarBancoDeDados(jogador *jogadores) {
  FILE *players = fopen("jogadores.txt", "w");
  if (players == NULL)
    return -1;

  for (int i = 0; i < 10; i++) {
    fprintf(players, "%s %.3f %.3f %.3f %.3f %.3f %.3f %d\n", jogadores[i].nome,
            jogadores[i].tempo_total, jogadores[i].tempo_area[0],
            jogadores[i].tempo_area[1], jogadores[i].tempo_area[2],
            jogadores[i].tempo_area[3], jogadores[i].tempo_area[4],
            jogadores[i].pontuacao);
  }

  fclose(players);

  return 0;
}