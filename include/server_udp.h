/**
 * server_udp.h
 * Interface do Servidor UDP (Servidor de Peers)
 * RCI 2025 - Mini-Projecto 1 - Membro A
 */

#ifndef SERVER_UDP_H
#define SERVER_UDP_H

#include "p2pnet.h"

// ============================================================
// FUNÇÕES PÚBLICAS DO SERVIDOR UDP
// ============================================================

/**
 * Inicializar e executar o servidor UDP
 * @param port Porto UDP onde o servidor escuta
 * @return SUCCESS ou ERROR
 */
int run_udp_server(int port);

/**
 * Processar comando REG (registar peer)
 * @param table Tabela de peers
 * @param client_ip IP do cliente
 * @param lnkport Porto TCP do cliente
 * @param response Buffer para resposta
 * @return SUCCESS ou ERROR
 */
int process_reg(peer_table_t *table, const char *client_ip, 
                int lnkport, char *response);

/**
 * Processar comando UNR (des-registar peer)
 * @param table Tabela de peers
 * @param seqnumber Número de sequência do peer
 * @param response Buffer para resposta
 * @return SUCCESS ou ERROR
 */
int process_unr(peer_table_t *table, int seqnumber, char *response);

/**
 * Processar comando PEERS (listar peers)
 * @param table Tabela de peers
 * @param response Buffer para resposta (deve ser grande)
 * @return SUCCESS ou ERROR
 */
int process_peers(peer_table_t *table, char *response);

/**
 * Inicializar tabela de peers
 * @param table Tabela a inicializar
 */
void init_peer_table(peer_table_t *table);

/**
 * Adicionar peer à tabela
 * @param table Tabela de peers
 * @param ip Endereço IP
 * @param port Porto TCP
 * @return Número de sequência atribuído, ou -1 em erro
 */
int add_peer(peer_table_t *table, const char *ip, int port);

/**
 * Remover peer da tabela
 * @param table Tabela de peers
 * @param seq Número de sequência
 * @return SUCCESS ou ERROR
 */
int remove_peer(peer_table_t *table, int seq);

/**
 * Encontrar peer por número de sequência
 * @param table Tabela de peers
 * @param seq Número de sequência
 * @return Ponteiro para peer, ou NULL se não encontrado
 */
peer_t* find_peer(peer_table_t *table, int seq);

/**
 * Imprimir estado da tabela de peers (para debug)
 * @param table Tabela de peers
 */
void print_peer_table(peer_table_t *table);

#endif // SERVER_UDP_H/**
 * server_udp.h
 * Interface do Servidor UDP (Servidor de Peers)
 * RCI 2025 - Mini-Projecto 1 - Membro A
 */

#ifndef SERVER_UDP_H
#define SERVER_UDP_H

#include "p2pnet.h"

// ============================================================
// FUNÇÕES PÚBLICAS DO SERVIDOR UDP
// ============================================================

/**
 * Inicializar e executar o servidor UDP
 * @param port Porto UDP onde o servidor escuta
 * @return SUCCESS ou ERROR
 */
int run_udp_server(int port);

/**
 * Processar comando REG (registar peer)
 * @param table Tabela de peers
 * @param client_ip IP do cliente
 * @param lnkport Porto TCP do cliente
 * @param response Buffer para resposta
 * @return SUCCESS ou ERROR
 */
int process_reg(peer_table_t *table, const char *client_ip, 
                int lnkport, char *response);

/**
 * Processar comando UNR (des-registar peer)
 * @param table Tabela de peers
 * @param seqnumber Número de sequência do peer
 * @param response Buffer para resposta
 * @return SUCCESS ou ERROR
 */
int process_unr(peer_table_t *table, int seqnumber, char *response);

/**
 * Processar comando PEERS (listar peers)
 * @param table Tabela de peers
 * @param response Buffer para resposta (deve ser grande)
 * @return SUCCESS ou ERROR
 */
int process_peers(peer_table_t *table, char *response);

/**
 * Inicializar tabela de peers
 * @param table Tabela a inicializar
 */
void init_peer_table(peer_table_t *table);

/**
 * Adicionar peer à tabela
 * @param table Tabela de peers
 * @param ip Endereço IP
 * @param port Porto TCP
 * @return Número de sequência atribuído, ou -1 em erro
 */
int add_peer(peer_table_t *table, const char *ip, int port);

/**
 * Remover peer da tabela
 * @param table Tabela de peers
 * @param seq Número de sequência
 * @return SUCCESS ou ERROR
 */
int remove_peer(peer_table_t *table, int seq);

/**
 * Encontrar peer por número de sequência
 * @param table Tabela de peers
 * @param seq Número de sequência
 * @return Ponteiro para peer, ou NULL se não encontrado
 */
peer_t* find_peer(peer_table_t *table, int seq);

/**
 * Imprimir estado da tabela de peers (para debug)
 * @param table Tabela de peers
 */
void print_peer_table(peer_table_t *table);

#endif // SERVER_UDP_H
