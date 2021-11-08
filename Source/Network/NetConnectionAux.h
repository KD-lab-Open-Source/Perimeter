#ifndef __P2P_INTERFACEAUX_H__
#define __P2P_INTERFACEAUX_H__

extern const char* currentShortVersion;

#include "UnitAttribute.h"
#include "../Terra/crc.h"

#define ATTRIBUTES_CRC_ARCHIVE XPrmOArchive
//#define ATTRIBUTES_CRC_ARCHIVE BinaryOArchive

///First packet sent upon connection
typedef uint64_t arch_flags;
const uint32_t NC_INFO_ID = 0xF8C20001;
class NetConnectionInfo {
private:
    //Header
    uint32_t id = 0;
    uint32_t versionCRC = 0;
    //Content
    arch_flags arch = 0;
    uint32_t passwordCRC = 0;
    uint32_t gameContent = 0;
    uint32_t attributesCRC = 0;
    char playerName[PLAYER_MAX_NAME_LEN] = "";
    uint32_t crc = 0;

    uint32_t calcOwnCRC(){
        uint32_t ownCRC=startCRC32;
        //Header
        ownCRC=crc32((unsigned char*)&id, sizeof(id), ownCRC);
        ownCRC=crc32((unsigned char*)&versionCRC, sizeof(versionCRC), ownCRC);
        //Content
        ownCRC=crc32((unsigned char*)&arch, sizeof(arch), ownCRC);
        ownCRC=crc32((unsigned char*)&passwordCRC, sizeof(passwordCRC), ownCRC);
        ownCRC=crc32((unsigned char*)&gameContent, sizeof(gameContent), ownCRC);
        ownCRC=crc32((unsigned char*)&attributesCRC, sizeof(attributesCRC), ownCRC);
        ownCRC=crc32((unsigned char*)&playerName, sizeof(playerName), ownCRC);
        return ownCRC;
    }

    static uint32_t getStringCRC(const char* str) {
        return crc32(reinterpret_cast<const unsigned char*>(str), strlen(str), startCRC32);
    }

public:

    static uint32_t getAttributesCRC() {
        const int floatDigits = 2;
        uint32_t attrcrc = startCRC32;
        attrcrc = getSerializationCRC<ATTRIBUTES_CRC_ARCHIVE>(attributeLibrary(), attrcrc, floatDigits);
        LogMsg("attributeLibrary CRC %X\n", attrcrc);
        attrcrc = getSerializationCRC<ATTRIBUTES_CRC_ARCHIVE>(rigidBodyPrmLibrary(), attrcrc, floatDigits);
        LogMsg("rigidBodyPrmLibrary CRC %X\n", attrcrc);
        attrcrc = getSerializationCRC<ATTRIBUTES_CRC_ARCHIVE>(globalAttr(), attrcrc, floatDigits);
        LogMsg("globalAttr CRC %X\n", attrcrc);
        return attrcrc;
    }

    static arch_flags computeArchFlags() {
        arch_flags val = 0;

        //Release build - 0 (1) bit
#if defined(_FINAL_VERSION_) && !defined(PERIMETER_DEBUG)
        val |= 1;
#endif

        //Compiler type - 1-7 (7) bits
        arch_flags compiler;
#if defined(_MSC_VER)
        compiler = 1;
#elif defined(__clang__)
        compiler = 2;
#elif defined(__GNUC__)
        compiler = 3; //Must be checked after clang as it also defines __GNUC__
#else
        compiler = 0;
#endif
        xassert(compiler <= 0x7F);
        val |= compiler<<1;

        //OS type - 8-15 (8) bits
        arch_flags os;
#if defined(__linux__)
        os = 1;
#elif defined( __APPLE__)
        os = 2;
#elif defined(_WIN32)
        os = 3;
#else
        os = 0;
#endif
        xassert(os <= 0xFF);
        val |= os<<8;

        //CPU type - 60-61-62-63 (4) bits
        arch_flags cpu = 0;
        //Arch - 60-61 bits (0 = under 32, 1 = 32, 2 = 64, 3 = above 64)
        cpu |= (sizeof(void*) / 4) & 3;
        //CPU endianness - 63 bit
#ifdef SDL_BIG_ENDIAN
        cpu |= 1<<3;
#endif
        xassert(cpu <= 0xF);
        val |= cpu<<60;

        return val;
    }
    
    
    NetConnectionInfo() = default;
    
    static size_t getSize() {
        size_t size = 0;
        size += sizeof(id);
        size += sizeof(versionCRC);
        
        size += sizeof(arch);
        size += sizeof(passwordCRC);
        size += sizeof(gameContent);
        size += sizeof(attributesCRC);
        size += sizeof(playerName);
        size += sizeof(crc);
        return size;
    }

