#ifndef PERIMETER_GAMECONTENT_H
#define PERIMETER_GAMECONTENT_H

extern GAME_CONTENT terGameContentBase;
extern GAME_CONTENT terGameContentAvailable;
extern GAME_CONTENT terGameContentSelect;

void detectGameContent();

bool unavailableContentUnitAttribute(terUnitAttributeID id, GAME_CONTENT content = terGameContentAvailable);
bool unavailableContentBelligerent(terBelligerent belligerent, GAME_CONTENT content = terGameContentAvailable);

///Decomposes each flags set in content into a separate list of enums for content 
std::vector<GAME_CONTENT> getGameContentEnums(const uint32_t& content);
///Decomposes each flag name set in content into a separate list of enums for content
std::vector<GAME_CONTENT> getGameContentFromEnumName(const std::string& content);
///Return's any content that is not available currently 
std::vector<GAME_CONTENT> getMissingGameContent(const GAME_CONTENT& content, const GAME_CONTENT& required);
///Get enum name for content or contents
std::string getGameContentEnumName(const GAME_CONTENT& content);
///Get user facing name for content
std::string getGameContentName(const GAME_CONTENT& content);
///Get game content for campaigns
GAME_CONTENT getGameContentCampaign();

#endif //PERIMETER_GAMECONTENT_H
