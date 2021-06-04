#pragma once

namespace menu_h{
inline void disableMenuItem(CMenu* pm, UINT id){
	pm->EnableMenuItem(id, MF_BYCOMMAND | MF_DISABLED| MF_GRAYED);
}
inline void enableMenuItem(CMenu* pm, UINT id){
	pm->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);
}

inline void checkMenuItem(CMenu* pm, UINT id, bool bCheck){
	if (bCheck)
		pm->CheckMenuItem(id, MF_BYCOMMAND|MF_CHECKED);
	else
		pm->CheckMenuItem(id, MF_BYCOMMAND|MF_UNCHECKED);
}

inline bool removeMenuItemByCommand(CMenu* pm, UINT id){
	return (pm->RemoveMenu(id, MF_BYCOMMAND) == TRUE);
}

inline bool removeMenuItemByPosition(CMenu* pm, UINT pos){
	return (pm->RemoveMenu(pos, MF_BYPOSITION) == TRUE);
}

}//end of menu_h