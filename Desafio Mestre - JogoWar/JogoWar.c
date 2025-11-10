#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

// -- Constantes Globais --
#define TAM_NOME 30
#define TAM_COR 10
#define MAX_MISSAO_LEN 100 
#define NUM_JOGADORES 2    
#define MAX_MISSOES 4 

// ----------------------------------------------------------------------
// -- Definição das Estruturas (Structs) --
// ----------------------------------------------------------------------

struct Territorio {
    char nome[TAM_NOME];
    char cor[TAM_COR];
    int tropas;
};
typedef struct Territorio Territorio; 

struct Jogador {
    char cor[TAM_COR]; 
    char *missao;      
    char alvo_missao[TAM_NOME];
};
typedef struct Jogador Jogador;

// -- Vetor de Missões --
char* MISSOES[MAX_MISSOES] = {
    "Dominar %s e manter a posse.", 
    "Eliminar todas as tropas da cor %s.", 
    "Dominar 5 territorios no total.", 
    "Terminar o turno com 10 ou mais tropas em um unico territorio." 
};


// ----------------------------------------------------------------------
// -- Funções de Utilidade e Memória --
// ----------------------------------------------------------------------

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void inicializar_dados_aleatorios() {
    srand(time(NULL));
}

Territorio* alocar_mapa(int *tamanho_maximo) {
    int n;
    printf("\n-------------------------------------\n");
    printf("--- Configuracao Inicial do Mapa ---\n");
    printf("-------------------------------------\n");
    printf("Digite o numero maximo de territorios (Tamanho do Mapa): ");
    
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("[AVISO] Tamanho invalido. Usando o padrao de 5.\n");
        n = 5;
    }
    limparBufferEntrada();
    
    Territorio* mapa = (Territorio*) calloc(n, sizeof(Territorio));
    
    if (mapa == NULL) {
        perror("Falha na alocacao de memoria");
        exit(EXIT_FAILURE);
    }
    
    *tamanho_maximo = n; 
    printf("Mapa alocado com sucesso para %d territorios.\n", n);
    return mapa;
}

void liberar_memoria(Territorio* mapa, Jogador jogadores[]) {
    if (mapa != NULL) {
        free(mapa);
        printf("\nMemoria do mapa liberada com sucesso.\n");
    }
    
    for (int i = 0; i < NUM_JOGADORES; i++) {
        if (jogadores[i].missao != NULL) {
            free(jogadores[i].missao);
        }
    }
}

// ----------------------------------------------------------------------
// -- Funções de Missão --
// ----------------------------------------------------------------------

/**
 * @brief Sorteia uma missão e, se for de conquista ou eliminação, define um alvo baseado no mapa.
 */
void atribuirMissao(Jogador *jogador, char* missoes[], int totalMissoes, Territorio* mapa, int total_cadastrado) {
    if (total_cadastrado < 2 || jogador->cor[0] == '\0') return;

    int indice_missao = rand() % totalMissoes;
    
    // --- Missão de Alvo Específico (Índice 0) ---
    if (indice_missao == 0) { 
        int indice_alvo;
        Territorio* alvo = NULL;
        int tentativas = 0;
        
        do {
            indice_alvo = rand() % total_cadastrado;
            alvo = mapa + indice_alvo;
            tentativas++;
        } while ((strcmp(alvo->cor, jogador->cor) == 0 || alvo->tropas == 0) && tentativas < total_cadastrado * 2); 
        
        if (tentativas < total_cadastrado * 2) {
             strcpy(jogador->alvo_missao, alvo->nome);
             sprintf(jogador->missao, missoes[indice_missao], alvo->nome);
        } else {
            indice_missao = 2; 
        }
    } 
    
    // --- Missão de Eliminação de Cor (Índice 1) ---
    if (indice_missao == 1) { 
        char cor_alvo[TAM_COR] = "";
        int tentativas = 0;
        
        do {
            int indice_t = rand() % total_cadastrado;
            Territorio* t = mapa + indice_t;
            
            if (strcmp(t->cor, jogador->cor) != 0) {
                strcpy(cor_alvo, t->cor);
                break;
            }
            tentativas++;
        } while (tentativas < total_cadastrado * 2);

        if (cor_alvo[0] != '\0') {
            strcpy(jogador->alvo_missao, ""); 
            sprintf(jogador->missao, missoes[indice_missao], cor_alvo);
        } else {
            indice_missao = 2; 
        }
    }

    // --- Outras Missões (Posse e Tropas) ---
    if (indice_missao == 2 || indice_missao == 3) {
        strcpy(jogador->missao, missoes[indice_missao]);
        strcpy(jogador->alvo_missao, ""); 
    }
}

