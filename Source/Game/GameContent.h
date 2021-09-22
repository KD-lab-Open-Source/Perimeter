#ifndef PERIMETER_GAMECONTENT_H
#define PERIMETER_GAMECONTENT_H

extern GAME_CONTENT terGameContentBase;
extern GAME_CONTENT terGameContent;

void detectGameContent();

bool unavailableContentUnitAttribute(terUnitAttributeID id, GAME_CONTENT content = terGameContent);
bool unavailableContentBelligerent(terBelligerent belligerent, GAME_CONTENT content = terGameContent);

#endif //PERIMETER_GAMECONTENT_H
