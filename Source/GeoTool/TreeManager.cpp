#include <stdafx.h>
#include <XMLUtils.hpp>
#include "LocationNode.h"
#include "SimpleNode.h"
#include "ModelNode.h"
#include "TreeManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


TreeManager::TreeManager(CTreeCtrl* tree) : tree(tree) {
}

TreeManager::~TreeManager() {
	clearTree();
}

void TreeManager::fillTree() {
	
	char listFile[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, listFile);
	strcat(listFile, "\\GeoTool\\res\\Templates\\list.xml");

	ifstream ilist(listFile);
	char szBuffer[1024 * 10];
	ilist.get(szBuffer, 1024 * 10, '\0');

	string treeXML = tx3d::XMLUtils::extractContentFromTag(string(szBuffer));
	string textures = tx3d::XMLUtils::extractContentFromTag(
		tx3d::XMLUtils::extractTagFromXML("textures", 0, treeXML) );
	string simples = tx3d::XMLUtils::extractContentFromTag(
		tx3d::XMLUtils::extractTagFromXML("simples", 0, textures) );

//	string models = tx3d::XMLUtils::extractContentFromTag(
//		tx3d::XMLUtils::extractTagFromXML("models", 0, textures) );

	string locations = tx3d::XMLUtils::extractContentFromTag(
		tx3d::XMLUtils::extractTagFromXML("locations", 0, treeXML) );

	texturesFolder = tree->InsertItem("Texture");
	tree->SetItemData(texturesFolder, 0);

	tree->SetItemState(texturesFolder, TVIS_EXPANDED, TVIS_EXPANDED);
	tree->SetItemState(texturesFolder, TVIS_BOLD, TVIS_BOLD);
	
	
	loadTextures(simples, true);

	fillModels();
	
	locationsFolder = tree->InsertItem("Location");
	tree->SetItemData(locationsFolder, 0);
	tree->SetItemState(locationsFolder, TVIS_EXPANDED, TVIS_EXPANDED);
	tree->SetItemState(locationsFolder, TVIS_BOLD, TVIS_BOLD);

	loadLocations(locations);

	selectedTexture = NULL;
	selectedLocation = NULL;

	focusedItem = NULL;
}

void TreeManager::fillModels() {
	char listFile[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, listFile);
	strcat(listFile, "\\GeoTool\\res\\Templates\\modelList.xml");

	ifstream ilist(listFile);
	char szBuffer[1024 * 10];
	ilist.get(szBuffer, 1024 * 10, '\0');

	string modelsXML = tx3d::XMLUtils::extractContentFromTag(string(szBuffer));
	modelsFolder = tree->InsertItem("Model", texturesFolder);
	tree->SetItemData(modelsFolder, 0);
	tree->SetItemState(modelsFolder, TVIS_EXPANDED, TVIS_EXPANDED);
	tree->SetItemState(modelsFolder, TVIS_BOLD, TVIS_BOLD);

	loadTextures(modelsXML, false);
}

void TreeManager::saveModels() {
	char listFile[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, listFile);
	strcat(listFile, "\\GeoTool\\res\\Templates\\modelList.xml");
	ofstream ofs(listFile);

	string strLUT = "<models>";
	
	HTREEITEM nextItem;
	HTREEITEM childItem = tree->GetChildItem(modelsFolder);

	while (childItem != NULL) {
		nextItem = tree->GetNextItem(childItem, TVGN_NEXT);
		ModelNode* node = (ModelNode*)tree->GetItemData(childItem);
		if (node) {
			strLUT += node->toXML();
		}
		childItem = nextItem;
	}
	strLUT += "</models>";
	tx3d::XMLUtils::indentXML(strLUT);
	ofs << strLUT.c_str();
	ofs.close();
}

void TreeManager::clearTree() {
	clearFolder(texturesFolder);
	clearFolder(modelsFolder);
	clearFolder(locationsFolder);
}

void TreeManager::clearFolder(HTREEITEM folder) {
	HTREEITEM nextItem;
	HTREEITEM childItem = tree->GetChildItem(folder);

	while (childItem != NULL) {
		nextItem = tree->GetNextItem(childItem, TVGN_NEXT);
		Node* node = (Node*)tree->GetItemData(childItem);
		if (node) {
			delete node;
			tree->SetItemData(childItem, 0);
		}
		childItem = nextItem;
	}
}


