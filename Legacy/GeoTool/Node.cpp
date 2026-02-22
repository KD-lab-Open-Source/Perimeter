#include <stdafx.h>
#include <TGA.hpp>
#include <FlatModelTexture3D.hpp>
#include "Node.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const string Node::TITLE = "title";

Node::Node(const string& paramString) {

	title = tx3d::XMLUtils::extractPropertyFromTag
						(
							tx3d::XMLUtils::VALUE,
							tx3d::XMLUtils::extractTagFromXML(TITLE, 0, paramString)
						 );

}

string Node::loadXML(const string& paramString, const string& tagName) {
	string fileName = tx3d::XMLUtils::extractPropertyFromTag
						(
							tx3d::FlatModelTexture3D::SRC,
							tx3d::XMLUtils::extractTagFromXML(tagName, 0, paramString)
						 );
	
	char filePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, filePath);
	strcat(filePath, "\\GeoTool\\res\\Templates\\");
	strcat(filePath, fileName.c_str());

	ifstream ifs(filePath);
	char szBuffer[1024 * 10];
	ifs.get(szBuffer, 1024 * 10, '\0');

	return string(szBuffer);
}
