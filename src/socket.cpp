#include "socket.hpp"

ssize_t
Recv (int sock, const void *buf, ssize_t size, ssize_t unit) {
	ssize_t recvd = 0;
	ssize_t yet = size;
	while (recvd < size) {
		if ( 0 < yet && yet < MAXBUFSIZE )  /* 아직 받지않은 데이터가 MAXBUFSIZE보다 작은 경우 */
			recvd += recv (sock, (void *) (buf + recvd/unit), yet, 0);
		else /* 받아야 할 데이터가 MAXBUFSIZE보다 큰 경우 */
			recvd += recv (sock, (void *) (buf + recvd/unit), MAXBUFSIZE, 0);

		yet = size - recvd;
	}
	return recvd;
}

int
tcp_connect (int af, char* servip, unsigned short port) {
	printf ("tcp_connect()\n");
	struct sockaddr_in servaddr;
	int s;
	// Create a socket.
	if ((s = socket(af, SOCK_STREAM, 0)) < 0)
		return -1;
	memset (&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = af;
	inet_pton (AF_INET, servip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port);

	// Connection request
	printf ("connect()\n");
	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		return -1;

	return s;
}

int
recv_mode_flag (const int& sock) {
	// printf ("Second, recv terminate_flag\n");
	int MODE_FLAG;
	int recvd = Recv (sock, &MODE_FLAG, sizeof(MODE_FLAG), 1);
	// printf (" >> recvd: %d\n", recvd);
	ASSERT (recvd == sizeof(MODE_FLAG));
	return MODE_FLAG;
}

Size
recv_res (const int& sock) {
	int width, height;
	int recvd = Recv (sock, &width, sizeof(width), 1);
	ASSERT (recvd == sizeof(width));
	recvd = Recv (sock, &height, sizeof(height), 1);
	ASSERT (recvd == sizeof(height));

	return Size(width, height);
}

bool
recv_notification (const int& sock) {
	// Get notification to take a picture.
	// printf ("Third, recv notification\n");
	// printf (" >> notification's size: %d\n", sizeof(bool));
	bool notification = false;
	int recvd = Recv (sock, &notification, sizeof(notification), 1);
	// printf (" >> recvd: %d\n", recvd);
	ASSERT (recvd == sizeof(notification));

	return notification;
}


/*
	Returns false if connection fails.
	Otherwise, returns true.
*/
void
SendBuffer (char* IP, unsigned short PORT, std::vector<unsigned char>& vec, int camId) {
	// Set socket.
	struct sockaddr_in server;
	int sock;
	sock = tcp_connect(AF_INET, IP, PORT);
	ASSERT (sock != -1);

	int sent;
	
	// Send camId
	// printf ("First, send a camId.. %d\n", camId);
	sent = send (sock, &camId, sizeof(int), 0);
	ASSERT (sent == sizeof(int));
	// printf (" >> sent camId!\n");

	int MODE_FLAG;
	Size res(416, 416); // default is 416X416
	while (true) {
		MODE_FLAG = recv_mode_flag(sock);
		if (MODE_FLAG == TERMINATE_MODE)
			break;
		res = recv_res(sock);
		// Receive notification
		if (recv_notification(sock)) {
			printf ("Got notification! take a picture.\n");
			vec = ImageProcessing(res); // Take a picture.
			printf ("encoding perfectly done!\n");
		}
		else // terminate
			break;

		// Send @vec.size()
		sent = 0;
		size_t bufSize = vec.size();
		sent = send (sock, (size_t *) &bufSize, sizeof(bufSize), 0);
		ASSERT (sent == sizeof(vec.size()));

		// Send @vec
		sent = 0;
		for (int i=0; i<vec.size(); i++) {
			sent += send (sock, &vec[i], sizeof(unsigned char), 0);
		}
		ASSERT (sent == vec.size());
	}
}
