#ifndef __EVENT_BUFFER_DP
#define __EVENT_BUFFER_DP

#include "CommonEvents.h"

class PNetCenter;
class InOutNetComBuffer : public XBuffer 
{
	unsigned long byte_receive;//in
	unsigned long byte_sending;//out
public:
	terEventID event_ID;//in

	unsigned long size_of_event;//in
	unsigned int next_event_pointer;//in
public:
	unsigned int filled_size;//in
	InOutNetComBuffer(unsigned int size, bool autoRealloc);

	void clearBufferOfTheProcessedCommands(void);//out
	int send(PNetCenter& conn, NETID netid);//out
	unsigned long getByteSending(){//out
		unsigned long result=byte_sending;
		byte_sending=0;
		return result;
	}
	unsigned long getByteReceive(){
		unsigned long result=byte_receive;
		byte_receive=0;
		return result;
	}
	void reset();//?
	int write2File(XStream& fout);
	void putNetCommand(const netCommandGeneral* event);//out
	bool putBufferPacket(char* buf, unsigned int size);//in
	int currentNetCommandID();//in
	terEventID nextNetCommand();//in
	void ignoreNetCommand();//in
	void backNetCommand();//in
	unsigned long getQuantAmount();//in
	bool isEmpty(void){
		return filled_size <= 0; // подразумевается ==
	}

};
#endif //__EVENT_BUFFER_DP




