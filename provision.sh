#!/bin/bash
#
# Script de Provisionamento para p2pnet
# RCI 2025 - Mini-Projecto 1
#

echo "=========================================="
echo "   Provisionamento do Ambiente p2pnet   "
echo "=========================================="

# Atualizar repositórios
echo ""
echo "[1/6] Atualizando repositórios do sistema..."
sudo apt-get update -qq

# Instalar ferramentas de compilação
echo ""
echo "[2/6] Instalando ferramentas de compilação..."
sudo apt-get install -y \
    build-essential \
    gcc \
    make \
    gdb \
    valgrind

# Instalar ferramentas de rede
echo ""
echo "[3/6] Instalando ferramentas de rede..."
sudo apt-get install -y \
    netcat-openbsd \
    telnet \
    net-tools \
    iproute2 \
    iputils-ping \
    tcpdump \
    dnsutils

# Instalar ferramentas de desenvolvimento
echo ""
echo "[4/6] Instalando ferramentas auxiliares..."
sudo apt-get install -y \
    vim \
    tree \
    git \
    curl \
    wget

# Verificar instalações
echo ""
echo "[5/6] Verificando instalações..."
echo "  - GCC: $(gcc --version | head -n1)"
echo "  - Make: $(make --version | head -n1)"
echo "  - GDB: $(gdb --version | head -n1)"
echo "  - Netcat: $(nc -h 2>&1 | head -n1)"

# Criar estrutura de diretórios (se não existir)
echo ""
echo "[6/6] Configurando estrutura do projecto..."
cd /vagrant

# Criar diretórios se não existirem
mkdir -p src include tests docs docs/diagramas bin

# Criar ficheiro .gitignore
cat > .gitignore << 'EOF'
# Executáveis
bin/
p2pnet
*.o
*.out

# Ficheiros temporários
*.swp
*.swo
*~
.DS_Store

# Vagrant
.vagrant/

# Logs
*.log

# Core dumps
core
vgcore.*
EOF

# Configurar permissões
chmod +x tests/*.sh 2>/dev/null || true

echo ""
echo "=========================================="
echo "   Provisionamento concluído com sucesso!"
echo "=========================================="
echo ""
echo "Próximos passos:"
echo "  1. vagrant ssh"
echo "  2. cd /vagrant"
echo "  3. make"
echo ""
echo "Para testar o servidor UDP (após implementação):"
echo "  ./bin/p2pnet -p 58000"
echo ""