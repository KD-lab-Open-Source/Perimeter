// GameSpy Chat SDK C Test App
// Dan "Mr. Pants" Schoenblum
// dan@gamespy.com

/*************
** INCLUDES **
*************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if defined(applec) || defined(THINK_C) || defined(__MWERKS__) && !defined(__mips64) && !defined(_WIN32)
	#include ":::nonport.h"
	#include "::chat.h"
#else
	#include "../chat.h"
	#include "../../nonport.h"
#endif

#ifdef UNDER_CE
void RetailOutputA(CHAR *tszErr, ...);
#define printf RetailOutputA
#endif

/************
** GLOBALS **
************/
CHAT chat;
char serverAddress[128];
int port;
char chatNick[128];
char chatUser[128];
char chatName[128];
char chatChannel[128];
char gamename[128];
char secretKey[128];
CHATBool quit;

#ifdef __MWERKS__	// CodeWarrior will warn if functions not prototyped
/***************
** PROTOTYPES **
***************/
int test_main(int argc, char **argv);
#endif

/**************
** FUNCTIONS **
**************/
static void Raw(CHAT chat, const char * raw, void * param)
{
	printf("RAW: %s\n", raw);
	
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void Disconnected(CHAT chat, const char * reason, void * param)
{
	printf("Disconnected: %s\n", reason);
	
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void ChangedNickCallback(CHAT chat, CHATBool success, const char * oldNick, const char * newNick, void * param)
{
	if(success)
	{
		printf("Successfully changed");
		strcpy(chatNick, newNick);
	}
	else
		printf("Failed to change");
	printf(" nick from %s to %s\n", oldNick, newNick);
	
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void PrivateMessage(CHAT chat, const char * user, const char * message, int type, void * param)
{
	printf("Private message from %s: %s\n", user, message);

	// Nick change?
	///////////////
	if(strncasecmp("nick", message, 4) == 0)
	{
		chatChangeNick(chat, &message[5], ChangedNickCallback, NULL, CHATFalse);
	}
	
	GSI_UNUSED(type);
	GSI_UNUSED(param);
}

static void Invited(CHAT chat, const char * channel, const char * user, void * param)
{
	printf("Invited by %s to %s\n", user, channel);
	
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void ChannelMessage(CHAT chat, const char * channel, const char * user, const char * message, int type, void * param)
{
	char buffer[1024];

	printf("%s, in %s, said \"%s\"\n", user, channel, message);

	// Is this from us?
	///////////////////
	if(strcasecmp(user, chatNick) == 0)
		return;

	// Is it a command?
	///////////////////
	if(message[0] == '!')
	{
		message++;

		if(!strcasecmp(message, "quit") || !strcasecmp(message, "exit"))
			quit = CHATTrue;

		return;
	}

	sprintf(buffer, "%s: I agree", user);
	chatSendChannelMessage(chat, channel, buffer, CHAT_MESSAGE);
	
	GSI_UNUSED(type);
	GSI_UNUSED(param);
}

static void Kicked(CHAT chat, const char * channel, const char * user, const char * reason, void * param)
{
	printf("Kicked from %s by %s: %s\n", channel, user, reason);

	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void UserJoined(CHAT chat, const char * channel, const char * user, int mode, void * param)
{
	printf("%s joined %s", user, channel);

	GSI_UNUSED(chat);
	GSI_UNUSED(param);
	GSI_UNUSED(mode);
}

static void UserParted(CHAT chat, const char * channel, const char * user, int why, const char * reason, const char * kicker, void * param)
{
	if(why == CHAT_LEFT)
		printf("%s left %s\n", user, channel);
	else if(why == CHAT_QUIT)
		printf("%s quit: %s\n", user, reason);
	else if(why == CHAT_KICKED)
		printf("%s was kicked from %s by %s: %s", user, channel, kicker, reason);
	else if(why == CHAT_KILLED)
		printf("%s was killed: %s\n", user, reason);
	else
		printf("UserParted() called with unknown part-type\n");
	
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void UserChangedNick(CHAT chat, const char * channel, const char * oldNick, const char * newNick, void * param)
{
	printf("%s changed nicks to %s\n", oldNick, newNick);
	
	GSI_UNUSED(chat);
	GSI_UNUSED(channel);
	GSI_UNUSED(param);
}

static void UserModeChanged(CHAT chat, const char * channel, const char * user, int mode, void * param)
{
	printf("%s's new mode in %s is ", user, channel);;
	if(mode == CHAT_VOICE)
		printf("voice\n");
	else if(mode == CHAT_OP)
		printf("ops\n");
	else if(mode == (CHAT_VOICE | CHAT_OP))
		printf("voice+ops\n");
	
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void TopicChanged(CHAT chat, const char * channel, const char * topic, void * param)
{
	printf("The topic in %s changed to %s\n", channel, topic);
	
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void ChannelModeChanged(CHAT chat, const char * channel, CHATChannelMode * mode, void * param)
{
	printf("The mode in %s has changed:\n", channel);
	printf("   InviteOnly: %d\n", mode->InviteOnly);
	printf("   Private: %d\n", mode->Private);
	printf("   Secret: %d\n", mode->Secret);
	printf("   Moderated: %d\n", mode->Moderated);
	printf("   NoExternalMessages: %d\n", mode->NoExternalMessages);
	printf("   OnlyOpsChangeTopic: %d\n", mode->OnlyOpsChangeTopic);
	printf("   Limit: ");
	if(mode->Limit == 0)
		printf("N/A\n");
	else
		printf("%d\n", mode->Limit);
		
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void UserListUpdated(CHAT chat, const char * channel, void * param)
{
	printf("User list updated\n");
	
	GSI_UNUSED(chat);
	GSI_UNUSED(channel);
	GSI_UNUSED(param);
}

static void ConnectCallback(CHAT chat, CHATBool success, void * param)
{
	GSI_UNUSED(chat);
	GSI_UNUSED(success);
	GSI_UNUSED(param);
}


static void FillInUserCallback(CHAT chat, unsigned int IP, char user[128], void * param)
{
	strcpy(user, chatUser);
	
	GSI_UNUSED(chat);
	GSI_UNUSED(IP);
	GSI_UNUSED(param);
}

static void NickErrorCallback(CHAT chat, int type, const char * nick, void * param)
{
	if(type == CHAT_IN_USE)
	{
		printf("The nick %s is already being used.\n", nick);
		sprintf(chatNick,"ChatC%lu",current_time());
		chatRetryWithNick(chat, chatNick);
	}
	else if(type == CHAT_INVALID)
	{
		printf("The nick %s is invalid!\n", nick);
		chatDisconnect(chat);
	}
	
	GSI_UNUSED(param);
}

CHATBool enterChannelSuccess;
static void EnterChannelCallback(CHAT chat, CHATBool success, CHATEnterResult result, const char * channel, void * param)
{
	enterChannelSuccess = success;
	
	GSI_UNUSED(chat);
	GSI_UNUSED(result);
	GSI_UNUSED(channel);
	GSI_UNUSED(param);
}

static void GetUserInfoCallback(CHAT chat, CHATBool success, const char * nick, const char * user, const char * name, const char * address, int numChannels, const char ** channels, void * param)
{
	int i;

	if(!success)
	{
		printf("GetUserInfo failed\n");
		return;
	}

	printf("%s's Info:\n", nick);
	printf("   User: %s\n", user);
	printf("   Name: %s\n", name);
	printf("   Address: %s\n", address);
	printf("   Channels (%d):\n", numChannels);
	for(i = 0 ; i < numChannels ; i++)
		printf("      %s\n", channels[i]);
	
	GSI_UNUSED(chat);
	GSI_UNUSED(param);
}

static void EnumUsersCallback(CHAT chat, CHATBool success, const char * channel, int numUsers, const char ** users, int * modes, void * param)
{
	int i;

	if(!success)
	{
		printf("EnumUsers failed\n");
		return;
	}

	for(i = 0 ; i < numUsers ; i++)
		chatGetUserInfo(chat, users[i], GetUserInfoCallback, NULL, CHATFalse);
		
	GSI_UNUSED(channel);
	GSI_UNUSED(modes);
	GSI_UNUSED(param);
}

#if defined(_PS2)
int test_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	int i;
	chatGlobalCallbacks globalCallbacks;
	chatChannelCallbacks channelCallbacks;
	unsigned long stopTime;

	// Set default options.
	///////////////////////
	strcpy(serverAddress, "peerchat.gamespy.com");
	port = 6667;
	sprintf(chatNick,"ChatC%lu",current_time() % 1000);
	strcpy(chatUser, "ChatCUser");
	strcpy(chatName, "ChatCName");
	strcpy(chatChannel, "#GSP!gmtest");
	strcpy(gamename, "gmtest");
	secretKey[0] = 'H';
	secretKey[1] = 'A';
	secretKey[2] = '6';
	secretKey[3] = 'z';
	secretKey[4] = 'k';
	secretKey[5] = 'S';
	secretKey[6] = '\0';

	// Go through command-line options.
	///////////////////////////////////
	for(i = 1 ; i < argc ; i++)
	{
		if((argv[i][0] == '-') && ((i + 1) < argc))
		{
			switch(argv[i][1])
			{
			case 's':
				strcpy(serverAddress, argv[++i]);
				break;
			case 'p':
				port = atoi(argv[++i]);
				break;
			case 'n':
				strcpy(chatNick, argv[++i]);
				break;
			case 'u':
				strcpy(chatUser, argv[++i]);
				break;
			case 'c':
				strcpy(chatChannel, argv[++i]);
				break;
			default:
				printf("Error parsing command-line: %s\n", argv[i]);
				return 1;
			}
		}
		else
		{
			printf("Error parsing command-line: %s\n", argv[i]);
			return 1;
		}
	}

	// Set global callbacks.
	////////////////////////
	memset(&globalCallbacks, 0, sizeof(chatGlobalCallbacks));
	globalCallbacks.raw = Raw;
	globalCallbacks.disconnected = Disconnected;
	globalCallbacks.privateMessage = PrivateMessage;
	globalCallbacks.invited = Invited;
	globalCallbacks.param = NULL;

	// Connect.
	///////////
	chat = chatConnectSecure(serverAddress, port, chatNick, chatName, gamename, secretKey, &globalCallbacks, NickErrorCallback, FillInUserCallback, ConnectCallback, NULL, CHATTrue);
	if(!chat)
	{
		printf("Connect failed\n");
		return 1;
	}

	// Set channel callbacks.
	/////////////////////////
	memset(&channelCallbacks, 0, sizeof(chatChannelCallbacks));
	channelCallbacks.channelMessage = ChannelMessage;
	channelCallbacks.channelModeChanged = ChannelModeChanged;
	channelCallbacks.kicked = Kicked;
	channelCallbacks.topicChanged = TopicChanged;
	channelCallbacks.userParted = UserParted;
	channelCallbacks.userJoined = UserJoined;
	channelCallbacks.userListUpdated = UserListUpdated;
	channelCallbacks.userModeChanged = UserModeChanged;
	channelCallbacks.userChangedNick = UserChangedNick;
	channelCallbacks.param = NULL;

	// Join.
	////////
	chatEnterChannel(chat, chatChannel, NULL, &channelCallbacks, EnterChannelCallback, NULL, CHATTrue);
	if(!enterChannelSuccess)
	{
		printf("Enter Channel failed\n");
		return 1;
	}

	// Say hi.
	//////////
	chatSendChannelMessage(chat, chatChannel, "Hi", CHAT_MESSAGE);

	// Enum through the players.
	////////////////////////////
	chatEnumUsers(chat, chatChannel, EnumUsersCallback, NULL, CHATFalse);

	// Stay for a while.
	////////////////////
	stopTime = (current_time() + 60000);
	do
	{
		chatThink(chat);
		msleep(50);
	}
	while(!quit && (current_time() < stopTime));

	// Say bye.
	///////////
	chatSendChannelMessage(chat, chatChannel, "Bye", CHAT_MESSAGE);

	// Leave.
	/////////
	chatLeaveChannel(chat, chatChannel, NULL);

	// Disconnect.
	//////////////
	chatDisconnect(chat);
	printf("All Done!\n");
	return 0;
}
