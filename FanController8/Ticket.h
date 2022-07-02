/*
 * Ticket.h
 *
 * Created: 1/9/2021 2:11:45 PM
 *  Author: Jack2bs
 */ 


#ifndef TICKET_H_
#define TICKET_H_

#include <stdint.h>
#include "jtUSB.h"
#include "Main.h"

#define KEY %%%

int sendCommonTicket(int nOfFans);
int sendNumberOfFansTicket(int nOfFans);
void rpmToString(int rpm, uint8_t string[4]);
int sendMaxTicket(int nOfFans);


#endif /* TICKET_H_ */