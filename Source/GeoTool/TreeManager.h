#ifndef _TREE_MANAGER_H
#define _TREE_MANAGER_H

#include <Texture3D.hpp>
#include "ActionNode.h"

class TreeManager {

	public :

		TreeManager(CTreeCtrl* tree);
		~TreeManager();

		void fillTree();
		void fillModels();
		void saveModels();
		void clearTree();
		bool setTextureSelected(HTREEITEM texture);
		bool setLocationSelected(HTREEITEM location);

		HTREEITEM getItemForTexture(tx3d::Texture3D* tx);
		HTREEITEM getItemForLocation(tx3d::Location3D* loc);

		string getItemName(HTREEITEM item);

		void setFocusedItem(HTREEITEM focusedItem, bool isTexture);

		void insertModelNode(HTREEITEM itemBefore, Node* newNode);
		void deleteModelNode(HTREEITEM modelNodeItem);

		bool isTextureFocused() {
			return textureFocused;
		}

		HTREEITEM getFocusedItem() {
			return focusedItem;
		}

		HTREEITEM getSelectedTexture() {
			return selectedTexture;
		}

		HTREEITEM getSelectedLocation() {
			return selectedLocation;
		}

	protected :

		HTREEITEM getItemForTexture(tx3d::Texture3D* tx, HTREEITEM firstChildItem);
		HTREEITEM addNode(const char* label, HTREEITEM parent, DWORD data);
		void loadTextures(const string& simples, bool isSimples);
		void loadLocations(const string& locations);
		void clearFolder(HTREEITEM folder);

		CTreeCtrl* tree;
		HTREEITEM focusedItem;
		bool textureFocused;

		HTREEITEM selectedTexture;
		HTREEITEM selectedLocation;

		HTREEITEM texturesFolder;
		HTREEITEM modelsFolder;
		HTREEITEM locationsFolder;
};

#endif //_TREE_MANAGER_H
