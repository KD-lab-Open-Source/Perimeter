#include "NetIncludes.h"

#include <SDL.h>

#include "SystemUtil.h"
#include "Runtime.h"
#include "UnitAttribute.h"
#include "Universe.h"
#include "P2P_interface.h"
#include "GameShell.h"
#include "files/files.h"
#include "GameContent.h"
#include "codepages/codepages.h"

bool net_log_mode=0;
XBuffer net_log_buffer(8192, 1);

//XStream quantTimeLog("quantTime.log",XS_OUT);
const char* autoSavePlayReelDir = "RESOURCE\\Replay\\Autosave";


const char * KEY_REPLAY_REEL="replay";

//Old RePlayReel date (до патча 1.2)
// {62177A85-C27C-4F6A-9837-922F635C7B08}
//static const GUID filePlayReelID = { 0x62177a85, 0xc27c, 0x4f6a, { 0x98, 0x37, 0x92, 0x2f, 0x63, 0x5c, 0x7b, 0x8 } };
//RePlayReel date (с патча 1.2)
// {BDFBD2AD-F686-4dd1-A5E1-9544C782AAB1}
//static const GUID filePlayReelID = { 0xbdfbd2ad, 0xf686, 0x4dd1, { 0xa5, 0xe1, 0x95, 0x44, 0xc7, 0x82, 0xaa, 0xb1 } };

#define FILE_REPLAY_MAGIC_LEN 20
static const char filePlayReelID[FILE_REPLAY_MAGIC_LEN] = "PerimeterReplay002\0";

#ifdef PERIMETER_DEBUG
class cMonowideFont {
	cFont* pfont;
public:
	cMonowideFont(){
		pfont=0;
	}
	~cMonowideFont(){
		if(pfont) pfont->Release();
	}
	cFont* getFont(){
		if(!pfont) {
			pfont= terVisGeneric->CreateGameFont("Courier New", 16, 1);//Тихая ошибка
			if(!pfont)pfont= terVisGeneric->CreateGameFont("Arial", 16);
		}
		return pfont;
	}
};
cMonowideFont* pMonowideFont;
#endif //PERIMETER_DEBUG

bool checkPlayReelMagic(XStream& fi) {
    char magic[FILE_REPLAY_MAGIC_LEN];
    fi.read(&magic, FILE_REPLAY_MAGIC_LEN);
    return strncmp(filePlayReelID, magic, FILE_REPLAY_MAGIC_LEN) == 0;
}

terHyperSpace::terHyperSpace(PNetCenter* net_client, MissionDescription& mission)
{
	m_FullListGameCommandLock = SDL_CreateMutex();

	flag_HostMigrate=false;
	flag_stopSavePlayReel=false;
	netPause=true;

	pNetCenter = net_client;

	if(pNetCenter) {
		if(IniManager("Network.ini").getInt("General","NetLog")) net_log_mode=1;
	}
	else net_log_mode=0;

	m_nLastPlayerReceived = 0;

	TimeOffset = 0;

	EmptyCount = 0;

	EventTime = 0;
	EventDeltaTime = 0;
	RealMaxEventTime = MaxEventTime = 0;
	EventWaitTime = 0;

	AverageEventTime = 0;
	AverageEventCount = 0;
	AverageEventSum = 0;
	AverageDampDelta = 10;

	MaxCorrectionTime = 3;
	MaxDeltaCorrectionTime = 5.0f;
	MaxCorrection = 3.0f;

	MinCorrectionTime = 1;
	MinDeltaCorrectionTime = 5.0f;
	MinCorrection = -0.5f;

	StopServerTime = 30;
	StopServerMode = 0;

	ServerSpeedScale = 1.0f;


	setLogicFp();

	currentQuant=0;
	//flag_endCurQuant=0;
	lagQuant=0;
	dropQuant=0;
	confirmQuant=0;
	signatureGame=startCRC32;

#ifdef PERIMETER_DEBUG
	pMonowideFont = new cMonowideFont();
#endif

	//Очистка списков команд
	{
		//Lock!
		CAutoLock lock(m_FullListGameCommandLock);
		clearListGameCommands(fullListGameCommands);
	}
	lastQuant_inFullListGameCommands=0;
	clearListGameCommands(replayListGameCommands);
	endQuant_inReplayListGameCommands=0;


	clientGeneralCommandCounterInListCommand=0;
	lastRealizedQuant=0;
	allowedRealizingQuant=0;

	lastQuantAllowedTimeCommand=0;
	generalCommandCounter4TimeCommand=0;

	//

	flag_autoSavePlayReel=flag_rePlayReel=flag_savePlayReel=false;
//	if(check_command_line(KEY_SAVE_PLAY_REEL)){
		flag_savePlayReel=true;
//	}
	if(mission.gameType_ == GT_PLAY_RELL){
		flag_rePlayReel=true;
		loadPlayReel(mission.playReelPath().c_str());
	}
	flag_autoSavePlayReel = IniManager("Perimeter.ini").getInt("Game","AutoSavePlayReel") != 0;

	currentQuant=0;
	//flag_endCurQuant=0;

	curGameComPosition=0;

	net_log_buffer.init();

    if (!mission.gameContent) {
        mission.gameContent = mission.isCampaign() ? getGameContentCampaign() : terGameContentSelect;
    }

	//if(flag_rePlayReel) mission=curMission; //Порядок Важен!
	if(flag_savePlayReel || flag_autoSavePlayReel) curMission=mission;

	vMap.selectUsedWorld(mission.worldID());
	vMap.fullLoad(IniManager("Perimeter.ini").getInt("TD","FastLoad"));
}

