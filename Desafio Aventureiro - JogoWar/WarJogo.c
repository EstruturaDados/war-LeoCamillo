#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

// -- Constantes Globais --
#define TAM_NOME 30
#define TAM_COR 10

// -- Definição da Estrutura (Struct) --
struct Territorio {
    char nome[TAM_NOME];
    char cor[TAM_COR];
    int tropas;
};

typedef struct Territorio Territorio; 

// ----------------------------------------------------------------------
// -- Funções de Utilidade --
// ----------------------------------------------------------------------

// Função para limpar o buffer de entrada
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Inicializa o gerador de números aleatórios para a simulação de dados
void inicializar_dados_aleatorios() {
    srand(time(NULL));
}

// ----------------------------------------------------------------------
// -- Gerenciamento de Memória --
// ----------------------------------------------------------------------

Territorio* alocar_mapa(int *tamanho_maximo) {
    int n;
    printf("\n-------------------------------------\n");
    printf("--- Configuracao Inicial do Mapa ---\n");
    printf("-------------------------------------\n");
    printf("Digite o numero maximo de territorios (Tamanho do Mapa): ");
    
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("[ERRO] Tamanho invalido. Usando o padrao de 5.\n");
        n = 5;
    }
    limparBufferEntrada();
    
    // Alocação dinâmica (calloc inicializa tudo com zero)
    Territorio* mapa = (Territorio*) calloc(n, sizeof(Territorio));
    
    if (mapa == NULL) {
        perror("Falha na alocacao de memoria");
        exit(EXIT_FAILURE);
    }
    
    *tamanho_maximo = n; 
    printf("Mapa alocado com sucesso para %d territorios.\n", n);
    return mapa;
}

void liberar_memoria(Territorio* mapa) {
    // Gerenciamento de memória (free)
    if (mapa != NULL) {
        free(mapa);
        printf("\nMemoria do mapa liberada com sucesso.\n");
    }
}

// ----------------------------------------------------------------------
// -- Funções de Funcionalidade --
// ----------------------------------------------------------------------

void cadastrar_territorio(Territorio* mapa, int tamanho_maximo, int* total_cadastrado) {
    printf("\n-------------------------------------\n");
    printf("--- Cadastro de Novo Territorio ---\n");
    printf("-------------------------------------\n");

    if (*total_cadastrado < tamanho_maximo) {
        // Uso de ponteiros (acessa a posição livre com aritmetica de ponteiros)
        Territorio* novo = mapa + *total_cadastrado; 
        
        printf("Digite o nome do Territorio: ");
        fgets(novo->nome, TAM_NOME, stdin);
        // Remove o '\n' do final da string
        novo->nome[strcspn(novo->nome, "\n")] = '\0';

        printf("Digite a cor do Exercito: ");
        fgets(novo->cor, TAM_COR, stdin);
        novo->cor[strcspn(novo->cor, "\n")] = '\0';
        
        printf("Digite o numero de Tropas: ");
        scanf("%d", &novo->tropas);
        limparBufferEntrada(); 

        // Garante que o número de tropas seja positivo
        if (novo->tropas <= 0) {
            printf("[AVISO] Tropas ajustadas para 1 (minimo).\n");
            novo->tropas = 1;
        }

        (*total_cadastrado)++; 
        printf("\nTerritorio cadastrado com sucesso! (%d de %d)\n", *total_cadastrado, tamanho_maximo);
    } else {
        printf("Mapa cheio! Nao e possivel cadastrar mais territorios.\n");
    }
}

void listar_territorios(Territorio* mapa, int total_cadastrado) {
    printf("\n----------------------------------------\n");
    printf("#-- Lista de Territorios Cadastrados --#\n");

    if (total_cadastrado == 0) {
        printf("Nenhum Territorio cadastrado ainda.\n");
    } else {
        for (int i = 0; i < total_cadastrado; i++) {
            // Requisito: Uso de ponteiros (acessa a posição i com aritmetica)
            Territorio* atual = mapa + i; 
            printf("\n-------------------------------------\n");
            // Exibe o índice (0, 1, 2...) que o usuário deve usar para ataque/defesa
            printf("TERRITORIO %d (Indice para ataque: %d)\n", i + 1, i);
            printf("- Nome: %s\n", atual->nome);
            printf("- Dominado por: Exercito %s\n", atual->cor);
            printf("- Tropas: %d\n", atual->tropas);
        }
    }
    printf("\n-------------------------------------\n");
}

