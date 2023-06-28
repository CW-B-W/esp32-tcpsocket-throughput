#include <Arduino.h>
#include <WiFi.h>

#include "lwip/opt.h"
#include <lwip/sockets.h>

#include "lwip/sys.h"
#include "lwip/api.h"

#define PORT              (5000)
#define RECV_BUF_SIZE     (32 * 1024)

static void tcp_socket(void *arg)
{
    int sock = -1, connected;
    char *recv_data;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;
    int recv_data_len;

    recv_data = (char *)pvPortMalloc(RECV_BUF_SIZE);
    if (recv_data == NULL)
    {
        Serial.printf("No memory\n");
        goto __exit;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        Serial.printf("Socket error\n");
        goto __exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        Serial.printf("Unable to bind\n");
        goto __exit;
    }

    if (listen(sock, 5) == -1)
    {
        Serial.printf("Listen error\n");
        goto __exit;
    }

    while (1)
    {
        sin_size = sizeof(struct sockaddr_in);

        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);

        Serial.printf("new client connected from (%s, %d)\n",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        {
            int flag = 1;

            setsockopt(connected,
                    IPPROTO_TCP,     /* set option at TCP level */
                    TCP_NODELAY,     /* name of option */
                    (void *) &flag, /* the cast is historical cruft */
                    sizeof(int));    /* length of option value */
        }

        int t = 0;
        int checksum = 0;
        while (1)
        {
            recv_data_len = recv(connected, recv_data, RECV_BUF_SIZE, 0);
            for (int i = 0; i < recv_data_len; ++i) {
                checksum += recv_data[i];
            }
            checksum &= 0xFF;
            Serial.println(t++);

            if (recv_data_len <= 0)
            {
                Serial.printf("checksum = %d\n", checksum);
                break;
            }

            // Serial.printf("recv %d len data\n", recv_data_len);
            // write(connected, recv_data, recv_data_len);
        }
        if (connected >= 0)
            closesocket(connected);

        connected = -1;
    }
__exit:
    if (sock >= 0) closesocket(sock);
    if (recv_data) free(recv_data);
}

void setup() {
    Serial.begin(115200); // 序列埠初始化，設定 Baud

    const char *ssid = "ESP32-AP";
    const char *pass = "00000000";
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pass);

    // setCpuFrequencyMhz(80);
}

void loop() {
    Serial.println("entering tcp_socket");
    tcp_socket(NULL);
    Serial.println("exited tcp_socket");
}