void getMissionDescriptionInThePlayReelFile(const char* fname, MissionDescription& md)
{
	XStream fi(convert_path_content(fname).c_str(), XS_IN);

	if(!checkPlayReelMagic(fi)) ErrH.Abort("Incorrect play reel file!:", XERR_USER, 0, fname);

	int sizeOtherData=fi.size()-fi.tell();
	XBuffer mdBuf(sizeOtherData);
	fi.read(mdBuf.address(), sizeOtherData);
	md.read(mdBuf);

	unsigned int quantAmount;
	mdBuf.read(&quantAmount, sizeof(quantAmount));
}

bool isCorrectPlayReelFile(const char* fname)
{
    std::string path = convert_path_content(fname);
    if (path.empty()) return false;
	XStream fi(false);
    if (!fi.open(path, XS_IN)) return false;
    return checkPlayReelMagic(fi);
}

size_t terHyperSpace::serializeGameCommands(XBuffer& out) const {
    //Store the accumulated quants from replay + current one
    unsigned int quants = endQuant_inReplayListGameCommands + lastQuant_inFullListGameCommands;
    size_t result = out.write(&quants, sizeof(quants));

    InOutNetComBuffer out_buffer(1024, true);
    //First we write any previously saved commands from before save
    for (auto p : replayListGameCommands) {
        out_buffer.putNetCommand(p);
    }
    //We save the actions done since loading game, applying offset of replay
    {
        //Lock!
        CAutoLock lock(m_FullListGameCommandLock);
        for (auto p: fullListGameCommands) {
            int ccq = p->curCommandQuant_;
            int ccc = p->curCommandCounter_;
            //We shouldnt be modifying the game commands inside const func before writing but we make a pinky swear that we leave them as is after
            p->setCurCommandQuantAndCounter(ccq + endQuant_inReplayListGameCommands, ccc);
            out_buffer.putNetCommand(p);
            p->setCurCommandQuantAndCounter(ccq, ccc);
        }
    }

    result += out.write(out_buffer.buf, out_buffer.filled_size);
    out_buffer.init();
    out_buffer.reset();
    return result;
}

void terHyperSpace::deserializeGameCommands(XBuffer& in, size_t len) {
    len -= in.read(&endQuant_inReplayListGameCommands, sizeof(endQuant_inReplayListGameCommands));
    xassert(len >= 0);
    
    InOutNetComBuffer in_buffer(len, true); //проверить необходимость автоувелечения!
    in_buffer.putBufferPacket(in.address()+in.tell(), len);

    while(in_buffer.currentNetCommandID()!=NETCOM_ID_NONE) {
        terEventID event = (terEventID)in_buffer.currentNetCommandID();
        switch(event){
            case NETCOM_4G_ID_UNIT_COMMAND:
            {
                netCommand4G_UnitCommand*  pnc= new netCommand4G_UnitCommand(in_buffer);
                replayListGameCommands.push_back(pnc);
            }
                break;
            case NETCOM_4G_ID_REGION:
            {
                netCommand4G_Region*  pnc= new netCommand4G_Region(in_buffer);
                replayListGameCommands.push_back(pnc);
            }
                break;
            case NETCOM_4G_ID_FORCED_DEFEAT:
            {
                netCommand4G_ForcedDefeat* pnc=new netCommand4G_ForcedDefeat(in_buffer);
                replayListGameCommands.push_back(pnc);
                break;
            }

            default:
                xassert(0&&"Incorrect commanf in playReel file!");
                break;
        }
        in_buffer.nextNetCommand();
    }
}

