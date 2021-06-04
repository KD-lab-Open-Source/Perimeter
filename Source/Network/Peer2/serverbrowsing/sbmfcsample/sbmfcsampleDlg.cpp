// sbmfcsampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sbmfcsample.h"
#include "sbmfcsampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//SB - timer ID and frequency
#define TIMER_ID         100
#define TIMER_FREQUENCY  10

//SB - server list columns
#define COL_SERVERNAME   0
#define COL_PING         1
#define COL_PLAYERS      2
#define COL_MAPNAME      3
#define COL_GAMETYPE     4

//SB - player list columns
#define COL_PNAME        0
#define COL_PPING        1
#define COL_PSCORE       2

//SB - starting and ending port for LAN game searches
#define START_PORT       7778
#define END_PORT         (START_PORT + 100)

//SB - maximum number of concurrent updates
#define MAX_UPDATES      30

/////////////////////////////////////////////////////////////////////////////
// CSbmfcsampleDlg dialog

CSbmfcsampleDlg::CSbmfcsampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSbmfcsampleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSbmfcsampleDlg)
	m_filter = _T("");
	m_gamename = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSbmfcsampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSbmfcsampleDlg)
	DDX_Control(pDX, IDC_SERVERS, m_servers);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_SERVERLIST, m_serverList);
	DDX_Control(pDX, IDC_PLAYERLIST, m_playerList);
	DDX_Text(pDX, IDC_FILTER, m_filter);
	DDX_Text(pDX, IDC_GAMENAME, m_gamename);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSbmfcsampleDlg, CDialog)
	//{{AFX_MSG_MAP(CSbmfcsampleDlg)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_NOTIFY(NM_CLICK, IDC_SERVERLIST, OnClickServerlist)
	ON_NOTIFY(NM_DBLCLK, IDC_SERVERLIST, OnDblclkServerlist)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_SERVERLIST, OnColumnclickServerlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSbmfcsampleDlg message handlers

BOOL CSbmfcsampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	//SB - setup list columns
	m_serverList.InsertColumn(COL_SERVERNAME, "Server Name", LVCFMT_LEFT, 150, -1);
	m_serverList.InsertColumn(COL_PING, "Ping", LVCFMT_LEFT, 50, 0);
	m_serverList.InsertColumn(COL_PLAYERS, "Players", LVCFMT_LEFT, 75, 1);
	m_serverList.InsertColumn(COL_MAPNAME, "Map", LVCFMT_LEFT, 75, 2);
	m_serverList.InsertColumn(COL_GAMETYPE, "GameType", LVCFMT_LEFT, 100, 3);
	m_playerList.InsertColumn(COL_PNAME, "Player Name", LVCFMT_LEFT, 150, -1);
	m_playerList.InsertColumn(COL_PPING, "Ping", LVCFMT_LEFT, 50, 0);
	m_playerList.InsertColumn(COL_PSCORE, "Score", LVCFMT_LEFT, 50, 1);
	ListView_SetExtendedListViewStyle(m_serverList.m_hWnd, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyle(m_playerList.m_hWnd, LVS_EX_FULLROWSELECT);

	//SB - default to Internet
	CheckRadioButton(IDC_INTERNET, IDC_LAN, IDC_INTERNET);

	//SB - default to QR2
	CheckRadioButton(IDC_GOA, IDC_QR2, IDC_QR2);

	//SB - no server browser yet
	m_serverBrowser = NULL;

	//SB - no timer yet
	m_timerID = 0;

	//SB - no servers yet
	m_servers.SetWindowText("");

	return TRUE;
}

BOOL CSbmfcsampleDlg::DestroyWindow() 
{
	// free the browser
	if(m_serverBrowser)
	{
		ServerBrowserFree(m_serverBrowser);
		m_serverBrowser = NULL;
	}
	
	return CDialog::DestroyWindow();
}

void CSbmfcsampleDlg::OnTimer(UINT nIDEvent) 
{
	// think if our timer was called
	if(nIDEvent == m_timerID)
		ServerBrowserThink(m_serverBrowser);
	
	CDialog::OnTimer(nIDEvent);
}

