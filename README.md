# p2pnet - Peer-to-Peer Overlay Network

**Redes de Computadores I - 2025/26**  
**Mini-Projecto 1**  
**ISPTEC - Departamento de Engenharias e Tecnologias**

---

## 📋 Descrição

Aplicação peer-to-peer que implementa uma rede sobreposta (overlay network) para partilha de identificadores de conteúdos. A aplicação permite:

- Registo de peers num servidor central (UDP)
- Estabelecimento de ligações TCP entre peers
- Pesquisa distribuída de identificadores na rede
- Manutenção automática de conectividade

---

## 🏗️ Arquitectura

### Componentes Principais:

1. **Servidor de Peers (UDP)** - Mantém lista de peers registados
2. **Cliente UDP** - Comunicação com servidor de peers
3. **Servidor TCP** - Aceita ligações de overlay
4. **Cliente TCP** - Estabelece ligações de overlay
5. **Módulo de Identificadores** - Gestão de conteúdos locais
6. **Protocolo de Pesquisa** - QRY/FND/NOTFND distribuído

---

## 🚀 Instalação e Configuração

### Pré-requisitos:

- Vagrant >= 2.3.0
- VirtualBox >= 7.0

### Passos:

```bash
# 1. Clonar repositório
git clone <url-do-repositorio>
cd p2pnet

# 2. Iniciar ambiente Vagrant
vagrant up

# 3. Aceder à VM
vagrant ssh

# 4. Compilar projecto
cd /vagrant
make

# 5. Executar
./bin/p2pnet -p 58000 -l 5000 -n 2 -h 3
```

---

## 💻 Utilização

### Sintaxe:

```bash
p2pnet [-s addr] [-p prport] [-l lnkport] [-n neigh] [-h hc]
```

### Parâmetros:

- `-s addr` - Endereço IP do servidor de peers (padrão: 192.168.56.21)
- `-p prport` - Porto UDP do servidor (padrão: 58000)
- `-l lnkport` - Porto TCP para ligações overlay
- `-n neigh` - Número máximo de vizinhos (N+ = N-)
- `-h hc` - Hopcount máximo para pesquisas

### Comandos Interactivos:

| Comando            | Descrição                         |
| ------------------ | --------------------------------- |
| `join`             | Aderir à rede sobreposta          |
| `leave`            | Abandonar a rede                  |
| `show neighbors`   | Listar vizinhos internos/externos |
| `release <seq>`    | Remover vizinho interno           |
| `post <id>`        | Adicionar identificador           |
| `unpost <id>`      | Remover identificador             |
| `list identifiers` | Listar identificadores locais     |
| `search <id>`      | Pesquisar identificador na rede   |
| `exit`             | Sair da aplicação                 |

---

## 🧪 Testes

### Testes Manuais do Servidor UDP:

```bash
# Terminal 1: Iniciar servidor
./bin/p2pnet -p 58000

# Terminal 2: Testar com netcat
echo "REG 5000" | nc -u 192.168.56.21 58000
# Esperado: SQN 1

echo "PEERS" | nc -u 192.168.56.21 58000
# Esperado: LST + lista de peers

echo "UNR 1" | nc -u 192.168.56.21 58000
# Esperado: OK
```

### Testes Automáticos:

```bash
make test
```

---

## 📦 Estrutura de Ficheiros

```
p2pnet/
├── Vagrantfile           # Configuração Vagrant
├── provision.sh          # Script de provisionamento
├── Makefile             # Automação de compilação
├── src/                 # Código fonte
├── include/             # Headers
├── tests/               # Scripts de teste
└── docs/                # Documentação
```

---

## 👥 Equipa

**Grupo:** [Número do Grupo]

| Membro    | Responsabilidade           |
| --------- | -------------------------- |
| ´ Muret ´ | UDP + Infraestrutura       |
| ´ Josué ´ | TCP + Overlay              |
| ´ Joel ´  | Identificadores + Pesquisa |

---

## 🐛 Debugging

```bash
# Executar com gdb
make debug

# Verificar memory leaks
make valgrind

# Limpar ficheiros gerados
make clean
```

---

## 📝 Notas de Desenvolvimento

Ver ficheiros em `docs/`:

- `notas_membro_muret.md` - Servidor/Cliente UDP
- `notas_membro_josue.md` - Overlay TCP
- `notas_membro_joel.md` - Identificadores e Pesquisa
