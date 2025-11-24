Vagrant.configure("2") do |config|
  config.vm.define "p2pnet-vm" do |p2pnet_vm_config|
    p2pnet_vm_config.vm.box = "ubuntu/trusty64"
    p2pnet_vm_config.vm.hostname = "p2pnet-vm"
    p2pnet_vm_config.vm.network "private_network", ip: "192.168.56.21"
    p2pnet_vm_config.vm.provision "shell", path: "bootstrap.sh"

    p2pnet_vm_config.vm.provider "virtualbox" do |vb|
      # vb.name = "p2pnet-vm"
      # opts = ["modifyvm", :id, "--natdnshostresolver1", "on"]
      # vb.customize opts
      vb.memory = "512"
    end
  end
end
