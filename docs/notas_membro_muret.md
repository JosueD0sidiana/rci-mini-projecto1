Notas - Servidor UDP
Responsável: Infraestrutura + Servidor de Peers (UDP) + Cliente UDP

#. Progresso no momento: Preso no bug na estutura do servidor udp.

1. Estruturas de Dados Implementadas
1.1 Estrutura peer_t
Representa um peer registado no servidor:
ctypedef struct {
    char ip[INET_ADDRSTRLEN];   // Endereço IP (formato xxx.xxx.xxx.xxx)
    int port;                    // Porto TCP para ligações overlay
    int seq;                     // Número de sequência (ordem de registo)
    int status;                  // Flag: 1 = activo, 0 = removido
} peer_t;
Finalidade: Armazenar informação de cada peer registado.
Implementação: De raiz, usando tipos básicos de C.

1.2 Estrutura peer_table_t
Tabela de peers mantida pelo servidor UDP:
ctypedef struct {
    peer_t peers[MAX_PEERS];     // Array de peers (máx. 100)
    int count;                   // Número de peers activos
    int next_seq;                // Próximo número de sequência
} peer_table_t;
Finalidade: Gestão centralizada de todos os peers registados.
Características:

Suporta até 100 peers simultâneos
Atribuição sequencial de números (1, 2, 3, ...)
Reutiliza slots quando peers são removidos


2. Protocolo UDP Implementado
2.1 Comando REG
Cliente → Servidor:
REG <lnkport>
Servidor → Cliente:
SQN <seqnumber>    (sucesso)
NOK                (erro)
Processamento:

Extrair lnkport do comando
Obter IP do cliente via recvfrom()
Adicionar peer à tabela
Atribuir seqnumber = next_seq++
Responder com SQN <seqnumber>

Casos de erro:

Tabela cheia → NOK
Porto inválido → NOK


2.2 Comando UNR
Cliente → Servidor:
UNR <seqnumber>
Servidor → Cliente:
OK     (sucesso)
NOK    (peer não encontrado)
Processamento:

Procurar peer com seqnumber na tabela
Se encontrado: marcar status = 0, decrementar count
Se não encontrado: responder NOK


2.3 Comando PEERS
Cliente → Servidor:
PEERS
Servidor → Cliente:
LST
<ip>:<port>#<seq>
<ip>:<port>#<seq>
...
[linha vazia]
Exemplo:
LST
192.168.56.31:5000#1
192.168.56.32:5001#2
Processamento:

Iterar por todos os peers activos
Para cada peer: adicionar linha ip:port#seq
Terminar com linha vazia


3. Implementação do Servidor UDP
3.1 Fluxo de Execução
1. Criar socket UDP (socket())
2. Fazer bind ao porto 58000 (bind())
3. Loop infinito:
   a. Receber datagrama (recvfrom())
   b. Extrair IP do cliente
   c. Parsear comando
   d. Processar comando (switch)
   e. Construir resposta
   f. Enviar resposta (sendto())
   g. Log e debug
3.2 Funções Principais
FunçãoDescriçãorun_udp_server()Loop principal do servidorinit_peer_table()Inicializar tabela vaziaprocess_reg()Processar comando REGprocess_unr()Processar comando UNRprocess_peers()Processar comando PEERSadd_peer()Adicionar peer à tabelaremove_peer()Remover peer da tabelafind_peer()Buscar peer por seqnumber

