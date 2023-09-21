#ifndef PERIMETER_GAMECONTENT_H
#define PERIMETER_GAMECONTENT_H

extern GAME_CONTENT terGameContentBase;
extern GAME_CONTENT terGameContentAvailable;
extern GAME_CONTENT terGameContentSelect;

/**
 * Contains each mod metadata
*/
class ModMetadata {
public:
    /// Path for this mod
    std::string path = {};
    /// Has campaign missions?
    //TODO use this on "Change Campaign"
    bool campaign = false;
    /// Is loaded?
    bool available = false;
    /// Is enabled?
    bool enabled = false;

    //Data from .ini
    /// Name to display in mod list and acts as identifier
    /// Mandatory
    /// Example: My mod
    std::string mod_name = {};

    /// Version of mod
    /// Mandatory
    /// Example: 1.0.0
    std::string mod_version = {};

    /// Description for mod
    /// Optional
    /// Example: Example mod description
    std::string mod_description = {};

    /// List of authors separated by comma
    /// Optional
    /// Example: AuthorA, AuthorB
    std::string mod_authors = {};

    /// URL link for mod
    /// Optional
    /// Example: URL
    std::string mod_url = {};

    /// Minimum game version required
    /// Optional
    /// Example: 3.0.11
    std::string content_game_minimum_version = {};

    /// Required game content to be present and active, may contain several enums
    /// Optional
    /// Example: PERIMETER|PERIMETER_ET
    std::string content_required_content = {};

    /// Disable mod if active/campaign game content matches exactly, may contain several enums
    /// Optional
    /// Example: PERIMETER|PERIMETER_ET
    std::string content_disallowed_content = {};

    ModMetadata() = default;
    ~ModMetadata() = default;
};

void detectGameContent();

bool unavailableContentUnitAttribute(terUnitAttributeID id, GAME_CONTENT content = terGameContentAvailable);
bool unavailableContentBelligerent(terBelligerent belligerent, GAME_CONTENT content = terGameContentAvailable);

///Get current game mods, enabled or not
std::map<std::string, ModMetadata>& getGameMods();
///Decomposes each flags set in content into a separate list of enums for content 
std::vector<GAME_CONTENT> getGameContentEnums(const uint32_t& content);
///Composes a single GAME_CONTENT with flags from a list of GAME_CONTENT
GAME_CONTENT mergeGameContentEnums(const std::vector<GAME_CONTENT>& list);
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
