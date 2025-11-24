#!/usr/bin/env bash
set -e

# Remove ESM repositories that cause errors on Trusty
rm -f /etc/apt/sources.list.d/ubuntu-esm-infra-trusty.list

apt-get update
apt-get install -y python3 python3-pip make


