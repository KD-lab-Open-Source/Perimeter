struct ModelProperty
{
	string name = "";
	float size = 0;
};

string terModelUnitSelection = "RESOURCE\\MODELS\\FX\\Select.m3d";
string terModelLightRay = "RESOURCE\\MODELS\\FX\\Lightray.m3d";

string terModelDefaultTree = "RESOURCE\\MODELS\\ENVIRONMENT\\Tree_Troam2.m3d";

ModelProperty terModelBuildingNoConnection = { name = "RESOURCE\\MODELS\\Main\\energy.m3d";  size = 8.3; };
ModelProperty terModelBuildingLowEnergy = { name = "RESOURCE\\MODELS\\Main\\energy1.m3d"; size = 10; };
ModelProperty terOmegaMinedIcon = { name = "RESOURCE\\MODELS\\Main\\mine_indicator.m3d";  size = 8.3; };
float terModelBuildingSpeed = 2000;

//string terModelWayFlag = "RESOURCE\\MODELS\\MAIN\\arrow.m3d";
//string terModelWayFlag = "RESOURCE\\MODELS\\MAIN\\arrow.m3d";
string terModelWayFlag = "RESOURCE\\MODELS\\MAIN\\obelisk.m3d";
float terModelWayFlagRadius = 16.0f;
float terModelWayFlagSpeed = 2000.0f;

//----------------------------------------

string terTextureField01 = "RESOURCE\\EFFECT\\envmap01.tga";
string terTextureField02 = "RESOURCE\\EFFECT\\tesla01.tga";
string terTextureHologram = "RESOURCE\\EFFECT\\lightmap.tga";
string terTextureUnitSelection = "RESOURCE\\EFFECT\\select.tga";

string terTextureCongregationUnit = "RESOURCE\\SPRITES\\Molel.tga";
string terTextureCongregationProtection = "RESOURCE\\SPRITES\\Protect.tga";
string terTextureCongregationAnnihilation = "RESOURCE\\SPRITES\\Annihilation.tga";

string terTextureATCFireTrail = "RESOURCE\\EFFECT\\sh_btr.tga";
string terTextureBlockFireTrail = "RESOURCE\\EFFECT\\sh_btr.tga";
string terTextureFighterFlyTrail = "RESOURCE\\EFFECT\\sh_gaub.tga";
string terTextureFighterFireTrail = "RESOURCE\\EFFECT\\corona.tga";
string terTextureSoldierFireTrail = "RESOURCE\\EFFECT\\sh_pulemet.tga";
string terTextureBomberFlyTrail = "RESOURCE\\EFFECT\\sh_gaub.tga";
string terTextureOfficerFireTrail = "RESOURCE\\EFFECT\\freeze.tga";
string terTextureTechnicFireTrail = "RESOURCE\\EFFECT\\sh_gaub.tga";
string terTexturePortFireTrail = "RESOURCE\\EFFECT\\corona.tga";
string terTextureLaserFireTrail = "RESOURCE\\EFFECT\\corona.tga";
string terTextureDebrisFlyTrail = "RESOURCE\\EFFECT\\sh_gaub.tga";

string terTextureLaserTrail = "RESOURCE\\EFFECT\\laser.tga";
string terTextureEnergyLinkTrail = "RESOURCE\\EFFECT\\corona.tga";
string terTextureFreezeTrail = "RESOURCE\\EFFECT\\corona.tga";

string terTextureLaser[int terTextureLaserSize] = { "RESOURCE\\EFFECT\\laser.tga", "RESOURCE\\EFFECT\\freeze.tga", "RESOURCE\\EFFECT\\laser_tech.tga" };
string terTextureLaserZ[int terTextureLaserSizeZ] = { "RESOURCE\\EFFECT\\laserZ.tga", "RESOURCE\\EFFECT\\freezeZ.tga", "RESOURCE\\EFFECT\\laser_techZ.tga" };

struct SetModelData
{
	string modelName = "";
	string logicName = 0;
	float scale = 1;
};

struct SetModelDataArray
{
	SetModelData table[int size = 4] = {};
delegate:
	const SetModelData& randomName() const;
};

struct SetModelDataArrayTable
{
	SetModelDataArray bullet = 
	{
		table = {
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\snar_osk1.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\snar_osk1.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\snar_osk2.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\snar_osk2.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\snar_osk3.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\snar_osk3.l3d";
			}
		};
	};

	SetModelDataArray insect = 
	{
		table = {
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_spider_01.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_spider_01.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_spider_02.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_spider_02.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_spider_03.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_spider_03.l3d";
			}
		};
	};

	SetModelDataArray unit = 
	{
		table = {
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_object_01.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_object_01.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_object_02.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_object_02.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_object_03.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_object_03.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_object_04.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_object_04.l3d";
			}
		};
	};

	SetModelDataArray structure = 
	{
		table = {
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_object_03.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_object_03.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_object_04.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_object_04.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\o_object_05.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\o_object_05.l3d";
			}
		};
	};

	SetModelDataArray ants = 
	{
		table = {
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\ant.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\ant.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\ant1.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\ant1.l3d";
			}
		};
	};

	SetModelDataArray snake = 
	{
		table = {
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\snake_01.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\snake_01.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\snake_02.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\snake_02.l3d";
			},
			{
				modelName = "RESOURCE\\MODELS\\MAIN\\snake_03.m3d";
				logicName = "RESOURCE\\MODELS\\MAIN\\snake_03.l3d";
			}
		};
	};
};

SetModelDataArrayTable setModelTable = {};
