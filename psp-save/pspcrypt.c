/* decrypt_data() and encrypt_data() are from
 * https://github.com/hrydgard/ppsspp/tree/master/Tools/SaveTool, so here's the
 * copyright:
 *
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * Copyright (c) 2005 Jim Paris <jim@jtan.com>
 * Coypright (c) 2005 psp123
 */

#include "chnnlsv.h"
#include <string.h>
#include "psp-save/pspcrypt.h"

/* Do the actual hardware decryption.
   mode is 3 for saves with a cryptkey, or 1 otherwise
   data, dataLen, and cryptkey must be multiples of 0x10.
   cryptkey is NULL if mode == 1.
*/
int decrypt_data(unsigned int mode, unsigned char *data, int *data_len, int *aligned_len, unsigned char *key)
{
    pspChnnlsvContext1 ctx1;
    pspChnnlsvContext2 ctx2;

    /* Need a 16-byte IV plus some data */
    if (*aligned_len <= 0x10)
        return -1;
    *data_len -= 0x10;
    *aligned_len -= 0x10;

    /* Set up buffers */
    memset(&ctx1, 0, sizeof(ctx1));
    memset(&ctx2, 0, sizeof(ctx2));

    /* Perform the magic */
    if (sceSdSetIndex_(&ctx1, mode) < 0)
        return -2;
    if (sceSdCreateList_(&ctx2, mode, 2, data, key) < 0)
        return -3;
    if (sceSdRemoveValue_(&ctx1, data, 0x10) < 0)
        return -4;
    if (sceSdRemoveValue_(&ctx1, data + 0x10, *aligned_len) < 0)
        return -5;
    if (sceSdSetMember_(&ctx2, data + 0x10, *aligned_len) < 0)
        return -6;

    /* Verify that it decrypted correctly */
    if (sceChnnlsv_21BE78B4_(&ctx2) < 0)
        return -7;

    /* The decrypted data starts at data + 0x10, so shift it back. */
    memmove(data, data + 0x10, *data_len);
    return 0;
}

/* Do the actual hardware encryption.
   mode is 3 for saves with a cryptkey, or 1 otherwise
   data, dataLen, and cryptkey must be multiples of 0x10.
   cryptkey is NULL if mode == 1.
*/
int encrypt_data(unsigned int mode, unsigned char *data, int *dataLen, int *alignedLen, unsigned char *hash, unsigned char *cryptkey)
{
    pspChnnlsvContext1 ctx1;
    pspChnnlsvContext2 ctx2;

    /* Make room for the IV in front of the data. */
    memmove(data + 0x10, data, *alignedLen);

    /* Set up buffers */
    memset(&ctx1, 0, sizeof(ctx1));
    memset(&ctx2, 0, sizeof(ctx2));
    memset(hash, 0, 0x10);
    memset(data, 0, 0x10);

    /* Build the 0x10-byte IV and setup encryption */
    if (sceSdCreateList_(&ctx2, mode, 1, data, cryptkey) < 0)
        return -1;
    if (sceSdSetIndex_(&ctx1, mode) < 0)
        return -2;
    if (sceSdRemoveValue_(&ctx1, data, 0x10) < 0)
        return -3;
    if (sceSdSetMember_(&ctx2, data + 0x10, *alignedLen) < 0)
        return -4;

    /* Clear any extra bytes left from the previous steps */
    memset(data + 0x10 + *dataLen, 0, *alignedLen - *dataLen);

    /* Encrypt the data */
    if (sceSdRemoveValue_(&ctx1, data + 0x10, *alignedLen) < 0)
        return -5;

    /* Verify encryption */
    if (sceChnnlsv_21BE78B4_(&ctx2) < 0)
        return -6;

    /* Build the file hash from this PSP */
    if (sceSdGetLastIndex_(&ctx1, hash, cryptkey) < 0)
        return -7;

    /* Adjust sizes to account for IV */
    *alignedLen += 0x10;
    *dataLen += 0x10;

    /* All done */
    return 0;
}