    void read_header(XBuffer& in) {
        in.read(id);
        in.read(versionCRC);
    }

    void read_content(XBuffer& in) {
        in.read(arch);
        in.read(passwordCRC);
        in.read(gameContent);
        in.read(attributesCRC);
        in.read(playerName, sizeof(playerName));
        in.read(crc);
    }

    void write(XBuffer& out) const {
        out.write(id);
        out.write(versionCRC);

        out.write(arch);
        out.write(passwordCRC);
        out.write(gameContent);
        out.write(attributesCRC);
        out.write(playerName, sizeof(playerName));
        out.write(crc);
    }
    
    void set(const char* _version, const char* _password, unsigned int _gameContent, const char* _playerName) {
        id=NC_INFO_ID;
        arch= computeArchFlags();
        versionCRC=getStringCRC(_version);
        passwordCRC=getStringCRC(_password);
        gameContent = _gameContent;
        attributesCRC=getAttributesCRC();
        strncpy(playerName, _playerName, PLAYER_MAX_NAME_LEN);
        crc=calcOwnCRC();
    }

    bool isIDCorrect() const {
        return id == NC_INFO_ID;
    }

    bool isVersionCorrect(const char* _version) const {
        return versionCRC == getStringCRC(_version);
    }

    bool checkCRCCorrect(){
        return crc == calcOwnCRC();
    }

    bool isArchCompatible(arch_flags mask) const {
        if (mask) {
            return (arch & mask) == (computeArchFlags() & mask);
        } else {
            return arch == computeArchFlags();
        }
    }
    
    arch_flags getArchFlags() const {
        return arch;
    }

    bool isGameContentCompatible(GAME_CONTENT content, uint32_t attrcrc) const {
        return gameContent == content && (attrcrc == 0 || attributesCRC == attrcrc);
    }

    bool isPasswordCorrect(const char* _password) const {
        return passwordCRC == getStringCRC(_password);
    }
    
    const char* getPlayerName() const {
        return playerName;
    }
};


///Reply from sConnectInfo
const uint32_t NC_INFO_REPLY_ID = 0x47C3FE65;
struct NetConnectionInfoResponse {
    enum e_ConnectResult{
        CR_NONE,
        CR_OK,
        CR_ERR_INCORRECT_SIGNATURE,
        CR_ERR_INCORRECT_ARCH,
        CR_ERR_INCORRECT_VERSION,
        CR_ERR_INCORRECT_CONTENT,
        CR_ERR_INCORRECT_PASWORD,
        CR_ERR_GAME_STARTED,
        CR_ERR_GAME_FULL
    };

    uint32_t id = 0;
    e_ConnectResult connectResult = CR_NONE;
    NETID clientID = 0;
    NETID hostID = 0;
    uint32_t crc = 0;

    NetConnectionInfoResponse() = default;

    static size_t getSize() {
        size_t size = 0;
        size += sizeof(id);
        size += sizeof(connectResult);
        size += sizeof(clientID);
        size += sizeof(hostID);
        size += sizeof(crc);
        return size;
    }

    uint32_t calcOwnCRC(){
        uint32_t ownCRC=startCRC32;
        ownCRC=crc32((unsigned char*)&id, sizeof(id), ownCRC);
        ownCRC=crc32((unsigned char*)&connectResult, sizeof(connectResult), ownCRC);
        ownCRC=crc32((unsigned char*)&clientID, sizeof(clientID), ownCRC);
        ownCRC=crc32((unsigned char*)&hostID, sizeof(hostID), ownCRC);
        return ownCRC;
    }

    void read(XBuffer& in) {
        in.read(id);
        in.read(connectResult);
        in.read(clientID);
        in.read(hostID);
        in.read(crc);
    }

    void write(XBuffer& out) const {
        out.write(id);
        out.write(connectResult);
        out.write(clientID);
        out.write(hostID);
        out.write(crc);
    }

    void set(e_ConnectResult cR, NETID clientID_, NETID hostID_) {
        id=NC_INFO_REPLY_ID;
        connectResult=cR;
        clientID=clientID_;
        hostID=hostID_;
        crc=calcOwnCRC();
    }

    bool checkOwnCorrect(){
        return ( (crc==calcOwnCRC()) && (id==NC_INFO_REPLY_ID) );
    }
};



////////////////////////////////////////////////////////////////////////

/**
 * InputPacket, holds buffer and connection sending it
 */
struct InputPacket : public XBuffer {
public:
    NETID netid;

    explicit InputPacket(NETID _netid) : XBuffer(0), netid(_netid) {
    }
};

#endif //__P2P_INTERFACEAUX_H__