bool terHyperSpace::loadPlayReel(const char* fname)
{
	XStream fi(convert_path_content(fname), XS_IN);

	if(!checkPlayReelMagic(fi)) ErrH.Abort("Incorrect play reel file!:", XERR_USER, 0, fname);

	int sizeOtherData=fi.size()-fi.tell();
	XBuffer buf(sizeOtherData, true);
	fi.read(buf.address(), sizeOtherData);
	//curMission.read(mdBuf);
	MissionDescription temp;
	temp.read(buf);
    
    size_t sizePlayReelData = sizeOtherData - buf.tell();
    deserializeGameCommands(buf, sizePlayReelData);

	curRePlayPosition=replayListGameCommands.begin();
	return true;
}

terHyperSpace::SAVE_REPLAY_RESULT terHyperSpace::savePlayReel(const char* fname) const {

	XStream fo(0);
    std::string path = convert_path_content(fname, true);
    if (path.empty()) path = fname;
	if(!fo.open(path, XS_OUT)){
		return SAVE_REPLAY_RW_ERROR;
	}

	fo.write(&filePlayReelID, sizeof(filePlayReelID));
	if (fo.ioError()) {
		return SAVE_REPLAY_RW_ERROR_OR_DISK_FULL;
	}

	XBuffer buf(1024, true);
    //Copy current mission
	MissionDescription md = gameShell->CurrentMission;
    //Set original save of this mission for replay, so the replay starts with initial state and all actions can be applied
    md.setSaveName(md.originalSaveName);
    
    //Write mission header
    md.write(buf);
	fo.write(buf.address(), buf.tell());
	if (fo.ioError()) {
		return SAVE_REPLAY_RW_ERROR_OR_DISK_FULL;
	}

    //Write game commands
    buf.init();
    serializeGameCommands(buf);
    fo.write(buf.address(), buf.tell());
    
    if (fo.ioError()) {
        return SAVE_REPLAY_RW_ERROR_OR_DISK_FULL;
    } else {
        scan_resource_paths(convert_path_content(autoSavePlayReelDir));
        return SAVE_REPLAY_OK;
    }
}



void terHyperSpace::autoSavePlayReel()
{
	//autosave
    time_t result = time(nullptr);
    std::string name = gameShell->CurrentMission.missionName();
    if (name.empty()) name = gameShell->CurrentMission.worldName();
    std::string path = std::string(autoSavePlayReelDir) + PATH_SEP + "autosave_" + name;
    for (int i = 0; i < gameShell->CurrentMission.playerAmountScenarioMax; ++i) {
        PlayerData& p = gameShell->CurrentMission.playersData[i];
        if (p.realPlayerType == REAL_PLAYER_TYPE_PLAYER || p.realPlayerType == REAL_PLAYER_TYPE_PLAYER_AI) {
            std::string conv = convertToUnicode(p.name(), getLocale());
            path += std::string("_") + conv;
        }
    }
    path += "_" + std::to_string(result);
	savePlayReel(path.c_str());
}

void terHyperSpace::allSavePlayReel()
{
	if(flag_savePlayReel){
		const char* fname=check_command_line("saveplay");
		if(fname) savePlayReel(fname);//savePlay to file
	}

	if(flag_autoSavePlayReel){
		autoSavePlayReel();
	}
}

terHyperSpace::~terHyperSpace()
{
	//Очистка логов
	clearLogList();
	//Очистка списков команд
	{
		//Lock!
		CAutoLock lock(m_FullListGameCommandLock);
		clearListGameCommands(fullListGameCommands);
	}
	lastQuant_inFullListGameCommands=0;
	clearListGameCommands(replayListGameCommands);
	endQuant_inReplayListGameCommands=0;


#ifdef PERIMETER_DEBUG
	delete pMonowideFont;
#endif
	SDL_DestroyMutex(m_FullListGameCommandLock);

}

