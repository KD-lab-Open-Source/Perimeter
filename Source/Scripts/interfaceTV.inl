///////////////////////////////////////////////////////////////////////////////////////////////////
//параметры камеры и моделей в окошке

//------------------------------------------------------
// параметры камеры и моделей в окошке - по умолчанию

struct InterfaceTV
{
	float radius = 80;
	float pos_dx = 0;
	float pos_dy = 0;
	float angle_x = 70;
	float angle_y = 0;
	float angle_z = 0;
	string model = "";
	string channel = "";
	string channel_power_on = "";
};



InterfaceTV interfaceTV_Frame = 
{
	//фрейм
	radius = 200;
	angle_x = 30;
	pos_dy = 70;
	channel = "fly";
};
InterfaceTV interfaceTV_Core = 
{
	//генератор
	radius = 150;
	angle_x =50;
	pos_dy = -12;
	channel = "off position";
	channel_power_on = "main";
};
InterfaceTV interfaceTV_Transmitter = 
{
	//ретранслятор
	radius = 200;
	angle_x =50;
	pos_dy = -5;
	channel = "off position";
};
InterfaceTV interfaceTV_CommandCenter = 
{
	//комм. центр
	angle_x =50;
	radius = 115;
	pos_dy = -7;
};
InterfaceTV interfaceTV_StaticBomb = 
{
	//строение-бомба
	angle_x =50;
	radius = 115;
	pos_dy = -7;
};
InterfaceTV interfaceTV_PlantSoldier = 
{
	//з.солдат
	angle_x =50;
	radius = 90;
	pos_dy = -10;
};
InterfaceTV interfaceTV_PlantOfficer = 
{
	//з.офф.
	angle_x =50;
	radius = 90;
	pos_dy = 10;
};
InterfaceTV interfaceTV_PlantTechnician = 
{
	//з.техн.
	angle_x =50;
	radius = 90;
	pos_dy = 8;
};
InterfaceTV interfaceTV_Amplifier = 
{
	//усилитель
	angle_x =50;
	radius = 200;
	pos_dy = 3;
};
InterfaceTV interfaceTV_GunLaser = 
{
	//laser gun
	angle_x =50;
	radius = 90;
	pos_dy = 15;
};
InterfaceTV interfaceTV_GunElectro = 
{
	//electro gun
	angle_x =50;
	radius = 90;
	pos_dy = 15;
};
InterfaceTV interfaceTV_GunRocket =  
{
	//rocket gun
	angle_x =50;
	radius = 95;
	pos_dy = 19;
	model = "RESOURCE\\Models\\Interface\\intrf_rocket_launcher.m3d";
};
InterfaceTV interfaceTV_GunHowitzer =  
{
	//howitzer gun
	angle_x =50;
	radius = 110;
	pos_dy = 22;
	model = "RESOURCE\\Models\\Interface\\intrf_haubiz.m3d";
};
InterfaceTV interfaceTV_GunSubchaser =  
{
	//subchaser gun
	angle_x =50;
	radius = 90;
	pos_dy = 5;
};
InterfaceTV interfaceTV_GunBallistic = 
{
	//ballistic
	angle_x =50;
	radius = 300;
	pos_dy = 0;
};
InterfaceTV interfaceTV_FilthNavigator = 
{
	//filth navigator
	angle_x =50;
	radius = 100;
	pos_dy = -15;
};
InterfaceTV interfaceTV_ScumDisruptor = 
{
	//scum disruptor
	angle_x =50;
	radius = 100;
	pos_dy = -15;
};
InterfaceTV interfaceTV_Sniper = 
{
	angle_x =50;
	radius = 75;
	pos_dy = 7;
};
InterfaceTV interfaceTV_Leamo = 
{
	angle_x =50;
	radius = 80;
	pos_dy = 7;
};
InterfaceTV interfaceTV_Impaler = 
{
	angle_x =50;
	radius = 80;
	pos_dy = 7;
};
InterfaceTV interfaceTV_Rocker = 
{
	angle_x =50;
	radius = 80;
	pos_dy = -2;
	model = "RESOURCE\\Models\\Interface\\intrf_rocker.m3d";
};
InterfaceTV interfaceTV_Mortar = 
{
	angle_x =50;
	radius = 120;
	pos_dy = 7;
	model = "RESOURCE\\Models\\Interface\\intrf_mortar.m3d";
};
InterfaceTV interfaceTV_Ceptor = 
{
	angle_x =50;
	radius = 85;
	pos_dy = 0;
	model = "RESOURCE\\Models\\Interface\\intrf_ceptor.m3d";
	channel = "main";
};
InterfaceTV interfaceTV_Gyroid = 
{
	angle_x =50;
	radius = 100;
	pos_dy = -9;
};
InterfaceTV interfaceTV_Eflair = 
{
	angle_x =50;
	radius = 100;
	pos_dy = -9;
};
InterfaceTV interfaceTV_Wargon = 
{
	angle_x =50;
	radius = 100;
	pos_dy = 13;
	model = "RESOURCE\\Models\\Interface\\intrf_wargon.m3d";
};
InterfaceTV interfaceTV_Bombie = 
{
	angle_x =50;
	radius = 90;
	pos_dy = -5;
};
InterfaceTV interfaceTV_RProjector = 
{
	angle_x =50;
	radius = 70;
	pos_dy = 12;
	model = "RESOURCE\\Models\\Interface\\intrf_r_progector.m3d";
};
InterfaceTV interfaceTV_Minotaur = 
{
	angle_x =50;
	radius = 70;
	pos_dy = -4;
};
InterfaceTV interfaceTV_Unseen = 
{
	angle_x =50;
	radius = 80;
	pos_dy = 0;
};
InterfaceTV interfaceTV_ScumSplitter = 
{
	angle_x =50;
	radius = 80;
	pos_dy = 0;
};
InterfaceTV interfaceTV_ScumTwister = 
{
	angle_x =50;
	radius = 60;
	pos_dy = 0;
};
InterfaceTV interfaceTV_ScumHeater = 
{
	angle_x =50;
	radius = 400;
	pos_dy = 0;
};
InterfaceTV interfaceTV_Conductor = 
{
	angle_x =50;
	radius = 400;
	pos_dy = 0;
};
InterfaceTV interfaceTV_ScumThrower = 
{
	angle_x =50;
	radius = 100;
	pos_dy = 0;
};
InterfaceTV interfaceTV_LabRocket1 = 
{
	angle_x =50;
	radius = 90;
	pos_dy = 10;
	channel = "main";
};
InterfaceTV interfaceTV_LabRocket2 = 
{
	angle_x =50;
	radius = 90;
	pos_dy = 10;
	channel = "main1";
};
InterfaceTV interfaceTV_LabRocket3 = 
{
	angle_x =50;
	radius = 90;
	pos_dy = 10;
	channel = "main2";
};
InterfaceTV interfaceTV_LabLaser1 = 
{
	angle_x =50;
	radius = 150;
	pos_dy = 0;
	channel = "main";
};
InterfaceTV interfaceTV_LabLaser2 = 
{
	angle_x =50;
	radius = 120;
	pos_dy = -8;
	channel = "main1";
};
InterfaceTV interfaceTV_LabLaser3 = 
{
	angle_x =50;
	radius = 120;
	pos_dy = -8;
	channel = "main2";
};
InterfaceTV interfaceTV_LabElectro1 = 
{
	angle_x =50;
	radius = 150;
	pos_dy = 0;
	channel = "main";
};
InterfaceTV interfaceTV_LabElectro2 = 
{
	angle_x =50;
	radius = 120;
	pos_dy = -8;
	channel = "main1";
};
InterfaceTV interfaceTV_LabElectro3 = 
{
	angle_x =50;
	radius = 120;
	pos_dy = -8;
	channel = "main2";
};
InterfaceTV interfaceTV_LabBomb1 = 
{
	angle_x =50;
	radius = 100;
	pos_dy = 7;
	channel = "main";
};
InterfaceTV interfaceTV_LabBomb2 = 
{
	angle_x =50;
	radius = 100;
	pos_dy = -5;
	channel = "main1";
};
InterfaceTV interfaceTV_LabBomb3 = 
{
	angle_x =50;
	radius = 100;
	pos_dy = -5;
	channel = "main2";
};
InterfaceTV interfaceTV_LabAntigrav1 = 
{
	angle_x =50;
	radius = 100;
	pos_dy = 12;
	channel = "main";
};
InterfaceTV interfaceTV_LabAntigrav2 = 
{
	angle_x =50;
	radius = 90;
	pos_dy = 12;
	channel = "main1";
};
InterfaceTV interfaceTV_LabSubterra1 = 
{
	angle_x =50;
	radius = 90;
	pos_dy = 0;
	channel = "main";
};
InterfaceTV interfaceTV_LabSubterra2 = 
{
	angle_x =50;
	radius = 90;
	pos_dy = 0;
	channel = "main1";
};
InterfaceTV interfaceTV_Soldier = 
{
	//солдат
	angle_x =50;
	radius = 85;
	pos_dy = -3;
};
InterfaceTV interfaceTV_TerrainMaster = 
{
	//бригадир
	angle_x =50;
	radius = 70;
	pos_dy = 10;
};
InterfaceTV interfaceTV_BuildMaster = 
{
	//прораб
	angle_x =50;
	radius = 80;
	pos_dy = 10;
};