void CSbmfcsampleDlg::OnRefresh() 
{
	UpdateData();

	// create the server list if we need to
	if(!CreateServerList())
		return;

	// if we're doing an update, cancel it
	SBState state = ServerBrowserState(m_serverBrowser);
	if((state != sb_connected) && (state != sb_disconnected))
	{
		ServerBrowserHalt(m_serverBrowser);
		return;
	}

	// clear the server browser and list
	ServerBrowserClear(m_serverBrowser);
	m_serverList.DeleteAllItems();

	// clear the progress bar
	m_progress.SetPos(0);

	// clear the server count
	m_serverCount = 0;
	m_servers.SetWindowText("0");

	// set a timer
	if(!m_timerID)
		m_timerID = SetTimer(TIMER_ID, TIMER_FREQUENCY, NULL);

	// fields we're interested in
	unsigned char fields[] = { HOSTNAME_KEY, NUMPLAYERS_KEY, MAXPLAYERS_KEY, MAPNAME_KEY, GAMETYPE_KEY };
	int numFields = sizeof(fields) / sizeof(fields[0]);

	// check for internet/lan
	bool internet = (IsDlgButtonChecked(IDC_INTERNET) == BST_CHECKED);

	// do an update
	SBError error;
	if(internet)
		error = ServerBrowserUpdate(m_serverBrowser, SBTrue, SBFalse, fields, numFields, (char *)(const char *)m_filter);
	else
		error = ServerBrowserLANUpdate(m_serverBrowser, SBTrue, START_PORT, END_PORT);
}

void CSbmfcsampleDlg::OnClickServerlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// clear the player box
	m_playerList.DeleteAllItems();

	// find the selected server
	POSITION pos = m_serverList.GetFirstSelectedItemPosition();
	if(pos == NULL)
		return;
	int index = m_serverList.GetNextSelectedItem(pos);

	// get the server
	LVITEM item;
	item.mask = LVIF_PARAM;
	item.iItem = index;
	item.iSubItem = 0;
	m_serverList.GetItem(&item);
	SBServer server = (SBServer)item.lParam;

	if (!SBServerHasFullKeys(server)) //we need to query for the full rules
	{
		// turn on the hour glass
		HCURSOR cursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

		// do a server update (this is blocking!)
		ServerBrowserAuxUpdateServer(m_serverBrowser, server, SBFalse, SBTrue);

		// turn off the hour glass
		SetCursor(cursor);
	}

	// get the player count
	int count = SBServerGetIntValue(server, "numplayers", 0);

	// add the players to the list
	for(int i = 0 ; i < count ; i++)
	{
		m_playerList.InsertItem(0, SBServerGetPlayerStringValue(server, i, "player", "(NO NAME)"));
		m_playerList.SetItem(0, COL_PPING, LVIF_TEXT, SBServerGetPlayerStringValue(server, i, "ping", "0"), -1, 0, 0, 0);
		m_playerList.SetItem(0, COL_PSCORE, LVIF_TEXT, SBServerGetPlayerStringValue(server, i, "score", "0"), -1, 0, 0, 0);
	}

	// MFC
	*pResult = 0;
}

void CSbmfcsampleDlg::OnDblclkServerlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// launch the game here
	MessageBox("If this were a real server browser, you would be launched now!","Go!");

	// MFC
	*pResult = 0;
}

void CSbmfcsampleDlg::OnColumnclickServerlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// track ascending/descending
	static bool ascending = false;
	ascending = !ascending;

	//figure out which column they clicked
	switch(pNMListView->iSubItem)
	{
	case COL_SERVERNAME:
		ServerBrowserSort(m_serverBrowser, (SBBool)ascending, "hostname", sbcm_stricase);
		break;
	case COL_PING:
		ServerBrowserSort(m_serverBrowser, (SBBool)ascending, "ping", sbcm_int);
		break;
	case COL_PLAYERS:
		ServerBrowserSort(m_serverBrowser, (SBBool)ascending, "numplayers", sbcm_int);
		break;
	case COL_MAPNAME:
		ServerBrowserSort(m_serverBrowser, (SBBool)ascending, "mapname", sbcm_stricase);
		break;
	case COL_GAMETYPE:
		ServerBrowserSort(m_serverBrowser, (SBBool)ascending, "gametype", sbcm_stricase);
		break;
	}

	// we don't want the server list to redraw every time we insert a server!
	m_serverList.SetRedraw(false);

	// clear the server list
	m_serverList.DeleteAllItems();

	// clear the server count
	m_serverCount = 0;
	m_servers.SetWindowText("0");

	// go through the list of servers
	for(int i = 0; i < ServerBrowserCount(m_serverBrowser) ; i++)
	{
		// if we got basic info for it, put it back in the list
		SBServer server = ServerBrowserGetServer(m_serverBrowser, i);
		if(SBServerHasBasicKeys(server))
			AddServer(server, FALSE);
	}

	// let the server list redraw itself now that we're done updating
	m_serverList.SetRedraw(true);
	
	*pResult = 0;
}

