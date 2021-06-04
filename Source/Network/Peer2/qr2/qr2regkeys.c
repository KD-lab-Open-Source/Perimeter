
#include "qr2regkeys.h"

const char *qr2_registered_key_list[MAX_REGISTERED_KEYS] =
{
	"", //0 is reserved
		"hostname",	//1
		"gamename",	//2
		"gamever",	//3
		"hostport",	//4
		"mapname",	//5
		"gametype",	//6
		"gamevariant",	//7
		"numplayers",	//8
		"numteams",	//9
		"maxplayers",	//10
		"gamemode",	//11
		"teamplay",	//12
		"fraglimit",	//13
		"teamfraglimit",//14
		"timeelapsed",	//15
		"timelimit",	//16
		"roundtime",	//17
		"roundelapsed",	//18
		"password",	//19
		"groupid",	//20
		"player_",	//21
		"score_",	//22
		"skill_",	//23
		"ping_",	//24
		"team_",	//25
		"deaths_",	//26
		"pid_",		//27
		"team_t",	//28
		"score_t"	//29
};


void qr2_register_key(int keyid, const char *key)
{
	if (keyid < NUM_RESERVED_KEYS || keyid > MAX_REGISTERED_KEYS)
		return;
	qr2_registered_key_list[keyid] = key;
}
