#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SOCKET_ERROR -1
#define MCLI_PORT_DEFAULT 8888

typedef int SOCKET;
static struct timeval rcv_timeout = {
	.tv_sec = 0,
	.tv_usec = 100 * 1000	/* 100 ms */
};

static struct timeval snd_timeout = {
	.tv_sec = 0,
	.tv_usec = 100 * 1000	/* 100 ms */
};

int mcli_client_init(void)
{
	SOCKET sock_fd = SOCKET_ERROR;
	struct sockaddr_in addr;
	int rc;

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_fd == SOCKET_ERROR)
		goto out;

	printf("After socket()\n");

	rc = setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO,
			(const char *)&rcv_timeout, sizeof(rcv_timeout));
	if (rc != 0)
		goto out;

	rc = setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO,
			(const char *)&snd_timeout, sizeof(snd_timeout));
	if (rc != 0)
		goto out;

	printf("After setsockopt()\n");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(MCLI_PORT_DEFAULT);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	inet_aton("127.0.0.1", &addr.sin_addr);

	rc = bind(sock_fd, (struct sockaddr *)&addr,
		sizeof(struct sockaddr));
	if (rc <= SOCKET_ERROR)
		goto out;

	printf("After bind()\n");

	return sock_fd;

out:
	if(sock_fd != SOCKET_ERROR)
		close(sock_fd);

	return SOCKET_ERROR;
}



int main(int argc, char *argv[]) {

	SOCKET mcli_client_sock = SOCKET_ERROR;

	printf("main() of mcli_client\n");

	mcli_client_sock = mcli_client_init();
	if (mcli_client_sock == SOCKET_ERROR) {
		printf("mcli_client_init failed\n");
		return -1;
	}

	return 0;
}
