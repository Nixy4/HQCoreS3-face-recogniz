#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

void fr_mdns_init();
void fr_mdns_update_framesize(int size);
const char *fr_mdns_query(size_t *out_len);

#ifdef __cplusplus
}
#endif
