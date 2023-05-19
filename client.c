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

int main(int argc, char **argv) {
    int sockfd;              // descripteur de socket
    struct sockaddr_in dest; // structure d'adresse qui contiendra les
                             // parametres reseaux du destinataire

    socklen_t fromlen = sizeof(struct sockaddr_in);
    char buf2[1024];
    char buf[1024];
    char quit[1024];
    quit[0] = 'q';
    quit[1] = 'u';
    quit[2] = 'i';
    quit[3] = 't';

    fd_set readfds;
    fd_set readfds2;

    FD_ZERO(&readfds);

    // verification du nombre d'arguments sur la ligne de commande
    if (argc != 3) {
        printf("Usage : %s @dest num_port \n", argv[0]);
        exit(-1);
    }

    // creation de la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // initialisation de la structure d'adresse du destinataire :

    // famille d'adresse
    dest.sin_family = AF_INET;

    // adresse IPv4 du destinataire
    inet_aton(argv[1], &(dest.sin_addr));

    // port du destinataire
    dest.sin_port = htons(atoi(argv[2]));
    connect(sockfd, (void *)&dest, sizeof(struct sockaddr_in));

    FD_SET(sockfd, &readfds);
    FD_SET(0, &readfds);
    // envoi de la chaine
    while (1) {
        // reception de la chaine de caracteres
        memset(buf, '\0', 1024);
        memset(buf2, '\0', 1024);
        readfds2 = readfds;

        select(sockfd + 1, &readfds2, 0, 0, 0);

        if (FD_ISSET(sockfd, &readfds2)) {
            if (recv(sockfd, buf, 1024, 0) == -1) {
                perror("erreur de reception -> ");
                exit(-3);
            }
            // affichage de la chaine de caracteres recue
            printf("client : %d %s", buf[0], buf);
        }
        if (FD_ISSET(0, &readfds2)) {
            read(0, buf2, sizeof(buf2));
            if (strncmp(buf2, "/quit", 5) == 0) {
                shutdown(sockfd, SHUT_RDWR);
                return 0;
            } else {
                if (send(sockfd, buf2, strlen(buf2) + 1, 0) == -1) {
                    perror("erreur a l'appel de la fonction send -> ");
                    exit(-2);
                }
            }
        }
    }
    // fermeture de la socket
    close(sockfd);
    return 0;
}
