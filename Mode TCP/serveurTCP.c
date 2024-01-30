#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1224

// Fonction pour envoyer la date et l'heure
void sendDateTime(int clientSocket) {
    time_t currentTime;
    time(&currentTime);
    struct tm *localTime = localtime(&currentTime);

    char datetime[MAX_BUFFER_SIZE];
    strftime(datetime, MAX_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", localTime);

    send(clientSocket, datetime, strlen(datetime), 0);
}

// Fonction pour envoyer la liste des fichiers dans un répertoire
void sendFileList(int clientSocket, const char *directoryPath) {
    DIR *dir = opendir(directoryPath);
    
    if (!dir) {
        perror("Erreur lors de l'ouverture du répertoire");
        send(clientSocket, "Erreur lors de l'ouverture du répertoire", strlen("Erreur lors de l'ouverture du répertoire"), 0);
        return;
    }

    char fileList[MAX_BUFFER_SIZE] = "";
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        strcat(fileList, entry->d_name);
        strcat(fileList, "\n");
    }

    closedir(dir);

    send(clientSocket, fileList, strlen(fileList), 0);
}

// Fonction pour envoyer le contenu d'un fichier
void sendFileContent(int clientSocket, const char *filePath) {
    int file = open(filePath, O_RDONLY);
    if (file == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        send(clientSocket, "Erreur lors de l'ouverture du fichier", strlen("Erreur lors de l'ouverture du fichier"), 0);
    }

    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(file, buffer, sizeof(buffer))) > 0) {
        send(clientSocket, buffer, bytesRead, 0);
    }

    close(file);
}

// Fonction pour envoyer le temps écoulé depuis le début du service
void sendElapsedTime(int clientSocket, time_t startTime) {
    // Obtient le temps actuel
    time_t currentTime;
    time(&currentTime);

    // Calcule le temps écoulé en secondes
    double elapsedTime = difftime(currentTime, startTime);

    // Formate le temps écoulé en une chaîne avec deux décimales
    char elapsedStr[MAX_BUFFER_SIZE];
    snprintf(elapsedStr, MAX_BUFFER_SIZE, "%.2f secondes", elapsedTime);

    // Envoie la chaîne de temps écoulé au client
    ssize_t sentBytes = send(clientSocket, elapsedStr, strlen(elapsedStr), 0);

    // Vérifie les erreurs de la fonction d'envoi
    if (sentBytes == -1) {
        perror("Erreur lors de l'envoi du temps écoulé");
    } else {
        printf("Temps écoulé envoyé avec succès : %s\n", elapsedStr);
    }
}

// Fonction pour gérer le client
void handleClient(int clientSocket) {
    // Obtient le temps de début
    time_t startTime;
    time(&startTime);
    char request[MAX_BUFFER_SIZE];
    ssize_t bytesRead;
    
    // Authentification
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];

    // Reçoit le nom d'utilisateur et le mot de passe du client
    bytesRead = recv(clientSocket, username, sizeof(username), 0);
    if (bytesRead <= 0) {
        perror("Erreur lors de la réception du nom d'utilisateur");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    username[bytesRead] = '\0';
	
    usleep(100000); 
    bytesRead = recv(clientSocket, password, sizeof(password), 0);
    if (bytesRead <= 0) {
        perror("Erreur lors de la réception du mot de passe");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    password[bytesRead] = '\0';

    // Effectue l'authentification
    if ((strcmp(password, "mabrouka") == 0) && (strcmp(username, "ghofrane") == 0)) {
        send(clientSocket, "AUTH_SUCCESS", strlen("AUTH_SUCCESS"), 0);
    } else {
        send(clientSocket, "AUTH_FAILED", strlen("AUTH_FAILED"), 0);
        close(clientSocket);
        exit(EXIT_SUCCESS);
    }

    // Boucle de traitement des requêtes
    while ((bytesRead = recv(clientSocket, request, sizeof(request), 0)) > 0) {
        request[bytesRead] = '\0'; 
        printf("%s\n",request);
        
        // Gère différents types de requêtes
        if (strcmp(request, "GET_DATETIME") == 0) {
            sendDateTime(clientSocket);
        } else if (strcmp(request, "GET_FILE_LIST") == 0) {
            char directoryPath[MAX_BUFFER_SIZE];
            bytesRead = recv(clientSocket, directoryPath, sizeof(directoryPath), 0);
            if (bytesRead > 0) {
                directoryPath[bytesRead] = '\0'; 
                sendFileList(clientSocket, directoryPath); 
            }
        } else if (strcmp(request, "GET_FILE_CONTENT") == 0) {
            char filePath[MAX_BUFFER_SIZE];
            printf("%s",filePath);
            bytesRead = recv(clientSocket, filePath, sizeof(filePath), 0);
            if (bytesRead > 0) {
                filePath[bytesRead] = '\0'; 
                sendFileContent(clientSocket, filePath);
            }
        } else if (strcmp(request, "GET_ELAPSED_TIME") == 0) {
            sendElapsedTime(clientSocket, startTime);
        } else {
            printf("Requête inconnue : %s\n", request);
        }
    }

    close(clientSocket);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    // Vérifie le nombre d'arguments
    if (argc != 2) {
        fprintf(stderr, "Utilisation : %s <port_du_serveur>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Obtient le port du serveur à partir des arguments de la ligne de commande
    const int serverPort = atoi(argv[1]);

    // Crée une socket serveur
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    // Configure l'adresse du serveur
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Lie la socket au port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erreur lors de la liaison de la socket");
        exit(EXIT_FAILURE);
    }

    // Met en écoute la socket
    if (listen(serverSocket, 5) == -1) {
        perror("Erreur lors de l'écoute");
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d...\n", serverPort);

    // Boucle principale du serveur
    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Erreur lors de l'acceptation de la connexion");
            continue;
        }

        pid_t childPid = fork();
        if (childPid == -1) {
            perror("Erreur lors de la création d'un processus enfant");
            close(clientSocket);
            continue;
        } else if (childPid == 0) {
            // Processus enfant
            printf("Nouvel utilisateur connecté au serveur. PID enfant : %d\n", getpid());
            close(serverSocket); // Le processus enfant n'a pas besoin de la socket d'écoute
            handleClient(clientSocket);
        } else {
            close(clientSocket); // Le processus parent n'a pas besoin de la socket connectée
        }
    }

    close(serverSocket);
    return 0;
}

