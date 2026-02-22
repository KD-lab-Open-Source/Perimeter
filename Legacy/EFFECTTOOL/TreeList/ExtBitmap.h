#pragma once

class CExtBitmap : public CBitmap
{
public:
	CExtBitmap(void);
	~CExtBitmap(void);
	bool LoadFromMemory(void* buff);
private:
};
