erver_udp.c
 * Implementação do Servidor UDP (Servidor de Peers)
 * RCI 2025 - Mini-Projecto 1 - Membro A
 */

#include "server_udp.h"

// ============================================================
// FUNÇÕES DE GESTÃO DA TABELA DE PEERS
// ============================================================

/**
 * Inicializar tabela de peers
 */
void init_peer_table(peer_table_t *table) {
    if (!table) return;
    
    table->count = 0;
    table->next_seq = 1;  // Começar em 1
    
    // Marcar todos como inativos
    for (int i = 0; i < MAX_PEERS; i++) {
        table->peers[i].active = 0;
        table->peers[i].seq = 0;
    }
    
    INFO_PRINT("Tabela de peers inicializada");
}

/**
 * Adicionar peer à tabela
 */
int add_peer(peer_table_t *table, const char *ip, int port) {
    if (!table || !ip) return ERROR;
    
    // Verificar se tabela está cheia
    if (table->count >= MAX_PEERS) {
        ERROR_PRINT("Tabela de peers cheia");
        return ERROR;
    }
    
    // Encontrar slot vazio
    int idx = -1;
    for (int i = 0; i < MAX_PEERS; i++) {
        if (!table->peers[i].active) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        ERROR_PRINT("Não foi possível encontrar slot vazio");
        return ERROR;
    }
    
    // Adicionar peer
    strncpy(table->peers[idx].ip, ip, INET_ADDRSTRLEN - 1);
    table->peers[idx].ip[INET_ADDRSTRLEN - 1] = '\0';
    table->peers[idx].port = port;
    table->peers[idx].seq = table->next_seq;
    table->peers[idx].active = 1;
    
    table->count++;
    int assigned_seq = table->next_seq;
    table->next_seq++;
    
    INFO_PRINT("Peer registado: %s:%d #%d", ip, port, assigned_seq);
    
    return assigned_seq;
}

/**
 * Remover peer da tabela
 */
int remove_peer(peer_table_t *table, int seq) {
    if (!table) return ERROR;
    
    // Encontrar peer
    for (int i = 0; i < MAX_PEERS; i++) {
        if (table->peers[i].active && table->peers[i].seq == seq) {
            INFO_PRINT("Peer removido: %s:%d #%d",
                      table->peers[i].ip,
                      table->peers[i].port,
                      table->peers[i].seq);
            
            table->peers[i].active = 0;
            table->count--;
            return SUCCESS;
        }
    }
    
    ERROR_PRINT("Peer #%d não encontrado", seq);
    return ERROR;
}

/**
 * Encontrar peer por número de sequência
 */
peer_t* find_peer(peer_table_t *table, int seq) {
    if (!table) return NULL;
    
    for (int i = 0; i < MAX_PEERS; i++) {
        if (table->peers[i].active && table->peers[i].seq == seq) {
            return &table->peers[i];
        }
    }
    
    return NULL;
}

/**
 * Imprimir estado da tabela
 */
void print_peer_table(peer_table_t *table) {
    if (!table) return;
    
    printf("\n========================================\n");
    printf("  Tabela de Peers (%d activos)\n", table->count);
    printf("========================================\n");
    
    if (table->count == 0) {
        printf("  (vazia)\n");
    } else {
        for (int i = 0; i < MAX_PEERS; i++) {
            if (table->peers[i].active) {
                printf("  #%d: %s:%d\n",
                       table->peers[i].seq,
                       table->peers[i].ip,
                       table->peers[i].port);
            }
        }
    }
    
    printf("========================================\n\n");
}

// ============================================================
// PROCESSAMENTO DE COMANDOS
// ============================================================

/**
 * Processar comando REG
 * Formato: REG <lnkport>
 * Resposta: SQN <seqnumber> ou NOK
 */
int process_reg(peer_table_t *table, const char *client_ip,
                int lnkport, char *response) {
    if (!table || !client_ip || !response) {
        strcpy(response, "NOK\n");
        return ERROR;
    }
    
    // Validar porto
    if (lnkport <= 0 || lnkport > 65535) {
        ERROR_PRINT("Porto inválido: %d", lnkport);
        strcpy(response, "NOK\n");
        return ERROR;
    }
    
    // Adicionar peer
    int seq = add_peer(table, client_ip, lnkport);
    
    if (seq < 0) {
        strcpy(response, "NOK\n");
        return ERROR;
    }
    
    // Construir resposta: SQN seqnumber\n
    snprintf(response, MAX_BUFFER, "SQN %d\n", seq);
    
    return SUCCESS;
}

