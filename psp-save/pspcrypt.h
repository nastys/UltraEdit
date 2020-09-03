#ifndef CRYPT_H
#define CRYPT_H

int decrypt_data(unsigned int mode, unsigned char *data, int *data_len, int *aligned_len, unsigned char *key);
int encrypt_data(unsigned int mode, unsigned char *data, int *dataLen, int *alignedLen, unsigned char *hash, unsigned char *cryptkey);

#endif // CRYPT_H
