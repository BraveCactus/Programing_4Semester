#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>


double pi_sum(int N, int beg, int end) {
    double height = 2.0 / N;
    double pi = 0.0;
    for (int i = beg; i <= end; ++i) {
        pi += 0.5 * (sqrt(4.0 - height * i * height * i) + sqrt(4.0 - height * (i + 1) * height * (i + 1))) * height;                           
    }
    return pi;
}

int main() {
    int sockfd, n;
    struct sockaddr_in servaddr, cliaddr;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(52038); 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Can't create socket, errno = %d\n", errno);
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("Can't bind socket, errno = %d\n", errno);
        close(sockfd);
        exit(1);
    }

    while (1) {
        int get_array[3];
        socklen_t clilen = sizeof(cliaddr);

        if ((n = recvfrom(sockfd, get_array, sizeof(get_array), 0,
                          (struct sockaddr *)&cliaddr, &clilen)) < 0) {
            printf("Can't receive request, errno = %d\n", errno);
            close(sockfd);
            exit(1);
        }

	double pi = 0.0;
        pi += pi_sum(get_array[0], get_array[1], get_array[2]);

        if (sendto(sockfd, &pi, sizeof(double), 0, (struct sockaddr *)&cliaddr,
                   clilen) < 0) {
            printf("Can't send answer, errno = %d\n", errno);
            close(sockfd);
            exit(1);
        }
    }
    
    close(sockfd);
    return 0;
}
