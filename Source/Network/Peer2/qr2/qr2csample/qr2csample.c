/******
qrcsample.c
GameSpy Query & Reporting SDK 
  
Copyright 2000 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, CA 92614
(949)798-4200
Fax(949)798-4299
******

 This source file includes a "sample" game shell which demonstrates
 how to use the GameSpy Query & Reporting SDK

 Please see the GameSpy Query & Reporting SDK documentation for more 
 information
******/


/********
INCLUDES
********/
#if defined(applec) || defined(THINK_C) || defined(__MWERKS__) && !defined(__mips64) && !defined(_WIN32)
	#include ":::nonport.h"
#else
	#include "../../nonport.h"
#endif

#ifdef _MACOS
	#include "::qr2.h"
#else
	#include "../qr2.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) && !defined(UNDER_CE)
#include <conio.h>
#endif

/********
DEFINES
********/
#define GAME_VERSION "2.00"
#define GAME_NAME "gmtest"
#define MAX_PLAYERS 32
#define MAX_TEAMS 2
#define BASE_PORT 26900

#ifdef _WIN32_WCE
void RetailOutputA(CHAR *tszErr, ...);
#define printf RetailOutputA
#endif

//define our additional keys
#define GRAVITY_KEY 100
#define RANKINGON_KEY 101
#define TIME__KEY 102
#define AVGPING_T_KEY 103

/********
TYPDEFS
********/
//representative of a game player structure
typedef struct
{
	char pname[80];
	int pfrags;
	int pdeaths;
	int ptime;
	int pping;
	int pteam;
} player_t;

//representative of a team structure
typedef struct
{
	char tname[80];
	int tscore;
	int avgping;

} team_t;

//representative of a game data structure
typedef struct
{
	player_t players[MAX_PLAYERS];
	team_t teams[MAX_TEAMS];
	char mapname[20];
	char hostname[120];
	char gamemode[200];
	char gametype[30];
	int numteams;
	int numplayers;
	int maxplayers;
	int fraglimit;
	int timelimit;
	int teamplay;
	int rankingson;
	int gravity;
	int hostport;
} gamedata_t;

/********
GLOBAL VARS
********/

//just to give us bogus data
char *constnames[MAX_PLAYERS]={"Joe Player","L33t 0n3","Raptor","Gr81","Flubber","Sarge","Void","runaway","Ph3ar","wh00t","gr1nder","Mace","stacy","lamby","Thrush"};
gamedata_t gamedata;

/********
PROTOTYPES - To prevent warnings on codewarrior strict compile
********/
void serverkey_callback(int keyid, qr2_buffer_t outbuf, void *userdata);
void playerkey_callback(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
void teamkey_callback(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
void keylist_callback(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata);
int count_callback(qr2_key_type keytype, void *userdata);
void adderror_callback(qr2_error_t error, char *errmsg, void *userdata);
void nn_callback(int cookie, void *userdata);
void cm_callback(char *data, int len, void *userdata);
void DoGameStuff(void);
int test_main(int argc, char **argp);

void serverkey_callback(int keyid, qr2_buffer_t outbuf, void *userdata)
{
	switch (keyid)
	{
	case HOSTNAME_KEY:
		qr2_buffer_add(outbuf, gamedata.hostname);
		break;
	case GAMEVER_KEY:
		qr2_buffer_add(outbuf, GAME_VERSION);
		break;
	case HOSTPORT_KEY:
		qr2_buffer_add_int(outbuf, gamedata.hostport);
		break;
	case MAPNAME_KEY:
		qr2_buffer_add(outbuf, gamedata.mapname);
		break;
	case GAMETYPE_KEY:
		qr2_buffer_add(outbuf, gamedata.gametype);
		break;
	case NUMPLAYERS_KEY:
		qr2_buffer_add_int(outbuf, gamedata.numplayers);
		break;
	case NUMTEAMS_KEY:
		qr2_buffer_add_int(outbuf, gamedata.numteams);
		break;
	case MAXPLAYERS_KEY:
		qr2_buffer_add_int(outbuf, gamedata.maxplayers);
		break;
	case GAMEMODE_KEY:
		qr2_buffer_add(outbuf, gamedata.gamemode);
		break;
	case TEAMPLAY_KEY:
		qr2_buffer_add_int(outbuf, gamedata.teamplay);
		break;
	case FRAGLIMIT_KEY:
		qr2_buffer_add_int(outbuf, gamedata.fraglimit);
		break;
	case TIMELIMIT_KEY:
		qr2_buffer_add_int(outbuf, gamedata.timelimit);
		break;
	case GRAVITY_KEY:
		qr2_buffer_add_int(outbuf, gamedata.gravity);
		break;
	case RANKINGON_KEY:
		qr2_buffer_add_int(outbuf, gamedata.rankingson);
		break;
	default:
		qr2_buffer_add(outbuf, "");
	}
	
	GSI_UNUSED(userdata);
}

void playerkey_callback(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
	//check for valid index
	if (index >= gamedata.numplayers)
	{
		qr2_buffer_add(outbuf, "");
		return;
	}
	switch (keyid)
	{
	case PLAYER__KEY:
		qr2_buffer_add(outbuf, gamedata.players[index].pname);
		break;
	case SCORE__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].pfrags);
		break;
	case DEATHS__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].pdeaths);
		break;
	case PING__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].pping);
		break;
	case TEAM__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].pteam);
		break;
	case TIME__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].ptime);
		break;
	default:
		qr2_buffer_add(outbuf, "");
		break;		
	}
	
	GSI_UNUSED(userdata);
}

