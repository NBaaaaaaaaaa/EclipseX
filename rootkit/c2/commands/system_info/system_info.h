#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <linux/uio.h>

void os_info(struct kvec *vec);
void hardware_info(void);
void system_load(void);

#endif