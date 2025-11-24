# -*- mode: ruby -*-
# vi: set ft=ruby :

#
# Vagrantfile para p2pnet
# RCI 2025 - Mini-Projecto 1
# Peer-to-Peer Overlay Network
#

Vagrant.configure("2") do |config|

  config.ssh.insert_key = false
  config.vbguest.auto_update = false


  config.vm.define "p2pnet-dev" do |p2pnet_config|

    # ============================================================
    # CONFIGURAÇÃO DA BOX BASE
    # ============================================================

    # Ubuntu LTS (trusty amd64)
    p2pnet_config.vm.box = "ubuntu/trusty64"

    # ============================================================
    # CONFIGURAÇÃO DO HOSTNAME
    # ============================================================

    p2pnet_config.vm.hostname = "p2pnet-dev"

    # ============================================================
    # CONFIGURAÇÃO DE REDE
    # ============================================================

    # Rede privada com IP fixo (conforme enunciado)
    # Servidor de peers: 192.168.56.21:58000
    p2pnet_config.vm.network "private_network", ip: "192.168.56.21"

    # Port forwarding para facilitar testes do host
    # UDP: Servidor de peers
    p2pnet_config.vm.network "forwarded_port", guest: 58000, host: 58000, protocol: "udp"

    # TCP: Portos para ligações overlay (exemplos)
    p2pnet_config.vm.network "forwarded_port", guest: 5000, host: 5000, protocol: "tcp"
    p2pnet_config.vm.network "forwarded_port", guest: 5001, host: 5001, protocol: "tcp"
    p2pnet_config.vm.network "forwarded_port", guest: 5002, host: 5002, protocol: "tcp"



    # ============================================================
    # SINCRONIZAÇÃO DE PASTAS
    # ============================================================

    # Sincronizar diretório do projecto
    p2pnet_config.vm.synced_folder ".", "/vagrant",
      owner: "vagrant",
      group: "vagrant",
      mount_options: ["dmode=775,fmode=664"]

    # ============================================================
    # CONFIGURAÇÃO DO PROVIDER (VirtualBox)
    # ============================================================

    p2pnet_config.vm.provider "virtualbox" do |vb|
      # Nome da VM
      vb.name = "p2pnet-dev"

      # Recursos
      vb.memory = "1024"  # 1GB RAM
      vb.cpus = 2         # 2 CPUs

      # Interface gráfica desligada
      vb.gui = false

      # Customizações adicionais
      vb.customize ["modifyvm", :id, "--natdnshostresolver1", "on"]
      vb.customize ["modifyvm", :id, "--natdnsproxy1", "on"]
    end

    # ============================================================
    # PROVISIONAMENTO
    # ============================================================

    # Executar script de provisionamento
    p2pnet_config.vm.provision "shell", path: "provision.sh"

    # Mensagem pós-provisionamento
    p2pnet_config.vm.post_up_message = <<-MSG
      ========================================
        Ambiente p2pnet está pronto!
      ========================================

      Para aceder à VM:
        $ vagrant ssh

      Para compilar o projecto:
        $ cd /vagrant
        $ make

      Para executar o servidor UDP:
        $ ./bin/p2pnet -p 58000

      IP da VM: 192.168.56.21
      Porto UDP padrão: 58000

      ========================================
    MSG
  end
end

# ============================================================
# CONFIGURAÇÃO MULTI-MÁQUINA (OPCIONAL - PARA TESTES)
# ============================================================
#
# Descomente o bloco abaixo para criar múltiplos peers
# para testes da rede sobreposta
#
# Vagrant.configure("2") do |config|
#
#   # Servidor de Peers
#   config.vm.define "server" do |server|
#     server.vm.box = "ubuntu/jammy64"
#     server.vm.network "private_network", ip: "192.168.56.21"
#     server.vm.hostname = "peer-server"
#     server.vm.provision "shell", path: "provision.sh"
#   end
#
#   # Peer 1
#   config.vm.define "peer1" do |peer|
#     peer.vm.box = "ubuntu/jammy64"
#     peer.vm.network "private_network", ip: "192.168.56.31"
#     peer.vm.hostname = "peer1"
#     peer.vm.provision "shell", path: "provision.sh"
#   end
#
#   # Peer 2
#   config.vm.define "peer2" do |peer|
#     peer.vm.box = "ubuntu/jammy64"
#     peer.vm.network "private_network", ip: "192.168.56.32"
#     peer.vm.hostname = "peer2"
#     peer.vm.provision "shell", path: "provision.sh"
#   end
#
# end
