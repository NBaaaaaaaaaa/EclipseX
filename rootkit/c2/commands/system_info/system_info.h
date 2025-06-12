#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H
#include <linux/uio.h>

extern struct class *real_block_class;
extern struct list_head *real_input_dev_list;
extern spinlock_t *real_input_mutex;
extern void (*real_si_swapinfo)(struct sysinfo *);
void os_info(void);
void hardware_info(void);
void system_load(void);

#endif