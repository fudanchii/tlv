/**
 * Copyright (c) 2012, Nurahmadie <nurahmadie@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <string.h>
#include <stdlib.h>

#include "tlv.h"

TLV *(tlvTable[0x100][0x100]);

#define tlvEntry(tag) (tlvTable[(byte)tag[0]][(byte)tag[1]])

#define tagEq(tag, cmp) ((tag[0] == *(cmp++)) && (tag[1] == *(cmp++)))

#define bin_cat(buf, src, len) \
do { \
    memcpy(buf, src, len); \
    buf += len; \
} while(0);

void tlv_initTable()
{
    int i, j;
    for (i = 0; i < 0xff; i++)
        for (j = 0; j < 0xff; j++)
            tlvTable[i][j] = NULL;
}

TLV *tlv_new(int len, byte **src)
{
    TLV *node = malloc(sizeof(TLV));
    if (!node)
        return NULL;
    node->length = len;
    node->value = malloc(len);
    if (!(node->value)) {
        free(node);
        return NULL;
    }
    memcpy(node->value, *src, len);
    *src += len;
    return node;
}

int tlv_build(byte *rawStr, byte *tagList[])
{
    int i = 0;
    byte *tag = NULL;
    byte *cursor = rawStr;
    
    tlv_initTable();
    
    while (tag = tagList[i++]) {
        if (tagEq(tag, cursor)) {
            tlvEntry(tag) = tlv_new((byte)*(cursor++), &cursor);
        }
    }
    return 0;
}


int tlv_getValue(byte *tag, byte **value)
{
    if (!tlvEntry(tag))
        return -1;
    *value = tlvEntry(tag)->value;
    return tlvEntry(tag)->length;
}

int tlv_insert(byte *tag, TLV *tlv)
{
    tlvEntry(tag) = tlv;
    return tlv->length;
}

int tlv_setValue(byte *tag, int length, byte *value)
{
    if (!tlvEntry(tag))
        return tlv_insert(tag, tlv_new(length, &value));
    if (tlvEntry(tag)->value)
        free(tlvEntry(tag)->value);

    tlvEntry(tag)->value = malloc(length);
    if (!(tlvEntry(tag)->value))
        return -1;
    memcpy(tlvEntry(tag)->value, value, length);

    tlvEntry(tag)->length = length;
    return length;
}

int tlv_dump(byte *tlvStr, byte *tagList[])
{
    int i = 0, length = 0;
    byte *tag;
    byte *cursor = tlvStr;
    while (tag = tagList[i++]) {
        if (tlvEntry(tag)) {
            bin_cat(cursor, tag, 2);
            *(cursor++) = (byte) tlvEntry(tag)->length;
            bin_cat(cursor, tlvEntry(tag)->value, tlvEntry(tag)->length);
            length += 2 + 1 + tlvEntry(tag)->length;
        }
    }
    return length;
}

