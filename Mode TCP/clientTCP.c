#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_BUFFER_SIZE 1224

// Fonction pour afficher le menu et obtenir le choix de l'utilisateur
int displayMenuAndGetChoice() {
    int choice;
    printf("\nMenu :\n");
    printf("1. Obtenir la date et l'heure\n");
    printf("2. Obtenir la liste des fichiers\n");
    printf("3. Obtenir le contenu d'un fichier\n");
    printf("4. Obtenir le temps écoulé\n");
    printf("5. Quitter\n");
    printf("Entrez votre choix : ");
    scanf("%d", &choice);
    getchar(); // Consomme le caractère de nouvelle ligne laissé dans le tampon d'entrée

    return choice;
}

// Fonction pour recevoir et afficher le résultat en provenance du serveur
void receiveAndDisplayResult(int clientSocket) {
    char resultBuffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, resultBuffer, sizeof(resultBuffer), 0);
    
    if (bytesRead > 0) {
        resultBuffer[bytesRead] = '\0'; // Termine la chaîne de caractères reçue
        printf("Résultat du serveur :\n%s\n", resultBuffer);
    } else {
        printf("Erreur lors de la réception du résultat du serveur.\n");
    }
}

// Fonction pour authentifier l'utilisateur
int authenticate(int clientSocket) {
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];

    printf("Entrez le nom d'utilisateur : ");
    scanf("%s", username);
    printf("Entrez le mot de passe : ");
    scanf("%s", password);

    // Envoie les informations d'authentification au serveur
    send(clientSocket, username, strlen(username), 0);
    
    usleep(100000); 
    send(clientSocket, password, strlen(password), 0);

    usleep(100000); 
    // Reçoit le résultat d'authentification du serveur
    char authResult[MAX_BUFFER_SIZE];
    recv(clientSocket, authResult, sizeof(authResult), 0);
    
    // Vérifie si l'authentification a réussi
    if (strcmp(authResult, "AUTH_SUCCESS") == 0) {
        printf("Authentification réussie.\n");
        return 1;
    } else {
        printf("Échec de l'authentification. Sortie...\n");
        return 0;
    }
}

// Fonction pour gérer la sélection du service et l'envoi des requêtes
void handleServiceSelection(int clientSocket) {
    // Authentifie l'utilisateur, quitte si l'authentification échoue
    if (!authenticate(clientSocket)) {
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    int choice;
    do {
        // Affiche le menu et obtient le choix de l'utilisateur
        choice = displayMenuAndGetChoice();

        // Traite la sélection du service
        switch (choice) {
            case 1:
                // Envoie une requête pour obtenir la date et l'heure
                send(clientSocket, "GET_DATETIME", strlen("GET_DATETIME"), 0);
                receiveAndDisplayResult(clientSocket);
                break;
            case 2:
                // Envoie une requête pour obtenir la liste des fichiers
                send(clientSocket, "GET_FILE_LIST", strlen("GET_FILE_LIST"), 0);
                char directoryPath[MAX_BUFFER_SIZE];
                printf("Entrez le chemin du répertoire : ");
                scanf("%s", directoryPath);
                send(clientSocket, directoryPath, strlen(directoryPath), 0);
                receiveAndDisplayResult(clientSocket);
                break;
            case 3:
                // Envoie une requête pour obtenir le contenu d'un fichier
                send(clientSocket, "GET_FILE_CONTENT", strlen("GET_FILE_CONTENT"), 0);
                char filePath[MAX_BUFFER_SIZE];
                printf("Entrez le chemin du fichier : ");
                scanf("%s", filePath);
                send(clientSocket, filePath, strlen(filePath), 0);
                receiveAndDisplayResult(clientSocket);
                break;
            case 4:
                // Envoie une requête pour obtenir le temps écoulé
                send(clientSocket, "GET_ELAPSED_TIME", strlen("GET_ELAPSED_TIME"), 0);
                receiveAndDisplayResult(clientSocket);
                break;
            case 5:
                // Quitte le programme
                printf("Sortie...\n");
                break;
            default:
                // Gère le cas où le choix est invalide
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choice != 5);
}

int main(int argc, char *argv[]) {
    // Vérifie si le nombre d'arguments est correct
    if (argc != 3) {
        fprintf(stderr, "Utilisation : %s <nom_du_serveur> <port_du_serveur>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Obtient le nom du serveur et le port à partir des arguments de la ligne de commande
    const char *serverHostname = argv[1];
    const int serverPort = atoi(argv[2]);

    // Crée une socket client
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    // Configure l'adresse du serveur
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverHostname, &serverAddr.sin_addr) <= 0) {
        perror("Adresse du serveur invalide");
        exit(EXIT_FAILURE);
    }

    // Se connecte au serveur
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    // Gère la sélection du service et les requêtes
    handleServiceSelection(clientSocket);

    // Ferme la socket client
    close(clientSocket);
    return 0;
}

