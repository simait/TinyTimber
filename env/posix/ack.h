#ifndef ACK_H_
#define ACK_H_

#include <signal.h>
#include <pthread.h>

typedef struct ack_t ack_t;

void ack_delete(ack_t *ack);
ack_t *ack_new(void);
void ack_set(ack_t *ack);
void ack_clear(ack_t *ack);
void ack_wait(ack_t *ack, int);
int  ack_acked(ack_t *ack);

#endif