void teamkey_callback(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
	//check for valid index
	if (index >= gamedata.numteams)
	{
		qr2_buffer_add(outbuf, "");
		return;
	}
	switch (keyid)
	{
	case TEAM_T_KEY:
		qr2_buffer_add(outbuf, gamedata.teams[index].tname);
		break;
	case SCORE_T_KEY:
		qr2_buffer_add_int(outbuf, gamedata.teams[index].tscore);
		break;
	case AVGPING_T_KEY:
		qr2_buffer_add_int(outbuf, gamedata.teams[index].avgping);
		break;
	default:
		qr2_buffer_add(outbuf, "");
		break;		
	}
	
	GSI_UNUSED(userdata);
}	

void keylist_callback(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata)
{
	//need to add all the keys we support
	switch (keytype)
	{
	case key_server:
		qr2_keybuffer_add(keybuffer, HOSTNAME_KEY);
		qr2_keybuffer_add(keybuffer, GAMEVER_KEY);
		qr2_keybuffer_add(keybuffer, HOSTPORT_KEY);
		qr2_keybuffer_add(keybuffer, MAPNAME_KEY);
		qr2_keybuffer_add(keybuffer, GAMETYPE_KEY);
		qr2_keybuffer_add(keybuffer, NUMPLAYERS_KEY);
		qr2_keybuffer_add(keybuffer, NUMTEAMS_KEY);
		qr2_keybuffer_add(keybuffer, MAXPLAYERS_KEY);
		qr2_keybuffer_add(keybuffer, GAMEMODE_KEY);
		qr2_keybuffer_add(keybuffer, TEAMPLAY_KEY);
		qr2_keybuffer_add(keybuffer, FRAGLIMIT_KEY);
		qr2_keybuffer_add(keybuffer, TIMELIMIT_KEY);
		qr2_keybuffer_add(keybuffer, GRAVITY_KEY); //a custom key
		qr2_keybuffer_add(keybuffer, RANKINGON_KEY); //a custom key
		break;
	case key_player:
		qr2_keybuffer_add(keybuffer, PLAYER__KEY);
		qr2_keybuffer_add(keybuffer, SCORE__KEY);
		qr2_keybuffer_add(keybuffer, DEATHS__KEY);
		qr2_keybuffer_add(keybuffer, PING__KEY);
		qr2_keybuffer_add(keybuffer, TEAM__KEY);
		qr2_keybuffer_add(keybuffer, TIME__KEY); //a custom key
		break;
	case key_team:
		qr2_keybuffer_add(keybuffer, TEAM_T_KEY);
		qr2_keybuffer_add(keybuffer, SCORE_T_KEY);
		qr2_keybuffer_add(keybuffer, AVGPING_T_KEY); //a custom key
		break;
	}
	
	GSI_UNUSED(userdata);
}

