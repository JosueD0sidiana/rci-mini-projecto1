n.c
 * Programa principal p2pnet
 * RCI 2025 - Mini-Projecto 1
 */

#include "p2pnet.h"
#include "server_udp.h"
#include <getopt.h>

// ============================================================
// FUNÇÕES AUXILIARES
// ============================================================

/**
 * Mostrar mensagem de uso
 */
void print_usage(const char *program_name) {
    printf("Uso: %s [-s addr] [-p prport] [-l lnkport] [-n neigh] [-h hc]\n\n", 
           program_name);
    printf("Opções:\n");
    printf("  -s addr     Endereço IP do servidor de peers (padrão: %s)\n", 
           DEFAULT_SERVER_IP);
    printf("  -p prport   Porto UDP do servidor (padrão: %d)\n", 
           DEFAULT_SERVER_PORT);
    printf("  -l lnkport  Porto TCP para ligações overlay (obrigatório para peer)\n");
    printf("  -n neigh    Número máximo de vizinhos N+/N- (padrão: %d)\n", 
           DEFAULT_MAX_NEIGHBORS);
    printf("  -h hc       Hopcount máximo (padrão: %d)\n", 
           DEFAULT_MAX_HOPS);
    printf("\n");
    printf("Modos de execução:\n");
    printf("  Servidor:  %s -p 58000\n", program_name);
    printf("  Peer:      %s -s 192.168.56.21 -p 58000 -l 5000 -n 2 -h 3\n", 
           program_name);
    printf("\n");
}

/**
 * Inicializar configuração com valores padrão
 */
void init_config(config_t *config) {
    strncpy(config->server_ip, DEFAULT_SERVER_IP, INET_ADDRSTRLEN - 1);
    config->server_ip[INET_ADDRSTRLEN - 1] = '\0';
    config->server_port = DEFAULT_SERVER_PORT;
    config->link_port = 0;  // Será definido por -l
    config->max_neighbors = DEFAULT_MAX_NEIGHBORS;
    config->max_hops = DEFAULT_MAX_HOPS;
    config->my_seq = 0;
    config->joined = 0;
}

/**
 * Processar argumentos da linha de comando
 */
int parse_arguments(int argc, char *argv[], config_t *config) {
    int opt;
    int custom_hc = 0;  // Flag para -h customizado
    
    while ((opt = getopt(argc, argv, "s:p:l:n:h:")) != -1) {
        switch (opt) {
            case 's':
                strncpy(config->server_ip, optarg, INET_ADDRSTRLEN - 1);
                config->server_ip[INET_ADDRSTRLEN - 1] = '\0';
                break;
            
            case 'p':
                config->server_port = atoi(optarg);
                if (config->server_port <= 0 || config->server_port > 65535) {
                    ERROR_PRINT("Porto inválido: %s", optarg);
                    return ERROR;
                }
                break;
            
            case 'l':
                config->link_port = atoi(optarg);
                if (config->link_port <= 0 || config->link_port > 65535) {
                    ERROR_PRINT("Porto de ligação inválido: %s", optarg);
                    return ERROR;
                }
                break;
            
            case 'n':
                config->max_neighbors = atoi(optarg);
                if (config->max_neighbors <= 0) {
                    ERROR_PRINT("Número de vizinhos deve ser > 0");
                    return ERROR;
                }
                break;
            
            case 'h':
                custom_hc = 1;
                config->max_hops = atoi(optarg);
                if (config->max_hops <= 0) {
                    ERROR_PRINT("Hopcount deve ser > 0");
                    return ERROR;
                }
                break;
            
            default:
                return ERROR;
        }
    }
    
    return SUCCESS;
}

// ============================================================
// FUNÇÃO PRINCIPAL
// ============================================================

int main(int argc, char *argv[]) {
    config_t config;
    
    // Banner
    printf("\n");
    printf("========================================\n");
    printf("  p2pnet - Peer-to-Peer Overlay Network\n");
    printf("  RCI 2025 - Mini-Projecto 1\n");
    printf("========================================\n\n");
    
    // Inicializar configuração
    init_config(&config);
    
    // Processar argumentos
    if (parse_arguments(argc, argv, &config) != SUCCESS) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    // Determinar modo de operação
    if (config.link_port == 0) {
        // MODO SERVIDOR: apenas servidor UDP
        INFO_PRINT("Modo: Servidor de Peers (UDP)");
        INFO_PRINT("Porto UDP: %d", config.server_port);
        
        // Executar servidor UDP
        if (run_udp_server(config.server_port) != SUCCESS) {
            ERROR_PRINT("Falha ao executar servidor UDP");
            return EXIT_FAILURE;
        }
        
    } else {
        // MODO PEER: aplicação completa (será implementado)
        INFO_PRINT("Modo: Peer");
        INFO_PRINT("Servidor de peers: %s:%d", config.server_ip, config.server_port);
        INFO_PRINT("Porto TCP: %d", config.link_port);
        INFO_PRINT("Max vizinhos: %d", config.max_neighbors);
        INFO_PRINT("Max hops: %d", config.max_hops);
        
        printf("\n[AVISO] Modo peer ainda não implementado\n");
        printf("Por enquanto, apenas o servidor UDP está funcional.\n\n");
        
        // TODO: Implementar aplicação peer completa
        // - Interface de utilizador
        // - Cliente UDP
        // - Servidor/Cliente TCP
        // - etc.
    }
    
    return EXIT_SUCCESS;
}
