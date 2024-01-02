#ifndef __EVENT_BUFFER_DP
#define __EVENT_BUFFER_DP

#include "CommonEvents.h"

typedef uint32_t event_size_t;
const uint32_t NETCOM_BUFFER_PACKET_ID = 0xB1FFE90D;
const unsigned int SIZE_NETCOM_PACKET_HEAD = sizeof(NETCOM_BUFFER_PACKET_ID) + sizeof(event_size_t) + sizeof(terEventID);

class PNetCenter;
class InOutNetComBuffer : public XBuffer 
{
public:
    //Used for stats
	size_t byte_receive;//in
	size_t byte_sending;//out

	terEventID event_ID;//in
	size_t next_event_pointer;//in
	size_t filled_size;//in
	InOutNetComBuffer(unsigned int size, bool autoRealloc);
    InOutNetComBuffer(void* p, size_t sz);

	void clearBufferOfTheProcessedCommands(void);//out
    int send(PNetCenter& conn, NETID netid);//out
	void reset();//?
    void reset_stats() {
        byte_receive = 0;
        byte_sending = 0;
    }
	void putNetCommand(const netCommandGeneral* event);//out
	bool putBufferPacket(char* buf, unsigned int size);//in
	int currentNetCommandID();//in
	terEventID nextNetCommand();//in
	void ignoreNetCommand();//in
	void backNetCommand();//in
    size_t getQuantAmount();//in
	bool isEmpty(void){
		return filled_size <= 0; // подразумевается ==
	}

};
#endif //__EVENT_BUFFER_DP




