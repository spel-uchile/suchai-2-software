//
// Created by carlos on 2/29/24.
//

#include "drivers-sim/interface.h"

typedef struct iface_driver {
    bool open;
    void *context;
    void *socket;
    sem_t sem;
} iface_driver_t;

static iface_driver_t driver = {0};

int iface_open()
{
    assert(!driver.open);
    sem_init(&driver.sem, 0, 1);
    sem_wait(&driver.sem);

    int timeout_ms = 2000;
    driver.context = zmq_ctx_new();
    driver.socket = zmq_socket(driver.context, ZMQ_REQ);
    zmq_setsockopt(driver.socket, ZMQ_RCVTIMEO, &timeout_ms, sizeof(timeout_ms));
    zmq_setsockopt(driver.socket, ZMQ_SNDTIMEO, &timeout_ms, sizeof(timeout_ms));
    const char *endpoint = "tcp://localhost:5555";
    zmq_connect(driver.socket, endpoint);
    driver.open = true;
    printf("SIM iface Driver connected to %s", endpoint);

    sem_post(&driver.sem);
    return 0;
}

void iface_close()
{
    assert(driver.open);
    sem_wait(&driver.sem);
    zmq_close(driver.socket);
    zmq_ctx_destroy(driver.context);
    sem_destroy(&driver.sem);
}

static void iface_buff_debug(uint8_t *buff, size_t len)
{
    int i;
    printf("[ ");
    for(i=0; i<len; i++) printf("%02X ", buff[i]);
    printf("]");
}

/**
 * iface transaction
 * @param send Send buffer
 * @param send_len Send buffer length
 * @param recv Receive buffer
 * @param recv_len Receive buffer length
 * @return 0 if Ok, -1 otherwise
 */
int iface_transaction(uint8_t *send, size_t send_len, uint8_t *recv, size_t recv_len)
{
    assert(driver.open);
    sem_wait(&driver.sem);
    printf("Sending "); iface_buff_debug(send, send_len); printf("\n");
    if(zmq_send(driver.socket, send, send_len, 0) != send_len)
    {
        printf("Error sending!\n");
        sem_post(&driver.sem);
        return -1;
    }
    if(zmq_recv(driver.socket, recv, recv_len, 0) != recv_len)
    {
        printf("Error receiving!\n");
        sem_post(&driver.sem);
        return -1;
    }
    printf("Received "); iface_buff_debug(recv, recv_len); printf("\n");
    sem_post(&driver.sem);
    return 0;
}