void atacar(Territorio* atacante, Territorio* defensor) {
    
    // O atacante deve ter pelo menos 2 tropas para iniciar um ataque
    if (atacante->tropas < 2) {
        printf("\n[FALHA] O %s deve ter no minimo 2 tropas (1 para atacar, 1 para defender).\n", atacante->nome);
        return;
    }
    
    // Validar se o jogador nao esta atacando um territorio da propria cor
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("\n[FALHA] Nao e possivel atacar um territorio da propria cor (%s).\n", atacante->cor);
        return;
    }

    // Simulação de rolagem de dados (1 a 6)
    int dado_ataque = (rand() % 6) + 1;
    int dado_defesa = (rand() % 6) + 1;

    printf("\n>>> Simulacao de Batalha: %s vs %s <<<\n", atacante->nome, defensor->nome);
    printf("Atacante (%s) rolou: %d\n", atacante->cor, dado_ataque);
    printf("Defensor (%s) rolou: %d\n", defensor->cor, dado_defesa);

    if (dado_ataque > dado_defesa) {
        // --- Atacante VENCE ---
        printf("\n[VITORIA] O exercito %s conquistou o territorio %s!\n", atacante->cor, defensor->nome);
        
        // Mudar de dono
        strcpy(defensor->cor, atacante->cor);

        // Transferir tropas: Metade das tropas do atacante se movem
        int tropas_movidas = atacante->tropas / 2;
        
        // Garante que o atacante nao fique com 0 (deve sobrar 1 no território atacante)
        if (tropas_movidas == 0 || (atacante->tropas - tropas_movidas) < 1) {
            tropas_movidas = atacante->tropas - 1; // Move o máximo possível (todas menos 1)
        }

        atacante->tropas -= tropas_movidas;
        defensor->tropas = tropas_movidas;
        
        printf("Tropas transferidas: %d. O novo territorio %s agora tem %d tropas.\n", tropas_movidas, defensor->nome, defensor->tropas);
        
    } else {
        // --- Defensor VENCE ou Empate ---
        printf("\n[DERROTA] O exercito %s defendeu o territorio %s!\n", defensor->cor, defensor->nome);

        // Atacante perde 1 tropa
        atacante->tropas -= 1;
        
        // Garante que o atacante sempre fique com pelo menos 1 tropa
        if (atacante->tropas < 1) {
             atacante->tropas = 1; 
             printf("[AVISO] O atacante perdeu todas as tropas, mas permaneceu com 1 para nao perder o territorio.\n");
        }

        printf("O atacante %s perdeu 1 tropa. Agora tem %d tropas.\n", atacante->cor, atacante->tropas);
    }
}

void fluxo_ataque(Territorio* mapa, int total_cadastrado) {
    int indice_atacante, indice_defensor;
    
    printf("\n----------------------------------------\n");
    printf("---------- Simulacao de Ataque ---------\n");
    printf("----------------------------------------\n");
    
    if (total_cadastrado < 2) {
        printf("E necessario ter pelo menos 2 territorios cadastrados para atacar.\n");
        return;
    }
    
    // Lista os territórios com seus índices e dados para facilitar a escolha.
    listar_territorios(mapa, total_cadastrado);
    
    // Seleção do Atacante
    printf("Digite o INDICE do Territorio ATACANTE: ");
    if (scanf("%d", &indice_atacante) != 1 || indice_atacante < 0 || indice_atacante >= total_cadastrado) {
        printf("Selecao de atacante invalida.\n");
        limparBufferEntrada();
        return;
    }
    
    // Seleção do Defensor
    printf("Digite o INDICE do Territorio DEFENSOR: ");
    if (scanf("%d", &indice_defensor) != 1 || indice_defensor < 0 || indice_defensor >= total_cadastrado) {
        printf("Selecao de defensor invalida.\n");
        limparBufferEntrada();
        return;
    }
    
    limparBufferEntrada();
    
    // Validação de índices
    if (indice_atacante == indice_defensor) {
        printf("Nao e possivel atacar a si mesmo.\n");
        return;
    }

    // Passa os ponteiros dos territórios para a função de ataque
    Territorio* atacante = mapa + indice_atacante;
    Territorio* defensor = mapa + indice_defensor;
    
    atacar(atacante, defensor);

    // Exibição pós-ataque
    printf("\n*** Dados Atualizados Apos o Ataque ***\n");
    printf("--- Territorio ATACANTE (%s) ---\n", atacante->nome);
    printf("Cor: %s | Tropas: %d\n", atacante->cor, atacante->tropas);
    printf("--- Territorio DEFENSOR (%s) ---\n", defensor->nome);
    printf("Cor: %s | Tropas: %d\n", defensor->cor, defensor->tropas);
    printf("----------------------------------------\n");
}


// ----------------------------------------------------------------------
// -- Função Principal do Menu --
// ----------------------------------------------------------------------

int main() {
    // Variáveis de controle para o mapa dinâmico
    Territorio* mapa = NULL; 
    int tamanho_maximo = 0;  
    int total_cadastrado = 0; 
    int opcao;

    // Inicializa o gerador de números aleatórios
    inicializar_dados_aleatorios();

    // Alocação dinâmica de memória
    mapa = alocar_mapa(&tamanho_maximo);
    
    // -- Laço Principal do Menu --
    do {
        printf("\n==============================\n");
        printf("     MAPA DOS TERRITORIOS     \n");
        printf("==============================\n");
        printf("1 - Cadastrar novo Territorio\n");
        printf("2 - Listar todos os Territorios\n");
        printf("3 - Simular Ataque entre Territorios\n"); 
        printf("0 - Sair\n");
        printf("_______________________________\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
             opcao = -1;
        }
        limparBufferEntrada();

        // -- Processamento da Opção --
        switch (opcao)
        {
        case 1:
            cadastrar_territorio(mapa, tamanho_maximo, &total_cadastrado);
            break;

        case 2:
            listar_territorios(mapa, total_cadastrado);
            break;

        case 3:
            fluxo_ataque(mapa, total_cadastrado);
            break;
            
        case 0:
            printf("\nSaindo do Sistema...\n");
            break;
        
        default:
            printf("\nOpcao Invalida! Tente novamente.\n");
            break;
        }

        if (opcao != 0) {
            printf("\n Pressione Enter para continuar...");
            getchar(); 
        }

    } while (opcao != 0);

    // Liberação de memória
    liberar_memoria(mapa);

    return 0; // Fim do programa.
}