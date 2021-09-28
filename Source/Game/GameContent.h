#ifndef PERIMETER_GAMECONTENT_H
#define PERIMETER_GAMECONTENT_H

extern GAME_CONTENT terGameContentBase;
extern GAME_CONTENT terGameContentAvailable;
extern GAME_CONTENT terGameContentSelect;

void detectGameContent();

bool unavailableContentUnitAttribute(terUnitAttributeID id, GAME_CONTENT content = terGameContentAvailable);
bool unavailableContentBelligerent(terBelligerent belligerent, GAME_CONTENT content = terGameContentAvailable);

#endif //PERIMETER_GAMECONTENT_H