void exibirMissao(char* missao, char* cor) {
    printf("\n--- SUA MISSAO (Exercito %s) ---\n", cor);
    printf("%s\n", missao); 
}

/**
 * @brief Avalia se a missão do jogador foi cumprida. Retorna 1 se sim, 0 se não.
 */
int verificarMissao(Jogador *jogador_atual, Territorio* mapa, int total_cadastrado) {
    char* missao = jogador_atual->missao;
    char* cor_jogador = jogador_atual->cor;
    
    int territorios_do_jogador = 0;
    for (int i = 0; i < total_cadastrado; i++) {
        if (strcmp((mapa + i)->cor, cor_jogador) == 0) {
            territorios_do_jogador++;
        }
    }

    // --- 1. Conquista de Alvo Específico ---
    if (jogador_atual->alvo_missao[0] != '\0' && strstr(missao, "Dominar") != NULL) {
        for (int i = 0; i < total_cadastrado; i++) {
            Territorio* t = mapa + i;
            if (strcmp(t->nome, jogador_atual->alvo_missao) == 0 && strcmp(t->cor, cor_jogador) == 0) {
                return 1; // Cumprida
            }
        }
        return 0;
    }
    
    // --- 2. Eliminação de Cor ---
    if (strstr(missao, "Eliminar todas as tropas da cor") != NULL) {
        char cor_alvo[TAM_COR] = "";
        char* p_cor = strstr(missao, "cor ");

        if (p_cor != NULL) {
            strncpy(cor_alvo, p_cor + 4, TAM_COR - 1);
            cor_alvo[TAM_COR - 1] = '\0';
            size_t len = strlen(cor_alvo);
            if (len > 0 && cor_alvo[len - 1] == '.') {
                cor_alvo[len - 1] = '\0';
            }
        }

        int tropas_alvo = 0;
        for (int i = 0; i < total_cadastrado; i++) {
            if (strcmp((mapa + i)->cor, cor_alvo) == 0) {
                tropas_alvo += (mapa + i)->tropas;
            }
        }
        return (tropas_alvo == 0); 
    }

    // --- 3. Domínio de 5 Territórios ---
    if (strstr(missao, "Dominar 5 territorios") != NULL) {
        return (territorios_do_jogador >= 5);
    }
    
    // --- 4. 10+ Tropas em 1 Território ---
    if (strstr(missao, "10 ou mais tropas") != NULL) {
        int max_tropas_territorio = 0;
        for (int i = 0; i < total_cadastrado; i++) {
            if (strcmp((mapa + i)->cor, cor_jogador) == 0) {
                if ((mapa + i)->tropas > max_tropas_territorio) {
                    max_tropas_territorio = (mapa + i)->tropas;
                }
            }
        }
        return (max_tropas_territorio >= 10);
    }
    
    return 0; 
}

/**
 * @brief Exibe o status da missão atual.
 */
void consultarProgressoMissao(Jogador *jogador_atual, Territorio* mapa, int total_cadastrado) {
    printf("\n--- VERIFICACAO DA MISSAO ---\n");
    if (total_cadastrado < 2 || jogador_atual->cor[0] == '\0' || strstr(jogador_atual->missao, "%s")) {
        printf("Missao nao definida ou mapa incompleto. Cadastre mais territorios.\n");
        return;
    }

    exibirMissao(jogador_atual->missao, jogador_atual->cor);

    if (verificarMissao(jogador_atual, mapa, total_cadastrado)) {
        printf("\nSTATUS: %s Parabens! A sua missao esta CUMPRIDA! %s\n", "✅", "🎉");
    } else {
        printf("\nSTATUS: %s A sua missao AINDA NAO FOI CUMPRIDA. Continue jogando! %s\n", "❌", "⚔️");
    }
    printf("-------------------------------\n");
}


// ----------------------------------------------------------------------
// -- Funções de Jogo --
// ----------------------------------------------------------------------

