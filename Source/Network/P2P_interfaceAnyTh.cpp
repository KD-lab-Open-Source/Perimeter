#include "NetIncludes.h"
#include "P2P_interface.h"
#include "NetConnectionAux.h"
#include "../HT/mt_config.h"

extern std::atomic_uint64_t net_thread_id;

//Запускается из 1 2-го потока
//Может вызываться с фдагом waitExecution только из одного потока (сейчас 1-го)
//Runs from 1st and 2nd thread
//Can be called with the waitExecution flag from only one thread (now the 1st one) 
bool PNetCenter::ExecuteInternalCommand(e_PNCInternalCommand ic, bool waitExecution)
{
	if( WaitForSingleObject(hSecondThread, 0) == WAIT_OBJECT_0) {
		return false;
	}

	if(waitExecution) ResetEvent(hCommandExecuted);
    internalCommandList.push_back(ic);

	if (waitExecution) {
        if (MTConfig::multithreading()) {
#ifdef PERIMETER_DEBUG
            //Ensure is not being called from net thread since it will deadlock
            xassert(net_thread_id != SDL_ThreadID() && "Waiting execution from net thread!");
#endif

            //if(WaitForSingleObject(hCommandExecuted, INFINITE) != WAIT_OBJECT_0) xassert(0&&"Error execute command");
            const unsigned char ha_size = 2;
            HANDLE ha[ha_size];
            ha[0] = hSecondThread;
            ha[1] = hCommandExecuted;
            uint32_t result = WaitForMultipleObjects(ha_size, ha, false, INFINITE);
            if (result >= (WAIT_OBJECT_0 + ha_size)) {
                xassert(0 && "Error execute command");
            }
        } else {
            while (!internalCommandList.empty()) {
                auto cmd = *internalCommandList.begin();
                SecondThreadQuant();
                if (cmd == ic) {
                    break;
                }
            }
        }
	}

	return true;
}


//Запускается из 1-го(деструктор) и 2-го потока
void PNetCenter::ClearClients()
{
	ClientMapType::iterator i;
	FOR_EACH(m_clients, i)
		delete *i;
	m_clients.clear();
}

//Запускается из 2 и 3-го потока
/// !!! педается указатель !!! удаление происходит автоматом после осылки !!!
void PNetCenter::PutGameCommand2Queue_andAutoDelete(NETID netid, netCommandGame* pCommand)
{
    //Ensure command is from correct sender
    if (pCommand->EventID != NETCOM_4G_ID_FORCED_DEFEAT) {
        unsigned int i = pCommand->PlayerID_;
        if (i >= hostMissionDescription->playerAmountScenarioMax
            || hostMissionDescription->playersData[i].netid != netid) {
            LogMsg("Discarding game command from incorrect netid 0x%" PRIX64 " to player %d\n", netid, i);
            delete pCommand;
            return;
        }
    }
    
	pCommand->setCurCommandQuantAndCounter(m_numberGameQuant, hostGeneralCommandCounter);
	m_CommandList.push_back(pCommand);
	hostGeneralCommandCounter++;
	m_nQuantCommandCounter++;
}

void PNetCenter::ClearQueuedGameCommands()
{
    for(auto p=m_QueuedGameCommands.begin(); p != m_QueuedGameCommands.end(); p++){
        delete *p;
    }
    m_QueuedGameCommands.clear();
}


bool PNetCenter::ExecuteInterfaceCommand(e_PNCInterfaceCommands ic, std::unique_ptr<LocalizedText> text)
{
	{
		interfaceCommandList.push_back(new sPNCInterfaceCommand(ic, std::move(text)));
	}
	return 1;
}

void PNetCenter::ClearInputPacketList() {
    for (NetConnectionMessage* packet : m_InputPacketList) {
        delete packet;
    }
    m_InputPacketList.clear();
}

