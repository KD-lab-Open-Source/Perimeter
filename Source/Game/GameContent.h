#ifndef PERIMETER_GAMECONTENT_H
#define PERIMETER_GAMECONTENT_H

extern GAME_CONTENT terGameContent;

void detectGameContent();

bool unavailableContentUnitAttribute(terUnitAttributeID id);
bool unavailableContentBelligerent(terBelligerent belligerent);

#endif //PERIMETER_GAMECONTENT_H