void TreeManager::loadTextures(const string& textures, bool isSimples) {
	int textureCount = 0;
	string nodeXML;
	TextureNode* node;
	while(
		(nodeXML = tx3d::XMLUtils::extractTagFromXML("node", textureCount, textures)) != ""
	) {
		if (isSimples) {
			node = new SimpleNode(tx3d::XMLUtils::extractContentFromTag(nodeXML));
		} else {
			node = new ModelNode(tx3d::XMLUtils::extractContentFromTag(nodeXML));
		}
		addNode(node->getTitle().c_str(), isSimples ? texturesFolder : modelsFolder, (DWORD) node);
		textureCount++;
	}
}

void TreeManager::loadLocations(const string& locations) {
	int locationCount = 0;
	string nodeXML;
	LocationNode* node;
	while(
		(nodeXML = tx3d::XMLUtils::extractTagFromXML("node", locationCount, locations)) != ""
	) {
		node = new LocationNode(tx3d::XMLUtils::extractContentFromTag(nodeXML));
		addNode(node->getTitle().c_str(), locationsFolder, (DWORD) node);
		locationCount++;
	}
}

HTREEITEM TreeManager::addNode(const char* label, HTREEITEM parent, DWORD data) {
	HTREEITEM res = tree->InsertItem(label, parent);
	tree->SetItemData(res, data);
	return res;
}

bool TreeManager::setTextureSelected(HTREEITEM texture) {
	if (texture == selectedTexture) {
		return false;
	}
	if (selectedTexture != NULL) {
		tree->SetItemState(selectedTexture, 0, TVIS_BOLD);
	}
	selectedTexture = texture;
	if (selectedTexture != NULL) {
		tree->SetItemState(selectedTexture, TVIS_BOLD, TVIS_BOLD);
	}
	return true;
}

bool TreeManager::setLocationSelected(HTREEITEM location) {
	if (location == selectedLocation) {
		return false;
	}
	if (selectedLocation != NULL) {
		tree->SetItemState(selectedLocation, 0, TVIS_BOLD);
	}
	selectedLocation = location;
	if (selectedLocation != NULL) {
		tree->SetItemState(selectedLocation, TVIS_BOLD, TVIS_BOLD);
	}
	return true;
}

HTREEITEM TreeManager::getItemForTexture(tx3d::Texture3D* tx) {
	HTREEITEM res = getItemForTexture(tx, tree->GetChildItem(texturesFolder));
	if (res == NULL) {
		res = getItemForTexture(tx, tree->GetChildItem(modelsFolder));
	}
	return res;
}

HTREEITEM TreeManager::getItemForTexture(tx3d::Texture3D* tx, HTREEITEM firstChildItem) {
	HTREEITEM nextItem;
	HTREEITEM childItem = firstChildItem;
	while (childItem != NULL) {
		nextItem = tree->GetNextItem(childItem, TVGN_NEXT);
		TextureNode* node = (TextureNode*)tree->GetItemData(childItem);
		if (node && node->isTexureFromNode(tx)) {
			return childItem;
		}
		childItem = nextItem;
	}
	return NULL;
}

HTREEITEM TreeManager::getItemForLocation(tx3d::Location3D* loc) {
	HTREEITEM nextItem;
	HTREEITEM childItem = tree->GetChildItem(locationsFolder);
	while (childItem != NULL) {
		nextItem = tree->GetNextItem(childItem, TVGN_NEXT);
		LocationNode* node = (LocationNode*)tree->GetItemData(childItem);
		if (node && node->isLocationFromNode(loc)) {
			return childItem;
		}
		childItem = nextItem;
	}
	return NULL;
}

string TreeManager::getItemName(HTREEITEM item) {
	return tree->GetItemText(item);
}

void TreeManager::setFocusedItem(HTREEITEM item, bool isTexture) {
	focusedItem = item;
	if (item != NULL) {
		textureFocused = isTexture;
		tree->SelectItem(item);
	}
}

void TreeManager::insertModelNode(HTREEITEM itemBefore, Node* newNode) {
	HTREEITEM res = tree->InsertItem(newNode->getTitle().c_str(), modelsFolder, itemBefore);
	tree->SetItemData(res, (DWORD) newNode);
	setTextureSelected(res);
	saveModels();
}

void TreeManager::deleteModelNode(HTREEITEM modelNodeItem) {
	ModelNode* node = (ModelNode*) tree->GetItemData(modelNodeItem);
	if (node) {
		tree->DeleteItem(modelNodeItem);
		delete node;
	}
	saveModels();
}