bool terHyperSpace::PrimaryQuant()
{
	if(!pNetCenter)
		return SingleQuant();
	else
		return MultiQuant();
}


size_t terHyperSpace::getInternalLagQuant(void)
{
	return allowedRealizingQuant-currentQuant;
}

bool terHyperSpace::MultiQuant()
{
	setLogicFp();

	if(allowedRealizingQuant > lastRealizedQuant){
        log_var("============== MultiQuant ==============");
        
		//Начало кванта
		currentQuant++;		//currentQuant=lastQuant_inFullListGameCommands;

		lastQuant_inFullListGameCommands=currentQuant;
		//поиск первой команды в последнем кванте
		std::vector<netCommandGame*>::iterator p;
		p=fullListGameCommands.end();
		while(p!=fullListGameCommands.begin()) {
			p--;
			if( (*p)->curCommandQuant_ < currentQuant ){
				p++; break;
			}
		}
		// p сейчас указывает на первую команду кванта
		for(; p!=fullListGameCommands.end(); p++) {
			if((*p)->curCommandQuant_ != currentQuant ) break; //проверка на конец команд кванта
			if((*p)->EventID==NETCOM_4G_ID_UNIT_COMMAND){
				netCommand4G_UnitCommand* pNC_UnitCommant=dynamic_cast<netCommand4G_UnitCommand*>(*p);
				receiveCommand(*pNC_UnitCommant);
                log_var(pNC_UnitCommant->PlayerID_);
				log_var(pNC_UnitCommant->unitCommand().commandID());
				log_var(pNC_UnitCommant->unitCommand().position());
			}
			else if((*p)->EventID==NETCOM_4G_ID_REGION){
				netCommand4G_Region* pNC_Region=dynamic_cast<netCommand4G_Region*>(*p);
				receiveCommand(*pNC_Region);
				log_var(pNC_Region->PlayerID_);
			}
			else if((*p)->EventID==NETCOM_4G_ID_FORCED_DEFEAT){
				///
				netCommand4G_ForcedDefeat* pNC_ForceDefeate=dynamic_cast<netCommand4G_ForcedDefeat*>(*p);
				forcedDefeat(pNC_ForceDefeate->PlayerID_);
				log_var(pNC_ForceDefeate->PlayerID_);
			}
			else xassert(0&&"Incorrect net comman in fullListGameCommands");
		}

		lagQuant=getInternalLagQuant(); //Для визуализации

		///xassert(currentQuant==lastQuant_inFullListGameCommands);
		//net_log_buffer.init();
		log_var(currentQuant);

		//flag_endCurQuant=0;

		Quant();

		//XBuffer vmapbuf(4096, 1);
		//vMap.generateChAreasInformation(vmapbuf);
		//pNetCenter->SendEvent(&netCommand4H_BackGameInformation(currentQuant, vmapbuf, net_log_buffer));

		//конец кванта
		///unsigned int gridCRC=vMap.getGridCRC(false, currentQuant);
		log_var(vMap.getGridCRC(false, currentQuant));
		///XBuffer vmapbuf(4096, 1);
		///vMap.generateChAreasInformation(vmapbuf);
		///pNetCenter->SendEvent(&netCommand4H_BackGameInformation(currentQuant, vmapbuf, net_log_buffer));

#if defined(PERIMETER_DEBUG) || defined(NET_LOG_EXHAUSTIVE)
		log_var(vMap.getChAreasInformationCRC());
#endif
        
#ifdef NET_LOG_EXHAUSTIVE
        log_var(vMap.getWorldCRC());
#endif

		logQuant();

		lastRealizedQuant=currentQuant;
		return true;
	}
	else {
		dropQuant++;
		return false;
	}

}

