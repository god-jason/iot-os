#ifndef __VERIFY_H__
#define __VERIFY_H__

#include <os.h>
#include <vfs.h>

typedef struct
{
   uint32_t ver;
   uint32_t crc;
   uint32_t keyOffset;
   uint32_t keySize;
   uint32_t sigOffset;
   uint32_t sigSize;
   //uint32_t cryptoAlg;
} T_SblCert;

typedef enum
{
   COMP_DATA   = 0x419CEEAD,
   COMP_CERT   = 0x429CEEAD,
   COMP_CONFIG = 0x439CEEAD,
   COMP_TAIL   = 0x449CEEAD,
} E_CompType;

typedef struct
{
   E_CompType type;
   uint32_t dataSize;
   uint32_t certSize;
} T_BinTailInfo;

int32_t DataCertVerify(T_SblCert *pCert, uint8_t *pKeyDigest, uint8_t *pSigData, uint32_t sigLen);
int32_t FileCertVerify(VFS_File* fp);

#endif  /* __VERIFY_H__ */
