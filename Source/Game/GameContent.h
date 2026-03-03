#ifndef PERIMETER_GAMECONTENT_H
#define PERIMETER_GAMECONTENT_H

///The identified content at the root of game content, this only can be one thing 
extern GAME_CONTENT terGameContentBase;
///All available contents in this installation (base + addons)
extern GAME_CONTENT terGameContentAvailable;
///Current selected content, can be several or only one in available content (when user chooses one)
extern GAME_CONTENT terGameContentSelect;

#define MOD_ORIGIN_GAME_MODS "game_mods"

/**
 * Contains each mod metadata
*/
class ModMetadata {
public:
    /// Path for this mod
    std::string path = {};
    /// Errors when loading mod if any
    std::vector<std::string> errors = {};
    /// Has campaign missions?
    //TODO use this on "Change Campaign"
    bool campaign = false;
    /// Is loaded?
    bool available = false;
    /// Is enabled?
    bool enabled = false;
    /// Is this ET content mod? (used to handle separately)
    bool is_content_et = false;
    /// Where was mod loaded from
    std::string mod_origin = {};

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

    /// Locale used for mod description
    /// Optional
    /// Example: english
    std::string mod_description_locale = {};

    /// Description for mod in english, this is the fallback description
    /// Optional
    /// Example: Example mod description
    std::string mod_description_english = {};

    /// List of authors separated by comma
    /// Optional
    /// Example: AuthorA, AuthorB
    std::string mod_authors = {};

    /// License for mod
    /// Optional
    /// Example: GPLv3
    std::string mod_license = {};

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
///Checks files in mod for disallowed files
bool containsDisallowedFilesMod(ModMetadata& mod, bool publishing);
///Parses mod from path and fills ModMetadata
void parseModInPath(ModMetadata& mod, const char* mod_path, const char* mod_origin);
///Adds mod as pending to load
void addPendingMod(ModMetadata& mod);
///Loads mods that were queued as pending
void loadPendingMods();

#endif //PERIMETER_GAMECONTENT_H