//power of two / степень двойки!
#ifdef PERIMETER_DEBUG
const unsigned int periodSendLogQuant=1;
#else
const unsigned int periodSendLogQuant=8;
#endif
const unsigned int maskPeriodSendLogQuant=periodSendLogQuant-1;//
void terHyperSpace::logQuant()
{
	lagQuant=getInternalLagQuant(); //Для сервера

    signatureGame=crc32((unsigned char*)net_log_buffer.address(), net_log_buffer.tell(), signatureGame);
	if((currentQuant & maskPeriodSendLogQuant)==0){ //Каждый 8 квант отсылается сигнатура
        //Basic cheap check for desync
        XBuffer rnd;
        rnd < terLogicRNDfrand();
        signatureGame=crc32((unsigned char*)rnd.address(), rnd.tell(), signatureGame);
        
        netCommand4H_BackGameInformation2 event = netCommand4H_BackGameInformation2(lagQuant, currentQuant, signatureGame, false, pNetCenter->m_state);
		pNetCenter->SendEvent(&event);
		signatureGame=startCRC32;
	}
	pushBackLogList(currentQuant, net_log_buffer);
	//sendLog(currentQuant);
	net_log_buffer.init();
}
void terHyperSpace::sendLog(unsigned int quant)
{
	xassert(quant);//Проверка, что не нулевой квант(кванты начинаются с 1-цы!)
	if(quant && ((quant & maskPeriodSendLogQuant)==0) ){
		unsigned int begLogQuant=((quant-1) & (~maskPeriodSendLogQuant)) +1;
		unsigned int sgn=startCRC32;
		int i;
		for(i=begLogQuant; i<=quant; i++){
			sLogElement* pLogElemente=getLogElement(i);
			xassert(pLogElemente);
			sgn=crc32((unsigned char*)pLogElemente->pLog->address(), pLogElemente->pLog->tell(), sgn);

		}
        netCommand4H_BackGameInformation2 event = netCommand4H_BackGameInformation2(0, quant, sgn, true, pNetCenter->m_state);
        pNetCenter->SendEvent(&event);
	}
}


// netCommand4G_Region from graphics thread, netCommand4G_UnitCommand from logic thread
void terHyperSpace::sendCommand(netCommandGame* command) 
{
    if(!pNetCenter){
        //Lock!
        CAutoLock lock(m_FullListGameCommandLock);
        
        command->setCurCommandQuantAndCounter(currentQuant+1,0);
        fullListGameCommands.push_back(command);
        /////receiveCommand(command);
    } else {
        pNetCenter->SendEvent(command);
        delete command;
    }
}

//----------------------------- Dread Place ----------------------------
bool terHyperSpace::SingleQuant()
{
	setLogicFp();

	//if(allowedRealizingQuant > lastRealizedQuant)
	//Начало кванта
	currentQuant++;
	if(!flag_stopSavePlayReel) lastQuant_inFullListGameCommands=currentQuant;

	if(flag_rePlayReel){
		std::vector<netCommandGame*>::iterator p;
		
		for(p=curRePlayPosition; p!=replayListGameCommands.end(); p++){
			if((*p)->curCommandQuant_==currentQuant){
				if((*p)->EventID==NETCOM_4G_ID_UNIT_COMMAND){
					netCommand4G_UnitCommand* pNC_UnitCommant=dynamic_cast<netCommand4G_UnitCommand*>(*p);
					receiveCommand(*pNC_UnitCommant);
                    log_var(pNC_UnitCommant->PlayerID_);
					log_var(pNC_UnitCommant->unitCommand().commandID());
					log_var(pNC_UnitCommant->unitCommand().position());
				}
				else if((*p)->EventID==NETCOM_4G_ID_REGION){
					netCommand4G_Region* pNC_Region=dynamic_cast<netCommand4G_Region*>(*p);
					receiveCommand(*pNC_Region);
					log_var(pNC_Region->PlayerID_);
				}
				else if((*p)->EventID==NETCOM_4G_ID_FORCED_DEFEAT){
					netCommand4G_ForcedDefeat* pNC_ForceDefeate=dynamic_cast<netCommand4G_ForcedDefeat*>(*p);
					forcedDefeat(pNC_ForceDefeate->PlayerID_);
					log_var(pNC_ForceDefeate->PlayerID_);
				}

				else xassert(0&&"Incorrect net comman in replayListGameCommands");
			}
			else {
				curRePlayPosition=p;
				break;
			}
		}

		if(currentQuant > endQuant_inReplayListGameCommands)
			_pShellDispatcher->OnInterfaceMessage(UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT); 
	}
	else {
		//Lock!
		CAutoLock lock(m_FullListGameCommandLock);

		for(curGameComPosition; curGameComPosition<fullListGameCommands.size(); curGameComPosition++){
			if(fullListGameCommands[curGameComPosition]->curCommandQuant_==currentQuant){
				if(fullListGameCommands[curGameComPosition]->EventID==NETCOM_4G_ID_UNIT_COMMAND){
					netCommand4G_UnitCommand* pNC_UnitCommant=dynamic_cast<netCommand4G_UnitCommand*>(fullListGameCommands[curGameComPosition]);
					receiveCommand(*pNC_UnitCommant);
                    log_var(pNC_UnitCommant->PlayerID_);
					log_var(pNC_UnitCommant->unitCommand().commandID());
					log_var(pNC_UnitCommant->unitCommand().position());
				}
				else if(fullListGameCommands[curGameComPosition]->EventID==NETCOM_4G_ID_REGION){
					netCommand4G_Region* pNC_Region=dynamic_cast<netCommand4G_Region*>(fullListGameCommands[curGameComPosition]);
					receiveCommand(*pNC_Region);
					log_var(pNC_Region->PlayerID_);
				}
				else if(fullListGameCommands[curGameComPosition]->EventID==NETCOM_4G_ID_FORCED_DEFEAT){
					netCommand4G_ForcedDefeat* pNC_ForceDefeate=dynamic_cast<netCommand4G_ForcedDefeat*>(fullListGameCommands[curGameComPosition]);
					forcedDefeat(pNC_ForceDefeate->PlayerID_);
					log_var(pNC_ForceDefeate->PlayerID_);
				}
				else xassert(0&&"Incorrect net comman in fullListGameCommands");
			}
			else {
				xassert("Strange error!");
			}
		}
	}

	log_var(currentQuant);

	Quant();

	//конец кванта
	///unsigned int gridCRC=vMap.getGridCRC(false, currentQuant);
	log_var(vMap.getGridCRC(false, currentQuant));

	allowedRealizingQuant=currentQuant+1;

	lastRealizedQuant=currentQuant;

	return true;
}

