#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define NMAX 100

int main(int argc, char *argv[]) {

    // Display welcome message
    printf("#######################################################\n");
    printf("                Bienvenu chez le Serveur                \n");
    printf("#######################################################\n\n");
    
    
    // Vérifie si le nombre d'arguments est correct
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Extrait le numéro de port du deuxième argument
    int server_port = atoi(argv[1]);

    // Crée un socket UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Structures pour les adresses du serveur et du client
    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    // Spécifie la famille d'adresses IPv4
    server_addr.sin_family = AF_INET;
    
    // Adresse IP à écouter (INADDR_ANY pour écouter sur toutes les interfaces)
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // Convertit et affecte le numéro de port
    server_addr.sin_port = htons(server_port);

    // Lie le socket à l'adresse spécifiée (Bind)
    if (bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Initialise le générateur de nombres aléatoires avec le temps actuel
    srand((unsigned int)time(NULL));

    // Variable pour stocker le nombre reçu du client
    int n;
    
    // Longueur de l'adresse du client
    socklen_t client_addr_len = sizeof(client_addr);
    
    // Affiche un message indiquant l'attente des données du client
    printf("*** En attente des données du client...\n");
    
    // Réception du nombre envoyé par le client
    if (recvfrom(sockfd, &n, sizeof(n), 0, (struct sockaddr*)&client_addr, &client_addr_len) == -1) {
        perror("Error receiving data from client");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Affiche le nombre reçu du client
    printf("\n=>Données reçues du client : %d\n", n);

     // Envoie n nombres aléatoires au client
    for (int i = 0; i < n; ++i) {
        int random_number = rand() % NMAX + 1;
        printf("- Numéro aléatoire : %d \n", random_number);
        if (sendto(sockfd, &random_number, sizeof(random_number), 0, (struct sockaddr*)&client_addr, client_addr_len) == -1) {
            perror("Error sending data to client");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }

    // Ferme le socket
    close(sockfd);
    return 0;
}

