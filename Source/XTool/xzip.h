#ifndef __XZIP_H__
#define __XZIP_H__

typedef unsigned long ulong;

ulong ZIP_compress(char* trg,ulong trgsize,char* src,ulong srcsize);
ulong ZIP_GetExpandedSize(char* p);
void ZIP_expand(char* trg,ulong trgsize,char* src,ulong srcsize);


#endif //__XZIP_H__