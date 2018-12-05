#include "headlines.h"

static size_t offset = 0;
static size_t size = 0;
static ResHandle handle = 0;

char * get_headline() {
    if (handle == 0) {
        handle = resource_get_handle(RESOURCE_ID_HEADLINES_FILE);
    }
    if (size == 0) {
        size = resource_size(handle);
    }
    uint16_t item_length;
    resource_load_byte_range(handle, offset, (uint8_t *) &item_length, 2);
    offset += 2;

    char * content = malloc(item_length + 1);
    resource_load_byte_range(handle, offset, (uint8_t *) content, item_length);
    *(content + item_length) = 0;

    offset += item_length;
    if (offset >= size) {
        offset = 0;
    }
    return content;
}
