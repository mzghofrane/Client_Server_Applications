#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define NMAX 10

int main(int argc, char *argv[]) {

    // Display welcome message
    printf("#######################################################\n");
    printf("                Bienvenu chez le Client                \n");
    printf("#######################################################\n\n");
    
    
    // Vérifie si le nombre d'arguments est correct
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Extrait le numéro de port du deuxième argument
    int server_port = atoi(argv[2]);

    // Crée un socket UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Structure pour l'adresse du serveur
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    // Spécifie la famille d'adresses IPv4
    server_addr.sin_family = AF_INET;
    
    // Convertit et affecte le numéro de port
    server_addr.sin_port = htons(server_port);

    // Convertit l'adresse IP du format texte vers le format binaire
    if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) <= 0) {
        perror("Invalid server address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Initialise le générateur de nombres aléatoires avec le temps actuel
    srand((unsigned int)time(NULL));

    // Génère un nombre aléatoire entre 1 et NMAX
    int n = rand() % NMAX + 1;

    // Affiche le nombre qui sera envoyé au serveur
    printf("\n*** Informations liées au Client :\n\t-Le nombre aléatoir à envoyer au serveur = %d \n", n);
    
    // Envoie le nombre généré au serveur
    if (sendto(sockfd, &n, sizeof(n), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de l'envoi des données au serveur");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Déclaration de la variable pour stocker la réponse du serveur
    int response;
    
    // Affiche un message indiquant l'attente de la réponse du serveur
    printf("\nEn attente de réponse du serveur...\n");
    
    // Réception et affichage de la réponse du serveur
    for (int i = 0; i < n; ++i) {
        if (recvfrom(sockfd, &response, sizeof(response), 0, NULL, NULL) == -1) {
            perror("Erreur de réception de la réponse du serveur");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        printf("\n=>Réponse du serveur [%d] : %d\n", i + 1, response);
    }

    // Ferme le socket
    close(sockfd);
    return 0;
}

