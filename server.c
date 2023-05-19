#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int max(int *tab) {
    int maxi = tab[0];
    for (int i = 0; i < 10; i++) {
        if (tab[i] > maxi)
            maxi = tab[i];
    }
    return maxi;
}

int main(int argc, char **argv) {
    int sockfd; // descripteur de socket
    char buf[1024];
    char buf2[1024]; // espace necessaire pour stocker le message recu

    fd_set readfds;
    fd_set readfds2;
    int nb;
    int sockclient[10];

    // taille d'une structure sockaddr_in utile pour la fonction recvfrom
    socklen_t fromlen = sizeof(struct sockaddr_in);

    struct sockaddr_in my_addr; // structure d'adresse qui contiendra les param
                                // reseaux du recepteur
    struct sockaddr_in client[10]; // structure d'adresse qui contiendra les
                                   // param reseaux de l'expediteur

    // verification du nombre d'arguments sur la ligne de commande
    if (argc != 2) {
        printf("Usage: %s port_local\n", argv[0]);
        exit(-1);
    }

    // creation de la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // initialisation de la structure d'adresse du recepteur (pg local)

    // famille d'adresse
    my_addr.sin_family = AF_INET;

    // recuperation du port du recepteur
    my_addr.sin_port = htons(atoi(argv[1]));

    // adresse IPv4 du recepteur
    inet_aton("127.0.0.1", &(my_addr.sin_addr));

    // association de la socket et des param reseaux du recepteur
    if (bind(sockfd, (void *)&my_addr, sizeof(struct sockaddr_in)) != 0) {
        perror("erreur lors de l'appel a bind -> ");
        exit(-2);
    }
    listen(sockfd, 5);

    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    FD_SET(sockfd, &readfds);
    int taille = 0;
    readfds2 = readfds;

    while (1) {

        // reception de la chaine de caracteres
        memset(buf, '\0', 1024);
        memset(buf2, '\0', 1024);
        readfds2 = readfds;
        nb = max(sockclient) + 1;

        select(nb, &readfds2, 0, 0, 0);

        if (FD_ISSET(sockfd, &readfds2)) {
            sockclient[taille] =
                accept(sockfd, (void *)&client[taille], &fromlen);
            FD_SET(sockclient[taille], &readfds);
            taille++;
        }

        for (int j = 0; j < taille; j++) {

            if (FD_ISSET(sockclient[j], &readfds2)) {
                if (recv(sockclient[j], buf, 1024, 0) == -1) {
                    perror("erreur de reception -> ");
                    exit(-3);
                }

                if (strncmp(buf, "/quit", 5) == 0) {
                    printf("client %i a quitté la discussion", j);
                    close(sockclient[j]);
                } else {
                    for (int i = 0; i < taille; i++) {
                        if (send(sockclient[i], buf, strlen(buf) + 1, 0) ==
                            -1) {
                            perror("erreur a l'appel de la fonction send -> ");
                            exit(-2);
                        }
                    }
                }
                // affichage de la chaine de caracteres recue
            }
        }

        if (FD_ISSET(0, &readfds2)) {
            read(0, buf2, sizeof(buf2));

            if (send(sockclient[taille], buf2, strlen(buf2) + 1, 0) == -1) {
                perror("erreur a l'appel de la fonction send -> ");
                exit(-2);
            }
        }
    }
    // fermeture de la socket
    close(sockfd);
    for (int k = 0; k < taille; k++)
        close(sockclient[k]);
    return 0;
}
