#ifndef _IR_CONTROL_H
#define _IR_CONTROL_H

void init_ir(void);
void ir_send_nec_data(uint8_t *data, int len);
void ir_recv_enable(void);
#endif
