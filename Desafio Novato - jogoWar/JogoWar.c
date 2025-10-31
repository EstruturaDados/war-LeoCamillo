#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para strcspn()

// -- Constantes Globais --
#define MAX_TERRITORIO 5
#define TAM_NOME 30
#define TAM_COR 10

// -- Definição da Estrutura (Struct) --
struct Territorio {
    char nome[TAM_NOME];
    char cor[TAM_COR];
    int tropas;
};

// -- Função para limpar o buffer de entrada --
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// -- Função Principal do Menu --
int main() {
    struct Territorio area[MAX_TERRITORIO];
    int totalTerritorio = 0;
    int opcao;

    // -- Laço Principal do Mennu --
    do {

        //Exibe o menu de opções. 
        printf("==============================\n");
        printf("     MAPA DOS TERRITORIOS  \n");
        printf("==============================\n");
        printf("1 - Cadastrar novo Territorio\n");
        printf("2 - Listar todos os Territorios\n");
        printf("0 - Sair\n");
        printf("_______________________________\n");
        printf("Escolha uma opcao: ");

        //Lé a opção do usuário.
        scanf("%d", &opcao);
        limparBufferEntrada(); // Limpa o \n deixado pelo scanf.

        // -- Processamento da Opção --
        switch (opcao)
        {
        case 1: // Cadastro de Territorios
            printf("-------------------------------------\n");
            printf("---Cadastro de Novo Territorio---\n");
            printf("-------------------------------------\n");

            if (totalTerritorio < MAX_TERRITORIO) {
                printf("Digite o nome do Territorio: ");
                fgets(area[totalTerritorio].nome, TAM_NOME, stdin);

                printf("Digite a cor do Exercito: ");
                fgets(area[totalTerritorio].cor, TAM_COR, stdin);
                
                area[totalTerritorio].nome[strcspn(area[totalTerritorio].nome, "\n")] = '\0';
                area[totalTerritorio].cor[strcspn(area[totalTerritorio].cor, "\n")] = '\0';

                printf("Digite o numero de Tropas: ");
                scanf("%d", &area[totalTerritorio].tropas);
                limparBufferEntrada();

                totalTerritorio++;

                printf("\nTerritorio cadastrado com sucesso!\n");
            } else {
                printf("Mapa cheio! Nao e possivel cadastrar mais territorios \n");
            }

            printf("\n Pressione Enter para continuar...");
            getchar(); //Pausa para o usuario ler a mensagem antes de voltar ao mennu.
            break;

        case 2: //Listagem de Territorios
            printf("\n----------------------------------------\n");
            printf("#-- Lista de Territorios Cadastrados --#\n");

            if (totalTerritorio == 0) {
                printf("\n-------------------------------------\n");
                printf("Nenhum Territorio cadastrado ainda.\n");
                printf("\n-------------------------------------\n");
            } else {
                for (int i = 0; i < totalTerritorio; i++) {
                    printf("\n-------------------------------------\n");
                    printf("TERRITORIO %d\n", i + 1);
                    printf("- Nome: %s\n", area[i].nome);
                    printf("- Dominado por: Exercito %s\n", area[i].cor);
                    printf("- Tropas: %d\n", area[i].tropas);
                }
                printf("\n-------------------------------------\n");
            }

            // A pausa é crucial para que o susuário veja a lista antes
            // do proximo loop limpar a tela.
            printf("\nPressione Enter para continuar...");
            getchar();
            break;

        case 0: //Sair
            printf("\nSaindo do Sistema...\n");
            break;
        
        default: //Opção Invalida
            printf("\nOpcao Invalida! Tente novamente.\n");
            printf("\nPressione Enter para continuar...");
            getchar();
            break;
        }
    } while (opcao != 0);

    return 0; // Fim do programa.
}