void CSbmfcsampleDlg::SBCallback(ServerBrowser serverBrowser, SBCallbackReason reason, SBServer server, void *instance)
{
	CString str;
	CSbmfcsampleDlg * dlg = (CSbmfcsampleDlg *)instance;

	CString address;
	if(server)
		address.Format("%s:%d", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));

	switch(reason)
	{
	case sbc_serveradded:
		dlg->AddServer(server, TRUE);
		break;
	case sbc_serverupdated:
		dlg->AddServer(server, TRUE);
		break;
	case sbc_serverupdatefailed:
		break;
	case sbc_serverdeleted:
		dlg->RemoveServer(server);
		break;
	case sbc_updatecomplete:
		dlg->m_progress.SetPos(100);
		break;
	case sbc_queryerror:
		str.Format("Query Error: %s\n", ServerBrowserListQueryError(dlg->m_serverBrowser));
		dlg->MessageBox(str);
		break;
	}
}

BOOL CSbmfcsampleDlg::CreateServerList()
{
	// only create the object once
	if(!m_serverBrowser)
	{
		// check for an empty gamename
		if(m_gamename.IsEmpty())
		{
			MessageBox("No game specified");
			GetDlgItem(IDC_GAMENAME)->SetFocus();
			return FALSE;
		}

		// create it
		m_serverBrowser = ServerBrowserNew(m_gamename, "gmtest", "HA6zkS", 0, MAX_UPDATES, (IsDlgButtonChecked(IDC_GOA) == BST_CHECKED)?QVERSION_GOA:QVERSION_QR2, SBCallback, this);
		if(!m_serverBrowser)
		{
			MessageBox("Unable to create the server browser object");
			return FALSE;
		}

		// don't let them change the gamename
		GetDlgItem(IDC_GAMENAME)->EnableWindow(FALSE);
	}

	return TRUE;
}

void CSbmfcsampleDlg::AddServer(SBServer server, BOOL checkForReplace)
{
	// set the progress
	if(ServerBrowserCount(m_serverBrowser) > 0)
		m_progress.SetPos((ServerBrowserCount(m_serverBrowser) - ServerBrowserPendingQueryCount(m_serverBrowser)) * 100 / ServerBrowserCount(m_serverBrowser));

	// check for the server in the list
	int index = FindServer(server);
	bool replace = (index != -1);

	// if we didn't find a server to replace, append
	if(!replace)
		index = m_serverList.GetItemCount();

	// set or insert the hostname
	const char * hostname = SBServerGetStringValue(server, "hostname","(NO NAME)");
	if(replace)
	{
		m_serverList.SetItem(index, COL_SERVERNAME, LVIF_TEXT, hostname, -1, 0, 0, NULL);
	}
	else
	{
		m_serverList.InsertItem(index, hostname);
		m_serverList.SetItem(index, COL_SERVERNAME, LVIF_PARAM, NULL, -1, 0, 0, (LPARAM)server);
	}

	// set the rest of the columns
	int numplayers = SBServerGetIntValue(server, "numplayers", 0);
	CString ping, players;
	ping.Format("%d", SBServerGetPing(server));
	players.Format("%d/%d", numplayers,  SBServerGetIntValue(server, "maxplayers", 0));
	m_serverList.SetItem(index, COL_PING, LVIF_TEXT, ping, -1, 0, 0, 0);
	m_serverList.SetItem(index, COL_PLAYERS, LVIF_TEXT, players, -1, 0, 0, 0);
	m_serverList.SetItem(index, COL_MAPNAME, LVIF_TEXT, SBServerGetStringValue(server, "mapname", "(NO MAP)"), -1, 0, 0, 0);
	m_serverList.SetItem(index, COL_GAMETYPE, LVIF_TEXT, SBServerGetStringValue(server, "gametype", ""), -1, 0, 0, 0);

	// update server count
	if(!replace)
	{
		CString str;
		str.Format("%d", ++m_serverCount);
		m_servers.SetWindowText(str);
	}
}

void CSbmfcsampleDlg::RemoveServer(SBServer server)
{
	// find the server
	int index = FindServer(server);
	ASSERT(index != -1);
	if(index == -1)
		return;

	// remove it
	m_serverList.DeleteItem(index);

	// update server count
	CString str;
	str.Format("%d", --m_serverCount);
	m_servers.SetWindowText(str);
}

int CSbmfcsampleDlg::FindServer(SBServer server)
{
	LVFINDINFO info;
	info.flags = LVFI_PARAM;
	info.lParam = (LPARAM)server;
	return m_serverList.FindItem(&info);
}
