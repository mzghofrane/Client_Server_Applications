#!/bin/bash

# Compilation et liaison du client
gcc clientTCP.c -o clientTCP -Wall

# Compilation et liaison du serveur
gcc serveurTCP.c -o serveurTCP -Wall

echo "Compilation et liaison terminées avec succès."
echo "Vous pouvez maintenant exécuter le client et le serveur avec les commandes suivantes :"
echo "./clientTCP <nom_du_serveur> <port_du_serveur>"
echo "./serveurTCP <port_du_serveur>"