/////////
InterfaceTV interfaceTV_Digger= 
{
	angle_x =50;
	radius = 120;
	pos_dy = 10;
};

InterfaceTV interfaceTV_Piercer= 
{
	angle_x =50;
	radius = 60;
	pos_dy = 10;
};

InterfaceTV interfaceTV_Strafer= 
{
	angle_x =50;
	radius = 80;
	pos_dy = 0;
};

InterfaceTV interfaceTV_Disintegrator= 
{
	angle_x =50;
	radius = 220;
	pos_dy = 20;
};

InterfaceTV interfaceTV_Extirpator= 
{
	angle_x =50;
	radius = 70;
	pos_dy = 0;
};

InterfaceTV interfaceTV_Leech= 
{
	angle_x =50;
	radius = 90;
	pos_dy = 10;
};

InterfaceTV interfaceTV_Scumer= 
{
	angle_x =50;
	radius = 80;
	pos_dy = 10;
};

InterfaceTV interfaceTV_FilthSpot0= 
{
	angle_x =50;
	radius = 75;
	pos_dy = -10;
};

InterfaceTV interfaceTV_FilthSpot1= 
{
	angle_x =50;
	radius = 75;
	pos_dy = -10;
};

InterfaceTV interfaceTV_FilthSpot2= 
{
	angle_x =50;
	radius = 75;
	pos_dy = -10;
};

InterfaceTV interfaceTV_FilthSpot3= 
{
	angle_x =50;
	radius = 70;
	pos_dy = -10;
};

InterfaceTV interfaceTV_station_exodus= 
{
	angle_x =50;
	radius = 90;
	pos_dy = -20;
};
InterfaceTV interfaceTV_station_empire= 
{
	angle_x =50;
	radius = 120;
	pos_dy = -12;
};
InterfaceTV interfaceTV_station_harkback= 
{
	angle_x =50;
	radius = 100;
	pos_dy = 0;
};



float small_camera_x = 416.0f / 1024.0f;
float small_camera_y = (885.0f - 256.0f) / 768.0f;
float small_camera_rect_dx  = 135.0f / 1024.0f;
float small_camera_rect_dy  = 115.0f / 768.0f;

float small_model_rate = 200;

//параметры камеры и моделей в окошке
///////////////////////////////////////////////////////////////////////////////////////////////////

