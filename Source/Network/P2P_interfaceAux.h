#ifndef __P2P_INTERFACEAUX_H__
#define __P2P_INTERFACEAUX_H__

#include "CommonEvents.h"//убрать перенеся SIMPLE_GAME_CURRENT_VERSION сюда!

#include "..\terra\crc.h"

struct sDigitalGameVersion {
	unsigned int gameVersion;
	sDigitalGameVersion(bool calc){
		if(calc){
			char buf[sizeof(SIMPLE_GAME_CURRENT_VERSION)];
			strncpy(buf, SIMPLE_GAME_CURRENT_VERSION, sizeof(SIMPLE_GAME_CURRENT_VERSION));
			int i;
			for(i=0; i<sizeof(SIMPLE_GAME_CURRENT_VERSION); i++){
				if(SIMPLE_GAME_CURRENT_VERSION[i]=='.' || SIMPLE_GAME_CURRENT_VERSION[i]==0){
					buf[i]=0; break;
				}
			}
			gameVersion=atoi(buf)<<16;
			i++;
			if(i < sizeof(buf)) gameVersion+=atoi(&buf[i]);
		}
		else gameVersion=0;//можно -1
	}
	bool operator == (const sDigitalGameVersion &ro) const { return (gameVersion==ro.gameVersion); }
	bool operator != (const sDigitalGameVersion &ro) const { return (gameVersion!=ro.gameVersion); }
	sDigitalGameVersion& operator =(const sDigitalGameVersion& donor){ 
		gameVersion=donor.gameVersion; 
		return *this;
	}

};


// {55C257F5-EBB7-4923-8543-6CDA554E8E5C}
const GUID CONNECT_INFO_GUID = 
{ 0x55c257f5, 0xebb7, 0x4923, { 0x85, 0x43, 0x6c, 0xda, 0x55, 0x4e, 0x8e, 0x5c } };
struct sConnectInfo{
	GUID guid;
	PlayerData perimeterPlayerData;
	sDigitalGameVersion dgv;
	unsigned long passwordCRC;
	unsigned int crc;

	sConnectInfo() : dgv(false) {}
	sConnectInfo(unsigned char* inBuf, int sizeInBuf) : dgv(false) {
		xassert( (sizeInBuf==sizeof(sConnectInfo)) && "Error size connect info");
		memcpy(this, inBuf, sizeof(sConnectInfo));
	}
	unsigned int calcOwnCRC(){
		unsigned int ownCRC=startCRC32;
		ownCRC=crc32((unsigned char*)&guid, sizeof(guid), ownCRC);
		ownCRC=crc32((unsigned char*)&perimeterPlayerData, sizeof(perimeterPlayerData), ownCRC);
		ownCRC=crc32((unsigned char*)&passwordCRC, sizeof(passwordCRC), ownCRC);
		ownCRC=crc32((unsigned char*)&dgv, sizeof(dgv), ownCRC);
		return ownCRC;
	}
	unsigned long getPasswordCRC(const char* _password){
		return crc32((unsigned char*)_password, strlen(_password), startCRC32);
	}
	void set(PlayerData& _perimeterPlayerData, const char* _password, sDigitalGameVersion _dgv){
		guid=CONNECT_INFO_GUID;
		perimeterPlayerData=_perimeterPlayerData;
		passwordCRC=getPasswordCRC(_password);
		dgv=_dgv;
		crc=calcOwnCRC();
	}
	bool checkOwnCorrect(){
		return ( (crc==calcOwnCRC()) && (guid==CONNECT_INFO_GUID) );
	}
	bool isPasswordCorrect(const char* _password){
		return (passwordCRC==getPasswordCRC(_password));
	}
};


// {47C3EE65-A429-4E7F-B6FB-475A4BEB86CF}
const GUID REPLY_CONNECT_INFO_GUID = 
{ 0x47c3ee65, 0xa429, 0x4e7f, { 0xb6, 0xfb, 0x47, 0x5a, 0x4b, 0xeb, 0x86, 0xcf } };
struct sReplyConnectInfo{
	enum e_ConnectResult{
		CR_OK,
		CR_ERR_INCORRECT_VERSION,
		CR_ERR_INCORRECT_PASWORD,
		CR_ERR_GAME_STARTED,
		CR_ERR_GAME_FULL,
		CR_ERR_CONNECT
	};
	GUID guid;
	e_ConnectResult connectResult;
	sDigitalGameVersion dgv;
	unsigned int crc;

	sReplyConnectInfo() : dgv(false) {}
	sReplyConnectInfo(unsigned char* inBuf, int sizeInBuf) : dgv(false) {
		xassert((sizeInBuf==sizeof(sReplyConnectInfo)) && "Error size connect info");
		memcpy(this, inBuf, sizeof(sReplyConnectInfo));
	}
	unsigned int calcOwnCRC(){
		unsigned int ownCRC=startCRC32;
		ownCRC=crc32((unsigned char*)&guid, sizeof(guid), ownCRC);
		ownCRC=crc32((unsigned char*)&connectResult, sizeof(connectResult), ownCRC);
		ownCRC=crc32((unsigned char*)&dgv, sizeof(dgv), ownCRC);
		return ownCRC;
	}
	void set(e_ConnectResult cR, sDigitalGameVersion _dgv){
		guid=REPLY_CONNECT_INFO_GUID;
		connectResult=cR;
		dgv=_dgv;
		crc=calcOwnCRC();
	}
	bool checkOwnCorrect(){
		return ( (crc==calcOwnCRC()) && (guid==REPLY_CONNECT_INFO_GUID) );
	}
};

#endif //__P2P_INTERFACEAUX_H__