void cadastrar_territorio(Territorio* mapa, int tamanho_maximo, int* total_cadastrado, Jogador jogadores[]) {
    printf("\n-------------------------------------\n");
    printf("--- Cadastro de Novo Territorio ---\n");
    printf("-------------------------------------\n");

    if (*total_cadastrado < tamanho_maximo) {
        Territorio* novo = mapa + *total_cadastrado; 
        
        printf("Digite o nome do Territorio: ");
        fgets(novo->nome, TAM_NOME, stdin);
        novo->nome[strcspn(novo->nome, "\n")] = '\0';

        printf("Digite a cor do Exercito: ");
        fgets(novo->cor, TAM_COR, stdin);
        novo->cor[strcspn(novo->cor, "\n")] = '\0';
        
        printf("Digite o numero de Tropas: ");
        scanf("%d", &novo->tropas);
        limparBufferEntrada(); 

        if (novo->tropas <= 0) {
            printf("[AVISO] Tropas ajustadas para 1 (minimo).\n");
            novo->tropas = 1;
        }

        (*total_cadastrado)++; 
        printf("\nTerritorio cadastrado com sucesso! (%d de %d)\n", *total_cadastrado, tamanho_maximo);

        // LÓGICA DE ATRIBUIÇÃO DE COR AO JOGADOR
        if (jogadores[0].cor[0] == '\0' && novo->tropas > 0) {
            strcpy(jogadores[0].cor, novo->cor);
            printf("[JOGADOR 1 DEFINIDO] A cor do Jogador 1 e agora: %s\n", novo->cor);
        } 
        else if (jogadores[1].cor[0] == '\0' && novo->tropas > 0 && strcmp(jogadores[0].cor, novo->cor) != 0) {
            strcpy(jogadores[1].cor, novo->cor);
            printf("[JOGADOR 2 DEFINIDO] A cor do Jogador 2 e agora: %s\n", novo->cor);
        }
    } else {
        printf("Mapa cheio! Nao e possivel cadastrar mais territorios.\n");
    }
}


void listar_territorios(Territorio* mapa, int total_cadastrado) {
    printf("\n==================== MAPA DO MUNDO ====================\n");

    if (total_cadastrado == 0) {
        printf("Nenhum Territorio cadastrado ainda.\n");
    } else {
        for (int i = 0; i < total_cadastrado; i++) {
            Territorio* atual = mapa + i; 
            printf("%d. %s (Exercito: %s , Tropas: %d)\n", 
                   i + 1, 
                   atual->nome, 
                   atual->cor, 
                   atual->tropas);
        }
    }
    printf("========================================================\n");
}


void atacar(Territorio* atacante, Territorio* defensor) {
    if (atacante->tropas < 2) {
        printf("\n[FALHA] O %s deve ter no minimo 2 tropas.\n", atacante->nome);
        return;
    }
    
    if (strcmp(atacante->cor, defensor->cor) == 0) {
        printf("\n[FALHA] Nao e possivel atacar um territorio da propria cor (%s).\n", atacante->cor);
        return;
    }

    int dado_ataque = (rand() % 6) + 1;
    int dado_defesa = (rand() % 6) + 1;

    printf("\n--- RESULTADO DA BATALHA ---\n");
    printf("Ataque (%s): %d | Defesa (%s): %d\n", atacante->nome, dado_ataque, defensor->nome, dado_defesa);

    if (dado_ataque > dado_defesa) {
        defensor->tropas -= 1;
        printf("VITORIA DO ATAQUE! O defensor (%s) perdeu 1 tropa. Tropas restantes: %d.\n", defensor->cor, defensor->tropas);
        
    } else {
        atacante->tropas -= 1;
        printf("VITORIA DA DEFESA! O atacante (%s) perdeu 1 tropa. Tropas restantes: %d.\n", atacante->cor, atacante->tropas);
    }
    
    if (atacante->tropas < 1) {
         atacante->tropas = 1; 
    }
    
    if (defensor->tropas <= 0) {
        printf("\n*** CONQUISTA! O exercito %s eliminou o defensor %s! ***\n", atacante->cor, defensor->nome);
        
        strcpy(defensor->cor, atacante->cor);
        
        int tropas_movidas = (atacante->tropas > 1) ? (atacante->tropas - 1) : 1;
        
        atacante->tropas -= tropas_movidas;
        defensor->tropas = tropas_movidas; 
        
        printf("Tropas transferidas: %d. %s agora pertence ao exercito %s com %d tropas.\n", tropas_movidas, defensor->nome, defensor->cor, defensor->tropas);
    }
}


