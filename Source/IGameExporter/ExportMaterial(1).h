#pragma once

class ExportMaterial
{
public:
	ExportMaterial(CSaver& saver_);
	bool Export(IGameMaterial * mat);
protected:
	CSaver& saver;
};