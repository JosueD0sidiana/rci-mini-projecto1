/**
 * p2pnet.h
 * Definições globais e constantes para p2pnet
 * RCI 2025 - Mini-Projecto 1
 */

#ifndef P2PNET_H
#define P2PNET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define MAX_PEERS 100           // Maximo de peers registados
#define MAX_BUFFER 1024         // Tamanho do buffer de mensagens
#define MAX_COMMAND 64          // Tamanho máximo de um comando
#define MAX_IDENTIFIERS 100     // Maximo de identificadores por peer

#define DEFAULT_SERVER_IP "192.168.56.21"
#define DEFAULT_SERVER_PORT 58000
#define DEFAULT_MAX_NEIGHBORS 2
#define DEFAULT_MAX_HOPS 3

// ** Estrutura que representa um peer registado

typedef struct {
    char ip[INET_ADDRSTRLEN];
    int port;
    int seq;
    int status;                  // Flag: 1 = activo, 0 = inactivo
} peer_t;

// ** Tabela de peers (servidor UDP)

typedef struct {
    peer_t peers[MAX_PEERS];
    int count;
    int next_seq;
} peer_table_t;

// ** Configuração da aplicação p2pnet

typedef struct {
    char server_ip[INET_ADDRSTRLEN];  // IP do servidor de peers
    int server_port;                   // Porto UDP do servidor
    int link_port;                     // Porto TCP para ligações overlay
    int max_neighbors;                 // N+ e N- (assumidos iguais)
    int max_hops;                      // Hopcount máximo
    int my_seq;                        // Número de sequência deste peer
    int joined;                        // Flag: peer está na rede?
} config_t;

// ============================================================
// CÓDIGOS DE RETORNO
// ============================================================

#define SUCCESS 0
#define ERROR -1

// ============================================================
// MACROS ÚTEIS
// ============================================================

#define DEBUG_PRINT(fmt, ...) \
    fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)

#define ERROR_PRINT(fmt, ...) \
    fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

#define INFO_PRINT(fmt, ...) \
    fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)

#endif // P2PNET_H
