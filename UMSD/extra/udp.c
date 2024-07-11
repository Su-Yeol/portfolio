#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>
#include <pthread.h>
#define BUF_SIZE 100

void *thread_udpsend(void *arg);

int main(void)
{
    pthread_t t_id;
    int thread_param = 5;
    void *thr_ret;

    int start_time, end_time;
    float time;
    int i;
    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t clnt_adr_sz;
    struct sockaddr_in serv_adr, clnt_adr;
    char *clnt;

    serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1)
        printf("UDP socket creation error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr("192.168.10.11");
    serv_adr.sin_port = htons(atoi("3333"));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        printf("bind() error");

    if (pthread_create(&t_id, NULL, thread_udpsend, NULL) != 0)
    {
        printf("udp thread create error\n");
        return -1;
    }
    while (1)
    {
        memset(message, 0, sizeof(message));
        clnt_adr_sz = sizeof(clnt_adr);
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0,
                           (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        clnt = inet_ntoa(clnt_adr.sin_addr);
        printf("Sensor Node Addr = %s\n", clnt);
    }

    if (pthread_join(t_id, &thr_ret) != 0)
    {
        printf("pthread join error \n");
        return -1;
    }
    free(thr_ret);
    close(serv_sock);
    return 0;
}

void *thread_udpsend(void *arg)
{
    // Socket 생성
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // IPv4, UDP/ip => 프로토콜 지정 / Protocol Family
    // sock = socket(AF_INET, SOCK_DGRAM, 0); IPv4, UDP/ip

    if (sock == -1) // socket creat fail.
        printf("Sock Error\n");

    struct sockaddr_in S32_adr;           // S32 IP, Port Setup => sockaddr_in 구조체, Socket의 주소체계 / Address Family
    memset(&S32_adr, 0, sizeof(S32_adr)); // add 0 Initialization
    S32_adr.sin_family = AF_INET;
    S32_adr.sin_addr.s_addr = inet_addr("192.168.10.11");
    S32_adr.sin_port = htons(atoi("3333"));

    if (bind(sock, (struct sockaddr *)&S32_adr, sizeof(S32_adr)) == -1)
        printf("bind() error");

    int flag = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)); // socketoption 에서 broadcast 허용을 해야한다.

    struct sockaddr_in dest_adr; // 5748 Ip, Port Setup
    memset(&dest_adr, 0, sizeof(dest_adr));
    dest_adr.sin_addr.s_addr = inet_addr("192.168.10.10");
    dest_adr.sin_port = htons(atoi("3333"));

    struct timeval mytime, t1, t2, t3;
    struct timespec req, rem;
    {
        req.tv_sec = 0;
        req.tv_nsec = 100000000;
    }

    int snd_buf, rcv_buf, state;
    socklen_t len;
    len = sizeof(snd_buf);
    getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &len);
    len = sizeof(rcv_buf);
    getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&rcv_buf, &len);

    char Data_send[10] = "HELLO";
    double CurTim = 0;
    // memset(Data_send, 0, sizeof(Data_send));
    int sendnum = 0;

    while (1)
    {
        gettimeofday(&t3, NULL);
        // CurTim = (t3.tv_sec * 1000 + t3.tv_usec / 1000);
        sendnum = sendto(sock, Data_send, strlen(Data_send), 0, (struct sockaddr *)&dest_adr, sizeof(dest_adr));
        if (sendnum != 0)
        {
            gettimeofday(&t2, NULL);
            double diffTime_ms = (t2.tv_sec - t1.tv_sec) * 1000 + ((t2.tv_usec - t1.tv_usec) / 1000);
            // printf("sendTime : %lf\n", diffTime_ms);
            gettimeofday(&t1, NULL);
        }
        else
            printf("errnum : %d\n", sendnum);

        if (nanosleep(&req, &rem) == -1)
            printf("sleep error\n");
    }
    close(sock);
    return 0;
}