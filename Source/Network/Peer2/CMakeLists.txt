INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Peer_SRCS
        peer/peerAutoMatch.c
        peer/peerCallbacks.c
        peer/peerGlobalCallbacks.c
        peer/peerHost.c
        peer/peerKeys.c
        peer/peerMain.c
        peer/peerMangle.c
        peer/peerOperations.c
        peer/peerPing.c
        peer/peerPlayers.c
        peer/peerQR.c
        peer/peerRooms.c
        peer/peerSB.c
        darray.c
        nonport.c
        chat/chatCallbacks.c
        chat/chatChannel.c
        chat/chatCrypt.c
        chat/chatHandlers.c
        chat/chatMain.c
        chat/chatSocket.c
        hashtable.c
        pinger/pingerMain.c
        md5c.c
        qr2/qr2.c
        qr2/qr2regkeys.c
        serverbrowsing/sb_crypt.c
        serverbrowsing/sb_queryengine.c
        serverbrowsing/sb_server.c
        serverbrowsing/sb_serverbrowsing.c
        serverbrowsing/sb_serverlist.c
)

ADD_LIBRARY(Peer STATIC ${Peer_SRCS})