int count_callback(qr2_key_type keytype, void *userdata)
{
	if (keytype == key_player)
		return gamedata.numplayers;
	else if (keytype == key_team)
		return gamedata.numteams;
	else
		return 0;
		
	GSI_UNUSED(userdata);
}

void adderror_callback(qr2_error_t error, char *errmsg, void *userdata)
{
	printf("Error adding server: %d, %s\n", error, errmsg);
	
	GSI_UNUSED(userdata);
}

void nn_callback(int cookie, void *userdata)
{
	printf("Got natneg cookie: %d\n", cookie);
	
	GSI_UNUSED(userdata);
}

void cm_callback(char *data, int len, void *userdata)
{
	printf("Got %d bytes from client\n", len);
	
	GSI_UNUSED(data);
	GSI_UNUSED(userdata);
}

/***********
init_game
Initialize the sample data structures with bogus data
************/
static void init_game(void)
{
	int i;

	srand((unsigned int) current_time() );
	gamedata.numplayers = rand() % 15;
	gamedata.maxplayers = MAX_PLAYERS;
	for (i = 0 ; i < gamedata.numplayers ; i++)
	{
		strcpy(gamedata.players[i].pname, constnames[i]);
		gamedata.players[i].pfrags = rand() % 32;
		gamedata.players[i].pdeaths = rand() % 32;
		gamedata.players[i].ptime = rand() % 1000;
		gamedata.players[i].pping = rand() % 500;
		gamedata.players[i].pteam = rand() % 2;
	}
	gamedata.numteams = 2;
	for (i = 0 ; i < gamedata.numteams ; i++)
	{
		gamedata.teams[i].tscore = rand() % 500;
		gamedata.teams[i].avgping = rand() % 500;
	}
	strcpy(gamedata.teams[0].tname,"Red");
	strcpy(gamedata.teams[1].tname,"Blue");
	
	strcpy(gamedata.mapname,"gmtmap1");
	strcpy(gamedata.gametype,"arena");
	strcpy(gamedata.hostname,"GameMaster Arena Server");
	strcpy(gamedata.gamemode,"openplaying");
	gamedata.fraglimit = 0;
	gamedata.timelimit = 40;
	gamedata.teamplay = 1;
	gamedata.rankingson = 1;
	gamedata.gravity = 800;
	gamedata.hostport = 25000;
}

/*******
 DoGameStuff
Simulate whatever else a game server does 
********/
void DoGameStuff(void)
{
	msleep(10);
}

/*******************
 main
Simulates a main program loop
First, initializes the Q&R items, then enters a main loop
*****************/
#if defined(_PS2)
int test_main(int argc, char **argp)
#else
int main(int argc, char **argp)
#endif
{
	char secret_key[9];
	//set the secret key, in a semi-obfuscated manner
	secret_key[0] = 'H';
	secret_key[1] = 'A';
	secret_key[2] = '6';
	secret_key[3] = 'z';
	secret_key[4] = 'k';
	secret_key[5] = 'S';
	secret_key[6] = '\0';
	//register our custom keys
	qr2_register_key(GRAVITY_KEY, "gravity");
	qr2_register_key(RANKINGON_KEY, "rankingon");
	qr2_register_key(TIME__KEY, "time_");
	qr2_register_key(AVGPING_T_KEY, "avgping_t");

	init_game();
	

	//call qr_init with the query port number and gamename, default IP address, and no user data
	if (qr2_init(NULL,argc>1?argp[1]:NULL,BASE_PORT,GAME_NAME, secret_key, 1, 0,
		serverkey_callback, playerkey_callback, teamkey_callback,
		keylist_callback, count_callback, adderror_callback, NULL) != e_qrnoerror)
	{
		printf("Error starting query sockets\n");
		return -1;
	}

	qr2_register_clientmessage_callback(NULL, cm_callback);
	qr2_register_natneg_callback(NULL, nn_callback);

	printf("Press any key to quit\n");
#if defined(_WIN32) && !defined(UNDER_CE)
	while (!_kbhit())
#else
	while (1)
#endif
	{
		DoGameStuff();
		//check for / process incoming queries
		qr2_think(NULL);

	}
	//let gamemaster know we are shutting down
	qr2_shutdown(NULL);

	return 0;

}

