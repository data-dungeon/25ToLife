
#ifdef __cplusplus
extern "C" {
#endif
char InitCompress(void);
void EndCompress(void);
char InitDecompress(void);
void EndDecompress(void);
void InitTree(void);
void InsertNode(int r);
void DeleteNode(int p);
int EncodeLZSS(unsigned char* source, unsigned char* dest, int size);
int DecodeLZSS(unsigned char* source, unsigned char* dest, int size);
int EncodeRLE(unsigned char* source, unsigned char* dest, int size);
int DecodeRLE(unsigned char* source, unsigned char* dest, int size);
#ifdef __cplusplus
}
#endif