void fluxo_ataque(Territorio* mapa, int total_cadastrado) {
    int escolha_atacante, escolha_defensor;
    int indice_atacante, indice_defensor;
    
    printf("\n--- FASE DE ATAQUE ---\n"); 
    
    if (total_cadastrado < 2) {
        printf("E necessario ter pelo menos 2 territorios cadastrados para atacar.\n");
        return;
    }
    
    printf("Escolha o territorio atacante (1 a %d): ", total_cadastrado);
    if (scanf("%d", &escolha_atacante) != 1 || escolha_atacante < 1 || escolha_atacante > total_cadastrado) {
        printf("Selecao de atacante invalida.\n");
        limparBufferEntrada();
        return;
    }
    indice_atacante = escolha_atacante - 1; 

    printf("Escolha o territorio defensor (1 a %d): ", total_cadastrado);
    if (scanf("%d", &escolha_defensor) != 1 || escolha_defensor < 1 || escolha_defensor > total_cadastrado) {
        printf("Selecao de defensor invalida.\n");
        limparBufferEntrada();
        return;
    }
    indice_defensor = escolha_defensor - 1; 
    
    limparBufferEntrada();
    
    if (indice_atacante == indice_defensor) {
        printf("Nao e possivel atacar a si mesmo.\n");
        return;
    }

    Territorio* atacante = mapa + indice_atacante;
    Territorio* defensor = mapa + indice_defensor;
    
    atacar(atacante, defensor);
}


// ----------------------------------------------------------------------
// -- FUNÇÃO PRINCIPAL --
// ----------------------------------------------------------------------