//void terHyperSpace::Start(MissionDescription& mission, const char* fnamePlayReel)
//{
//}

std::string terHyperSpace::GetNetInfo() {
    static int rb = 0, sb = 0;
    static int lastInfoQuant = 0;
    static int lastInfoQuantTime = 0;
    static int quantPerSec = 0;

    int secondQuant = currentQuant/10;
    if (lastInfoQuant < secondQuant) {
        lastInfoQuant=secondQuant;
        rb=pNetCenter->in_ClientBuf.getByteReceive();
        sb=pNetCenter->out_ClientBuf.getByteSending();
        quantPerSec=10*1000/(clocki()-lastInfoQuantTime);
        lastInfoQuantTime=clocki();
    }

    std::string msg = "Net state: ";
    msg += pNetCenter->getStrState();
    msg += "\nBytes recv: " + std::to_string(rb) + "\nBytes sent: " + std::to_string(sb);
    msg += "\n\nQuants:\n current: " + std::to_string(currentQuant);
    msg += "\n confirm: " + std::to_string(confirmQuant);
    msg += "\n diff: " + std::to_string(currentQuant - confirmQuant);
    msg += "\n lag: " + std::to_string(lagQuant);
    msg += "\n drop: " + std::to_string(dropQuant);
    msg += "\n per second: " + std::to_string(quantPerSec);
    
    return msg;
}

//По идее вызов корректный т.к. reJoin не пошлется пока игра не остановлена(stopGame_HostMigrate)
void terHyperSpace::sendListGameCommand2Host(unsigned int begQuant, unsigned int endQuant)
{
	std::vector<netCommandGame*>::iterator p;
	InOutNetComBuffer out_buffer(1024,1);
	if(lastQuant_inFullListGameCommands < endQuant) endQuant=lastQuant_inFullListGameCommands; /// 
	for(p=fullListGameCommands.begin(); p!=fullListGameCommands.end(); p++){
		xassert( ((*p)->EventID==NETCOM_4G_ID_UNIT_COMMAND) || ((*p)->EventID==NETCOM_4G_ID_REGION) || ((*p)->EventID==NETCOM_4G_ID_FORCED_DEFEAT) );
		if( ((*p)->curCommandQuant_ >=begQuant) && ((*p)->curCommandQuant_ <=endQuant))
			out_buffer.putNetCommand((*p));
	}
	netCommand4H_ResponceLastQuantsCommands nco(begQuant, endQuant, clientGeneralCommandCounterInListCommand, out_buffer.filled_size, out_buffer.address() );
	if(pNetCenter){
		pNetCenter->SendEvent(&nco);
	}

}


