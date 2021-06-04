#pragma once

class ExportBasement
{
public:
	ExportBasement(CSaver& saver,const char* name_);
	void Export(IGameMesh* pobject,IGameNode* node,Matrix3 root_node_matrix);
protected:
	CSaver& saver;
	IGameMesh* pobject;
	const char* name;
};