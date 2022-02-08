#ifndef __EVENT_BUFFER_DP
#define __EVENT_BUFFER_DP

#include "CommonEvents.h"

typedef uint32_t event_size_t;

class PNetCenter;
class InOutNetComBuffer : public XBuffer 
{
	size_t byte_receive;//in
	size_t byte_sending;//out
public:
	terEventID event_ID;//in

	event_size_t size_of_event;//in
	size_t next_event_pointer;//in
public:
	size_t filled_size;//in
	InOutNetComBuffer(unsigned int size, bool autoRealloc);
    InOutNetComBuffer(void* p, size_t sz);

	void clearBufferOfTheProcessedCommands(void);//out
	int send(PNetCenter& conn, NETID netid);//out
	size_t getByteSending(){//out
        size_t result=byte_sending;
		byte_sending=0;
		return result;
	}
	size_t getByteReceive(){
		size_t result=byte_receive;
		byte_receive=0;
		return result;
	}
	void reset();//?
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




