
struct UnitInterfaceActions
{
	int op_move = 0;
	int op_stop = 0;
	int op_stop2 = 0;
	int op_attack = 0;
	int op_back = 0;
	int op_offdeff = 0;
	int op_patrol = 0;
	int op_power_onoff = 0;
	int op_sell = 0;
	int op_upgrade = 0;
	int op_field = 0;
	int op_install = 0;
	int op_teleportate = 0;
	int op_alarm = 0;
	int op_brig_back = 0;
	int op_brig_change = 0;
	int op_brig_build = 0;
	int op_charging = 0;
	int op_charge_control = 0;

delegate:

	void set(){
		op_move = op_stop = op_stop2 = op_attack = op_back = op_patrol = op_offdeff = 0;
		op_power_onoff = op_sell = op_upgrade = 
		op_field = op_install = op_teleportate = op_alarm = op_brig_back = op_brig_change = op_brig_build = op_charging = op_charge_control = 1;
	}

	void clear(){
		op_move = op_stop = op_stop2 = op_attack = op_back = op_offdeff = 
		op_patrol = op_power_onoff = op_sell = op_upgrade = 
		op_field = op_install = op_teleportate = op_alarm = op_brig_back = op_brig_change = op_brig_build = op_charging = op_charge_control = 0;
	}

	UnitInterfaceActions& operator&=(const UnitInterfaceActions& act){
		op_move |= act.op_move;
		op_stop |= act.op_stop;
		op_stop2 |= act.op_stop2;
		op_attack |= act.op_attack;
		op_back |= act.op_back;
		op_offdeff |= act.op_offdeff;
		op_patrol |= act.op_patrol;
		op_power_onoff &= act.op_power_onoff;
		op_sell &= act.op_sell;
		op_upgrade &= act.op_upgrade;
		op_field &= act.op_field;
		op_install &= act.op_install;
		op_teleportate &= act.op_teleportate;
		op_alarm &= act.op_alarm;
		op_brig_back &= act.op_brig_back;
		op_brig_change &= act.op_brig_change;
		op_brig_build &= act.op_brig_build;
		op_charging &= act.op_charging;
		op_charge_control &= act.op_charge_control;

		return *this;
	}

};

struct UnitInterfacePrm
{
	UnitInterfaceActions actions = {};

	ShellControlID upgrade_button_id = SQSH_SELPANEL_UPGRADE_ID;
	ShellControlID upgrading_button_id = SQSH_SELPANEL_UPGRADE_ID;

	PopupFormatGroup format_group = POPUP_FORMAT_NONE;
	string popup = "";
};

static struct UnitInterfaceData
{
	struct Base : UnitInterfaceActions
	{
	};

	UnitInterfaceActions DefaultAction = {};

	Base SquadBase = 
	{
		op_move = 1; op_stop = 1;
		op_back = 1; op_patrol = 1;
		op_charging = 1;
	};

	Base SquadEmpty = 
	{
		op_move = 1;
		op_charging = 1;
	};

	Base SquadAttack = SquadBase{ op_attack = 1;};
	Base SquadFull   = SquadAttack{ op_offdeff = 1;};

	Base Sellable = DefaultAction{ op_sell = 1;};
	Base Pluggable = Sellable{ op_power_onoff = 1;};

	Base Core = Sellable{ };
//	Base Core = Sellable{ op_upgrade = 1; op_charge_control = 1;};

	Base Amplif_Relay = Sellable {op_charging = 1;} ;

//	Base Protector = Sellable{ op_field = 1;};
	Base Protector = Sellable{ op_field = 1; op_charge_control = 1; op_charging = 1;};

	Base Alpha = DefaultAction {op_power_onoff = 0;};
	Base Omega = Alpha { op_upgrade = 1;};

	Base Station1 = Sellable{ op_upgrade = 1;};

	Base Station2 = Sellable;
	Base Bomb = Station2{op_sell = 0;};

	Base FrameInstalled = DefaultAction{ op_install = 1; op_teleportate = 1; op_alarm = 1;op_charging = 1;};

	Base FrameUninstalled = DefaultAction{ op_install = 1; op_teleportate = 1; op_alarm = 1; op_move = 1; op_stop = 1;op_charging = 1;};

	Base Brigadier = DefaultAction{op_move = 1; op_stop = 1; op_brig_back = 1; op_brig_change = 1;  op_charging = 1; };
	Base Prorab    = Brigadier{ op_brig_build = 1;};

	Base Gun1 = Pluggable{op_attack = 1; op_charging = 1;  op_stop2 = 1; };