/**
 * Processar comando UNR
 * Formato: UNR <seqnumber>
 * Resposta: OK ou NOK
 */
int process_unr(peer_table_t *table, int seqnumber, char *response) {
    if (!table || !response) {
        strcpy(response, "NOK\n");
        return ERROR;
    }
    
    int result = remove_peer(table, seqnumber);
    
    if (result == SUCCESS) {
        strcpy(response, "OK\n");
        return SUCCESS;
    } else {
        strcpy(response, "NOK\n");
        return ERROR;
    }
}

/**
 * Processar comando PEERS
 * Formato: PEERS
 * Resposta: LST\n<ip>:<port>#<seq>\n...\n\n
 */
int process_peers(peer_table_t *table, char *response) {
    if (!table || !response) return ERROR;
    
    // Começar com LST
    strcpy(response, "LST\n");
    
    // Adicionar cada peer activo
    for (int i = 0; i < MAX_PEERS; i++) {
        if (table->peers[i].active) {
            char line[128];
            snprintf(line, sizeof(line), "%s:%d#%d\n",
                    table->peers[i].ip,
                    table->peers[i].port,
                    table->peers[i].seq);
            strcat(response, line);
        }
    }
    
    // Adicionar linha vazia final
    strcat(response, "\n");
    
    return SUCCESS;
}

// ============================================================
// LOOP PRINCIPAL DO SERVIDOR UDP
// ============================================================

/**
 * Executar servidor UDP
 */
int run_udp_server(int port) {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[MAX_BUFFER];
    char response[MAX_BUFFER * 10];  // Maior para PEERS
    peer_table_t peer_table;
    
    // Inicializar tabela de peers
    init_peer_table(&peer_table);
    
    // Criar socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        ERROR_PRINT("Erro ao criar socket: %s", strerror(errno));
        return ERROR;
    }
    
    INFO_PRINT("Socket UDP criado");
    
    // Configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // Bind
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        ERROR_PRINT("Erro no bind: %s", strerror(errno));
        close(sockfd);
        return ERROR;
    }
    
    INFO_PRINT("Servidor UDP a escutar na porta %d", port);
    printf("\n========================================\n");
    printf("  Servidor de Peers Activo\n");
    printf("  Porto UDP: %d\n", port);
    printf("========================================\n\n");
    
    // Loop principal
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        client_len = sizeof(client_addr);
        
        // Receber datagrama
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                            (struct sockaddr*)&client_addr, &client_len);
        
        if (n < 0) {
            ERROR_PRINT("Erro no recvfrom: %s", strerror(errno));
            continue;
        }
        
        buffer[n] = '\0';
        
        // Remover newline final se existir
        if (buffer[n-1] == '\n') {
            buffer[n-1] = '\0';
        }
        
        // Obter IP do cliente
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        
        INFO_PRINT("Recebido de %s: \"%s\"", client_ip, buffer);
        
        // Processar comando
        char cmd[MAX_COMMAND];
        int arg;
        
        memset(response, 0, sizeof(response));
        
        if (sscanf(buffer, "%s %d", cmd, &arg) >= 1) {
            if (strcmp(cmd, "REG") == 0) {
                // REG <lnkport>
                process_reg(&peer_table, client_ip, arg, response);
                
            } else if (strcmp(cmd, "UNR") == 0) {
                // UNR <seqnumber>
                process_unr(&peer_table, arg, response);
                
            } else if (strcmp(cmd, "PEERS") == 0) {
                // PEERS
                process_peers(&peer_table, response);
                
            } else {
                ERROR_PRINT("Comando desconhecido: %s", cmd);
                strcpy(response, "NOK\n");
            }
        } else {
            ERROR_PRINT("Formato de comando inválido");
            strcpy(response, "NOK\n");
        }
        
        // Enviar resposta
        sendto(sockfd, response, strlen(response), 0,
               (struct sockaddr*)&client_addr, client_len);
        
        INFO_PRINT("Resposta enviada (%zu bytes)", strlen(response));
        
        // Debug: mostrar tabela
        if (strcmp(cmd, "REG") == 0 || strcmp(cmd, "UNR") == 0) {
            print_peer_table(&peer_table);
        }
    }
    
    close(sockfd);
    return SUCCESS;
}
