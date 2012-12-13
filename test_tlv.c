#include "tlv.h"
#include <stdio.h>
#include <synchro/test_fwx.h>

int main()
{
    byte *raw = "\x9f\x06\x05\xa0\x00\x00\x03\x33" \
                "\x9f\x22\x01\x01" \
                "\xdf\x02\x10" \
                    "\x80\xc8\x90\x20\x2e\xb6\x66\x51" \
                    "\xc4\xfc\x2c\xbe\x6d\xa9\xc5\xbe";

    byte *tags[] = {"\x9f\x06", "\x9f\x22", "\xdf\x02", NULL};
    byte *test_value;
    byte buf[32];
    int test_len;

    test_begin;

    tlv_build(raw, tags);
    
    test_len = tlv_getValue("\xdf\x02", &test_value);
    assert_eq(test_len, 16);
    assert_mem(test_value, "\x80\xc8\x90\x20\x2e\xb6\x66\x51" \
                           "\xc4\xfc\x2c\xbe\x6d\xa9\xc5\xbe", test_len);

    test_len = tlv_getValue("\x9f\x06", &test_value);
    assert_eq(test_len, 0x05);
    assert_mem(test_value, "\xa0\x00\x00\x03\x33", test_len);

    test_len = tlv_dump(buf, tags);
    assert_mem(buf, raw, test_len);

    tlv_setValue("\xdf\x02", 5, "Hello");
    test_len = tlv_getValue("\xdf\x02", &test_value);
    assert_eq(test_len, 5);
    assert_mem(test_value, "Hello", 5);

    test_end;
    return 0;
}
