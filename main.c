#include "tadlabirinto.h" // TAD do programa.
#include <stdio.h>        // Biblioteca padrão de entrada e saida.
#include <stdlib.h>       // Biblioteca padrão do C.

int main() {
  int opcao, opcaoR;
  Grafo *labirinto = NULL;
  ArvBin arvore = NULL;
  jogador *jogador = NULL;
  char nome[50];
  struct jogador jogadores[10];

  // Importa o banco de dados para o vetor jogadores.
  importarBancoDeDados(jogadores);

  // Pede o nome do jogador atual
  printf("\nDigite seu nome para participar do ranking!\n");
  fflush(stdin);
  fgets(nome, 50, stdin);
  int tamNome = strlen(nome);
  nome[tamNome - 1] = '\0';

  // Cria jogador
  jogador = criarJogador(nome);

  // Menu do jogo, apresenta as opcoes e espera pela opcao do jogador
  do {
    printf("\n===== Menu do Jogo de Labirinto =====\n");
    printf("1. Criar Labirinto\n");
    printf("2. Jogar Labirinto\n");
    printf("3. Ver placar\n");
    printf("4. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
    case 1: // Jogador escolhe criar um labirinto

      // Se a arvore já existe a finaliza
      if (arvore != NULL) {
        finalizarArvoreBinaria(arvore);
        arvore = NULL;
      }

      // Dificuldade maxima é 10 e minima é 5
      int dificuldade = 10;

      // Anda pela dificuldade e cria o labirinto
      for (int i = 0; i < 5; i++) {
        // Como a raiz é um grafo ponderado, então faz com que o criar grafo
        // crie ponderado caso o i seja 0, ou seja a raiz da arvore
        if (i == 0)
          labirinto = criarGrafo(1, dificuldade - 1, 4);
        else
          labirinto = criarGrafo(0, dificuldade - 1, 4);

        // Se labirinto for NULL deu algum erro de criação (Provavelmente
        // alocação de memória)
        if (labirinto == NULL) {
          printf("Erro ao criar labirinto!\n");
        } else {

          // Caso não tenha dado erro gera um grafo e insere ele na arvore
          geraAreas(labirinto);
          arvore = inserirNo(arvore, labirinto);
        }

        // Checagem para parar o loop quando a dificuldade for 5, pode ser
        // ajustado para que tenha uma arvore com mais ou menos nós.
        if (dificuldade < 5)
          break;
        dificuldade--;
      }

      printf("Áreas do labirinto geradas com sucesso!\n");

      break;
    case 2:

      // Jogador escolhe jogar o labirinto que foi gerado
      if (labirinto == NULL || arvore == NULL) {
        printf("Crie o labirinto e gere as áreas primeiro!\n");
      } else {

        // Toda lógica de jogo está em labirinto.c se refira a ele para enteder
        // o que acontece, o jogo começa na chamade de posFixa.
        int indice = 0;
        int area_atual = 0;
        posFixa(arvore, jogador, &indice, &area_atual);

        for (int i = 0; i < 5; i++)
          jogador->tempo_total += jogador->tempo_area[i];
        printf("\nTempo total gasto nas áreas: %.3f segundos\n",
               jogador->tempo_total);
      }

      jogadores[9] = *jogador;

      break;
    case 3:

      // Jogador escolheu ver o ranking, toda lógica de ordenaçao esta em
      // labirinto.c se refira a ele para mais detalhes
      do {
        printf("\n===== Como deseja rankear os jogadores? =====\n");
        printf("1. Ordem alfabética\n");
        printf("2. Tempo total\n");
        printf("3. Tempo gasto em uma determinada área\n");
        printf("4. Pontuacao\n");
        printf("5. Voltar ao menu do labirinto\n");
        scanf("%d", &opcaoR);

        if (opcaoR == 1) {
          insertionSortNome(jogadores, 10);
          imprimirJogadores(jogadores, 10, opcaoR, NULL);
        }
        if (opcaoR == 2) {
          insertionSortTempoTotal(jogadores, 10);
          imprimirJogadores(jogadores, 10, opcaoR, NULL);
        }
        if (opcaoR == 3) {
          printf("\nQual area rankear os jogadores?\n");
          int areaRanking;
          scanf("%d", &areaRanking);
          insertionSortArea(jogadores, 10, areaRanking);
          imprimirJogadores(jogadores, 10, opcaoR, areaRanking);
        }
        if (opcaoR == 4) {
          insertionSortPontuacao(jogadores, 10);
          imprimirJogadores(jogadores, 10, opcaoR, NULL);
        }
        if (opcaoR == 5) {
          printf("\nVoltando ao menu principal\n\n");
          opcao = 0;
          break;
        }
      } while (opcaoR != 5);
      break;
    case 4:

      // Jogador escolhe sair do jogo, finaliza a arvore toda, salva o banco de
      // dados e fecha.
      printf("Encerrando o jogo...\n");
      if (arvore != NULL) {
        finalizarArvoreBinaria(arvore);
      }

      salvarBancoDeDados(jogadores);

      break;
    default:
      printf("Opção inválida! Escolha novamente.\n");
      break;
    }
  } while (opcao != 4);

  return 0;
}