	Base Gun2 = Gun1{op_stop2 = 1; op_charging = 1; };

	Base GunWithCharge = Gun2{op_charging = 1; };

	////////
	UnitInterfacePrm  Default = 
	{
		actions = DefaultAction;
	};

	UnitInterfacePrm  squad = { actions =  SquadFull; popup = "<Squad>"; format_group = POPUP_FORMAT_SQUAD;};
	UnitInterfacePrm  squad1 = { actions =  SquadBase;};
	UnitInterfacePrm  squad2 = { actions =  SquadAttack;};
	UnitInterfacePrm  squad3 = { actions =  SquadEmpty;};

	UnitInterfacePrm  amplifier = { actions =  Amplif_Relay;  popup = "<Yadro_ex>"; format_group = POPUP_FORMAT_AMPLIFIER;};
	UnitInterfacePrm  relay = { actions =  Amplif_Relay;      popup = "<Transmitter>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  protector = { actions =  Protector; popup = "<generator>"; format_group = POPUP_FORMAT_CORE;};

	UnitInterfacePrm  station_rocket1  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_ROCKET1_ID; popup = "<station_rocket>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_laser1  = { actions =  Station1;  upgrade_button_id = SQSH_SELPANEL_UPGRADE_LASER1_ID;  popup = "<station_laser>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_electro1  = { actions =  Station1;  upgrade_button_id = SQSH_SELPANEL_UPGRADE_ELECTRO1_ID;  popup = "<station_electro>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_bomb1  = { actions =  Station1;   upgrade_button_id = SQSH_SELPANEL_UPGRADE_BOMB1_ID;   popup = "<station_bomb>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_fly1  = { actions =  Station1;    upgrade_button_id = SQSH_SELPANEL_UPGRADE_FLY_ID;     popup = "<station_fly>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_exodus1  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_EXODUS1_ID; popup = "<station_exodus>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_empire1  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_EMPIRE1_ID; popup = "<station_empire>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_harkback1  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_HARKBACK1_ID; popup = "<station_harkback>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_subterra1  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_SUBTERRA_ID;	  popup = "<station_subterra>"; format_group = POPUP_FORMAT_BUILDING;};

	UnitInterfacePrm  station_rocket2  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_ROCKET2_ID; upgrading_button_id = SQSH_SELPANEL_UPGRADE_ROCKET1_ID; popup = "<upgrade_rocket1_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_laser2  = { actions =  Station1;  upgrade_button_id = SQSH_SELPANEL_UPGRADE_LASER2_ID; upgrading_button_id = SQSH_SELPANEL_UPGRADE_LASER1_ID;   popup = "<upgrade_laser1_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_electro2  = { actions =  Station1;  upgrade_button_id = SQSH_SELPANEL_UPGRADE_ELECTRO2_ID; upgrading_button_id = SQSH_SELPANEL_UPGRADE_ELECTRO1_ID;   popup = "<upgrade_electro1_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_bomb2  = { actions =  Station1;   upgrade_button_id = SQSH_SELPANEL_UPGRADE_BOMB2_ID; upgrading_button_id = SQSH_SELPANEL_UPGRADE_BOMB1_ID;     popup = "<upgrade_bomb1_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_exodus2  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_EXODUS2_ID; upgrading_button_id = SQSH_SELPANEL_UPGRADE_EXODUS1_ID; popup = "<upgrade_exodus1_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_empire2  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_EMPIRE2_ID; upgrading_button_id = SQSH_SELPANEL_UPGRADE_EMPIRE1_ID; popup = "<upgrade_empire1_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_harkback2  = { actions =  Station1; upgrade_button_id = SQSH_SELPANEL_UPGRADE_HARKBACK2_ID; upgrading_button_id = SQSH_SELPANEL_UPGRADE_HARKBACK1_ID; popup = "<upgrade_harkback1_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_fly2  = { actions =  Station2;    upgrading_button_id = SQSH_SELPANEL_UPGRADE_FLY_ID; popup = "<upgrade_fly_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_subterra2  = { actions =  Station2; upgrading_button_id = SQSH_SELPANEL_UPGRADE_SUBTERRA_ID; popup = "<upgrade_subterra_unit>"; format_group = POPUP_FORMAT_BUILDING;};

	UnitInterfacePrm  station_rocket3  = { actions =  Station2; upgrading_button_id = SQSH_SELPANEL_UPGRADE_ROCKET2_ID; popup = "<upgrade_rocket2_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_laser3  = { actions =  Station2;  upgrading_button_id = SQSH_SELPANEL_UPGRADE_LASER2_ID;  popup = "<upgrade_laser2_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_electro3  = { actions =  Station2;  upgrading_button_id = SQSH_SELPANEL_UPGRADE_ELECTRO2_ID;  popup = "<upgrade_electro2_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_bomb3  = { actions =  Station2;   upgrading_button_id = SQSH_SELPANEL_UPGRADE_BOMB2_ID;   popup = "<upgrade_bomb2_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_exodus3  = { actions =  Station2; upgrading_button_id = SQSH_SELPANEL_UPGRADE_EXODUS2_ID; popup = "<upgrade_exodus2_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_empire3  = { actions =  Station2; upgrading_button_id = SQSH_SELPANEL_UPGRADE_EMPIRE2_ID; popup = "<upgrade_empire2_unit>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  station_harkback3  = { actions =  Station2; upgrading_button_id = SQSH_SELPANEL_UPGRADE_HARKBACK2_ID; popup = "<upgrade_harkback2_unit>"; format_group = POPUP_FORMAT_BUILDING;};

	UnitInterfacePrm  commander    = { actions =  Station2; popup = "<CommandCenter>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  static_bomb    = { actions =  Bomb; popup = "<StaticBomb>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  plant_soldier  = { actions =  Station2; popup = "<Plant_soldier>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  plant_officer  = { actions =  Station2; popup = "<Plant_officer>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  plant_technic  = { actions =  Station2; popup = "<Plant_technic>"; format_group = POPUP_FORMAT_BUILDING;};

	UnitInterfacePrm  frame_installed  = { actions =  FrameInstalled; popup = "<frame>"; format_group = POPUP_FORMAT_FRAME;};
	UnitInterfacePrm  frame_uninstalled  = { actions =  FrameUninstalled; popup = "<frame>"; format_group = POPUP_FORMAT_FRAME;};
	UnitInterfacePrm  brigadier  = { actions =  Brigadier; popup = "<brigadier_unit>"; format_group = POPUP_FORMAT_MMP;};
	UnitInterfacePrm  prorab  = { actions =  Prorab; popup = "<prorab_unit>"; format_group = POPUP_FORMAT_MMP;};

	UnitInterfacePrm  gun_laser = { actions =  Gun1;  popup = "<gun_laser>"; format_group = POPUP_FORMAT_GUN;};
	UnitInterfacePrm  gun_electro = { actions =  Gun1;  popup = "<gun_electro>"; format_group = POPUP_FORMAT_GUN;};
	UnitInterfacePrm  gun_rocket = { actions =  Gun1; popup = "<gun_rocket>"; format_group = POPUP_FORMAT_GUN;};
	UnitInterfacePrm  gun_howitzer = { actions =  Gun2; popup = "<gun_howitzer>"; format_group = POPUP_FORMAT_GUN;};
	UnitInterfacePrm  gun_subchaser = { actions =  Gun2; popup = "<gun_subchaser>"; format_group = POPUP_FORMAT_GUN;};
	UnitInterfacePrm  gun_ballistic = { actions =  GunWithCharge; popup = "<gun_ballistic>"; format_group = POPUP_FORMAT_GUN;};
	UnitInterfacePrm  gun_filth_navigator = { actions =  GunWithCharge; popup = "<gun_filth_navigator>"; format_group = POPUP_FORMAT_GUN;};
	UnitInterfacePrm  gun_scum_disruptor = { actions =  GunWithCharge; popup = "<gun_scum_disruptor>"; format_group = POPUP_FORMAT_GUN;};

	UnitInterfacePrm  corridor_alpha  = { actions =  Alpha; popup = "<CorridorAlpha>"; format_group = POPUP_FORMAT_BUILDING;};
	UnitInterfacePrm  corridor_omega  = { actions =  Omega; upgrading_button_id = SQSH_SELPANEL_UPGRADE_OMEGA_ID; upgrade_button_id = SQSH_SELPANEL_UPGRADE_OMEGA_ID; popup = "<CorridorOmega>"; format_group = POPUP_FORMAT_BUILDING;};
};

static UnitInterfaceData unit_interface_data = {};

UnitInterfacePrm  interface_squad1_prm = unit_interface_data.squad1;
UnitInterfacePrm  interface_squad2_prm = unit_interface_data.squad2;
UnitInterfacePrm  interface_squad3_prm = unit_interface_data.squad3;
UnitInterfacePrm  interface_frame_uninstalled = unit_interface_data.frame_uninstalled;

