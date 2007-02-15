//-----------------------------------------------------//
// Rijndael encryption
//-----------------------------------------------------//

#ifndef H__RIJNDAEL
#define H__RIJNDAEL

int rijndaelSetupEncrypt(u32 *rk, const u8 *key, int keybits);
int rijndaelSetupDecrypt(u32 *rk, const u8 *key, int keybits);
void rijndaelEncrypt(const u32 *rk, int nrounds,
  const u8 plaintext[16], unsigned char ciphertext[16]);
void rijndaelDecrypt(const u32 *rk, int nrounds,
  const u8 ciphertext[16], unsigned char plaintext[16]);

int RijnDael_decrypt(char* InBuff, char* OutBuff, int Size, char* Password);
int RijnDael_encrypt( char* InBuff, char* OutBuff, int Size, char* Password);

#define KEYLENGTH(keybits) ((keybits)/8)
#define RKLENGTH(keybits)  ((keybits)/8+28)
#define NROUNDS(keybits)   ((keybits)/32+6)

#endif	// H__RIJNDAEL