int main() {
    Territorio* mapa = NULL; 
    int tamanho_maximo = 0;
    int total_cadastrado = 0; 
    int opcao;
    int jogador_atual = 0; 
    int vencedor = -1;     
    int sair_jogo = 0; 

    inicializar_dados_aleatorios();

    mapa = alocar_mapa(&tamanho_maximo);
    
    // Inicialização dos Jogadores: Cor vazia no início
    Jogador jogadores[NUM_JOGADORES];

    strcpy(jogadores[0].cor, ""); 
    jogadores[0].missao = (char*) malloc(MAX_MISSAO_LEN * sizeof(char));
    strcpy(jogadores[0].missao, ""); 
    strcpy(jogadores[0].alvo_missao, ""); 
    
    strcpy(jogadores[1].cor, ""); 
    jogadores[1].missao = (char*) malloc(MAX_MISSAO_LEN * sizeof(char));
    strcpy(jogadores[1].missao, ""); 
    strcpy(jogadores[1].alvo_missao, ""); 

    printf("\n>>> Por favor, cadastre pelo menos 2 territorios com cores diferentes para comecar o jogo (Opcao 2). <<<\n");
    
    do {
        
        // 1. Atribuição de Missões Tardia e Condicional
        if (total_cadastrado >= 2) {
            
            // Atribuição JOGADOR 0
            if (jogadores[0].cor[0] != '\0' && (jogadores[0].missao[0] == '\0' || strstr(jogadores[0].missao, "%s"))) {
                 atribuirMissao(&jogadores[0], MISSOES, MAX_MISSOES, mapa, total_cadastrado);
                 if (jogadores[0].cor[0] != '\0' && !strstr(jogadores[0].missao, "%s")) {
                    printf("\n!!! MISSAO SECRETA ATRIBUIDA/ATUALIZADA PARA O JOGADOR %s!!!\n", jogadores[0].cor);
                 }
            }
            // Atribuição JOGADOR 1
            if (jogadores[1].cor[0] != '\0' && (jogadores[1].missao[0] == '\0' || strstr(jogadores[1].missao, "%s"))) {
                 atribuirMissao(&jogadores[1], MISSOES, MAX_MISSOES, mapa, total_cadastrado);
                 if (jogadores[1].cor[0] != '\0' && !strstr(jogadores[1].missao, "%s")) {
                    printf("\n!!! MISSAO SECRETA ATRIBUIDA/ATUALIZADA PARA O JOGADOR %s!!!\n", jogadores[1].cor);
                 }
            }
        }
        
        // 2. Exibir MAPA DO MUNDO
        listar_territorios(mapa, total_cadastrado);
        
        // 3. Exibir MISSAO (Condicional)
        if (jogadores[jogador_atual].cor[0] != '\0' && total_cadastrado >= 2 && !strstr(jogadores[jogador_atual].missao, "%s")) {
            exibirMissao(jogadores[jogador_atual].missao, jogadores[jogador_atual].cor);
        } else {
            printf("\n--- SUA MISSAO ---\n"); 
            printf("MISSAO PENDENTE: Cadastre territorios (Opcao 2) para definir seu Exercito e sortear a Missao!\n"); 
        }
        
        // 4. Exibir MENU DE ACOES (Turno Condicional)
        char turno_str[TAM_COR + 15]; 
        if (jogadores[jogador_atual].cor[0] != '\0') {
            sprintf(turno_str, "Turno do %s", jogadores[jogador_atual].cor);
        } else {
            strcpy(turno_str, "Acoes");
        }
        
        printf("\n--- MENU DE ACOES (%s) ---\n", turno_str);
        printf("1 - Atacar\n");
        printf("2 - Consultar Status do Mapa/Cadastrar\n");
        printf("3 - Verificar Missao\n"); // NOVA OPÇÃO
        printf("0 - Encerrar Turno (Passa a vez)\n");
        printf("9 - SAIR DO JOGO (Encerramento total)\n");
        printf("Escolha sua acao: ");

        if (scanf("%d", &opcao) != 1) {
            opcao = -1;
        }
        limparBufferEntrada();

        switch (opcao)
        {
        case 1: // ATACAR
            if (total_cadastrado < 2) {
                 printf("[ERRO] E necessario cadastrar pelo menos 2 territorios para atacar.\n");
                 break;
            }
            fluxo_ataque(mapa, total_cadastrado);
            break;

        case 2: // CADASTRO / LISTAR
            printf("\n--- FUNCOES DE SUPORTE ---\n");
            printf("1 - Cadastrar novo Territorio\n");
            printf("2 - Listar todos os Territorios\n");
            printf("Escolha: ");
            int sub_opcao;
            scanf("%d", &sub_opcao);
            limparBufferEntrada();
            
            if (sub_opcao == 1) {
                 cadastrar_territorio(mapa, tamanho_maximo, &total_cadastrado, jogadores);
            } else if (sub_opcao == 2) {
                 listar_territorios(mapa, total_cadastrado); 
            } else {
                printf("Opcao invalida.\n");
            }
            break;

        case 3: // VERIFICAR MISSÃO (NOVO)
            consultarProgressoMissao(&jogadores[jogador_atual], mapa, total_cadastrado);
            break;
            
        case 0: // ENCERRAR TURNO
            printf("\nEncerrando turno do Jogador %s...\n", jogadores[jogador_atual].cor[0] != '\0' ? jogadores[jogador_atual].cor : "atual");
            break;

        case 9: // SAIR DO JOGO
            printf("\nOpcao de SAIDA DEFINITIVA selecionada.\n");
            sair_jogo = 1; 
            break;
        
        default:
            printf("\nOpcao Invalida! Tente novamente.\n");
            break;
        }

        // Troca o jogador e verifica vitória (se houver condições mínimas)
        if ((opcao == 0 || opcao == 1 || opcao == 3) && jogadores[0].cor[0] != '\0' && jogadores[1].cor[0] != '\0') { 
            
            // Verificação de vitória APENAS se a opção for ataque (1) ou se o usuário pediu para verificar (3)
            if ((opcao == 1 || opcao == 3) && verificarMissao(&jogadores[jogador_atual], mapa, total_cadastrado)) {
                vencedor = jogador_atual;
                printf("\n#####################################################\n");
                printf("!!! JOGADOR %s VENCEU CUMPRINDO SUA MISSAO !!!\n", jogadores[vencedor].cor);
                printf("#####################################################\n");
                break; 
            }
            
            // A troca de turno só ocorre se for a opção 0 (Encerrar Turno)
            if (opcao == 0) {
                 jogador_atual = 1 - jogador_atual; 
            }
        }
        
        if (vencedor == -1 && sair_jogo == 0) { 
            printf("\n Pressione Enter para continuar...");
            getchar(); 
        }

    } while (vencedor == -1 && sair_jogo == 0); 

    liberar_memoria(mapa, jogadores);

    return 0; 
}