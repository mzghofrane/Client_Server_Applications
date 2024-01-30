#!/bin/bash

# Compilation et liaison du client
gcc clientUDP.c -o clientUDP -Wall

# Compilation et liaison du serveur
gcc serveurUDP.c -o serveurUDP -Wall

echo "Compilation et liaison terminées avec succès."
echo "Vous pouvez maintenant exécuter le client et le serveur avec les commandes suivantes :"
echo "./clientUDP <nom_du_serveur> <port_du_serveur>"
echo "./serveurUDP <port_du_serveur>"