void terHyperSpace::stopGame_HostMigrate()
{
	///clearLastQuantListGameCommand();
	//Очистка всех не выполненных команд из списка
//	vector<netCommandGame*>::reverse_iterator p;
//	for(p=fullListGameCommands.rbegin(); p!=fullListGameCommands.rend(); ){
//		if((*p)->curCommandQuant_ > lastQuant_inFullListGameCommands) {
//			delete *p;
//			fullListGameCommands.erase(p.base());
//			p=fullListGameCommands.rbegin();
//		}
//		else break;//p++;
//	}

	xassert(allowedRealizingQuant==lastRealizedQuant);
	xassert(allowedRealizingQuant==lastQuant_inFullListGameCommands);
	std::vector<netCommandGame*>::iterator p;
	while(!fullListGameCommands.empty()){
		p=fullListGameCommands.end();
		p--;
		if((*p)->curCommandQuant_ > lastQuant_inFullListGameCommands){
			delete *p;
			fullListGameCommands.erase(p);
			clientGeneralCommandCounterInListCommand--;
		}
		else break;
	}

	lastQuantAllowedTimeCommand=0;
	generalCommandCounter4TimeCommand=0;
	allowedRealizingQuant=lastQuant_inFullListGameCommands;

	xassert(clientGeneralCommandCounterInListCommand==fullListGameCommands.size());
	//clientGeneralCommandCounterInListCommand;

	//clientGeneralCommandCounterInListCommand   ---- fullListGameCommands.push_back(pnc);
	//allowedRealizingQuant
	flag_HostMigrate=true;
}

void terHyperSpace::putInputGameCommand2fullListGameCommandAndCheckAllowedRun(netCommandGame* pnc)
{
	///if(pnc->curCommandQuant_ >= lastQuant_inFullListGameCommands) //В случае смены хоста - пропускать выполненные комманды
	if(pnc->curCommandQuant_ > allowedRealizingQuant) {//В случае смены хоста - пропускать выполненные комманды
		fullListGameCommands.push_back(pnc);
		xassert(pnc->curCommandCounter_==clientGeneralCommandCounterInListCommand);
		clientGeneralCommandCounterInListCommand++;
//		receiveCommand(*pnc);

		//Разрешение проигрывания комманд
		if(pnc->flag_lastCommandInQuant_){ //Комманда была последняя в кванте, поэтому разрешаем проигрывать до кванта включительно
			allowedRealizingQuant=pnc->curCommandQuant_;
		}
		else { //Комманда была не последняя в кванте, поэтому разрешаем проигрывать до предыдущего кванта
			if(pnc->curCommandQuant_>0){
				allowedRealizingQuant=pnc->curCommandQuant_-1;
			}
		} 
		//Если количество комманд совпадает со счетчиком команд в последней NEXT_COMMAND(TimeCommand)
		// и при этом более позднее чем разрешенное сейчас  то разрешается проигрывание до кванта указанного в последней TimeComand-е
		if( (clientGeneralCommandCounterInListCommand==generalCommandCounter4TimeCommand) &&
			(lastQuantAllowedTimeCommand > allowedRealizingQuant) ){
			allowedRealizingQuant=lastQuantAllowedTimeCommand;
		}
	}
	else {
		xassert(0&&"host migrate ?");
		//можно вставить поиск в списке и подтверждение
		std::vector<netCommandGame*>::iterator p;
		for(p=fullListGameCommands.begin(); p!=fullListGameCommands.end(); p++){
			if((*p)->curCommandCounter_==pnc->curCommandCounter_) break;
		}
		if(p!=fullListGameCommands.end()){
			if(**p==*pnc){
				//good
			}
			else {
				xassert(0&&"Command not compare!(by HostMigrate!)");
			}
		}
		else{
			xassert(0&&"Command not found!(by HostMigrate!)");
		}
	}
}


//#define NETCOM_DBG_LOG
#ifdef NETCOM_DBG_LOG
XStream netCommandLog("netcommandlog.log", XS_OUT);
#endif

bool terHyperSpace::ReceiveEvent(terEventID event, InOutNetComBuffer& in_buffer)
{
//	if(flag_endCurQuant==false){
		switch(event) {
/*		case NETCOM_ID_START_GAME: {
			netCommandStartGame event(in_buffer);
#ifdef NETCOM_DBG_LOG
			netCommandLog < "NETCOM_START_GAME\n";
#endif
			//Start();
			break;
			}
			*/			
		case NETCOM_4C_ID_SEND_LOG_2_HOST:
			{
				xassert(flag_HostMigrate);
				netCommand4C_sendLog2Host nc(in_buffer);
				unsigned int i;
				for(i=nc.begQuant; i<=lastQuant_inFullListGameCommands; i++)
					sendLog(i);
				flag_HostMigrate=false;

			}
			break;
		case NETCOM_ID_NEXT_QUANT:
			{
				xassert(!flag_HostMigrate);
				netCommandNextQuant nc(in_buffer);
				if(nc.numberQuant_ > lastQuantAllowedTimeCommand){
					lastQuantAllowedTimeCommand=nc.numberQuant_;
					generalCommandCounter4TimeCommand=nc.globalCommandCounter_;
				}
				if(clientGeneralCommandCounterInListCommand==generalCommandCounter4TimeCommand){
					allowedRealizingQuant=lastQuantAllowedTimeCommand;
				}
				//Не работает ! 
				/*if(nc.numberQuant_ <= lastQuant_inFullListGameCommands) {
					XBuffer* pCurLogQuant=getLogInLogList(nc.numberQuant_); //В случае смены хоста - пропускать выполненные комманды но отсылать backGameInformation
					if(pCurLogQuant){
						unsigned int signature=crc32((unsigned char*)pCurLogQuant->address(), pCurLogQuant->tell(), startCRC32);
						pNetCenter->SendEvent(&netCommand4H_BackGameInformation2(0, nc.numberQuant_, signature));
					}
					else xassert(0&&"No log(after migrate host)!");
					break;
				}*/
				if(nc.quantConfirmation_!=netCommandNextQuant::NOT_QUANT_CONFIRMATION){ //Сейчас quantConfirmation_ посылается во всех коммандах
					confirmQuant=nc.quantConfirmation_;
					//clear list 
					eraseLogListUntil(nc.quantConfirmation_);
				}
	#ifdef NETCOM_DBG_LOG
				netCommandLog < "Quant=" <=nc.numberQuant_ <"\n";
	#endif
				///xassert(nc.numberQuant_==(lastQuant_inFullListGameCommands+1));//Проверка на пропуск кванта(в случае выпадения!? пакета) PS. сейчас они выпадают!

				netPause=nc.flag_pause_;
			}
			break;


		case NETCOM_4G_ID_UNIT_COMMAND: 
			{
				xassert(!flag_HostMigrate);
				netCommand4G_UnitCommand*  pnc= new netCommand4G_UnitCommand(in_buffer);
				//xassert(pnc->curCommandQuant_ < 0xcdcd0000);
				putInputGameCommand2fullListGameCommandAndCheckAllowedRun(pnc);
			}
			break;
		case NETCOM_4G_ID_REGION: 
			{
				xassert(!flag_HostMigrate);
				netCommand4G_Region*  pnc= new netCommand4G_Region(in_buffer);
				//xassert(pnc->curCommandQuant_ < 0xcdcd0000);
				putInputGameCommand2fullListGameCommandAndCheckAllowedRun(pnc);
			}
			break;
		case NETCOM_4G_ID_FORCED_DEFEAT:
			{
				xassert(!flag_HostMigrate);
				netCommand4G_ForcedDefeat*  pnc= new netCommand4G_ForcedDefeat(in_buffer);
				//xassert(pnc->curCommandQuant_ < 0xcdcd0000);
				putInputGameCommand2fullListGameCommandAndCheckAllowedRun(pnc);
			}
			break;
		case EVENT_ID_SERVER_TIME_CONTROL: {
			terEventControlServerTime event(in_buffer);
			SetServerSpeedScale(event.scale);
			break;
			}

		default:
			//return false;
			xassert("Invalid netCommand to client");
			in_buffer.ignoreNetCommand();
			break;
		}
		return true;
//	}
//	else {
//		return false;
//	}

}
