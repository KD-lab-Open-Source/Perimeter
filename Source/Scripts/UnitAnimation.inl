
//---------------------------------------------------

// Параметры анимационной цепочки
struct AnimationChain
{
	ChainID chainID = CHAIN_NONE;
	string chainName = "";
	float phase = 0;   // 0..1
	int period = 2000; // mS
	ChainNodeID begin = CHAIN_NODE_NONE;
	ChainNodeID end = CHAIN_NODE_NONE;
	int periodDeviation = 0; // !0: period += rnd(perionDeviation), phase = rnd(1)
	int enableReverse = 0; // Разрешить реверс между смежными цепочками
	int counter = 0; // for path finding
	SoundEventID soundID = SOUND_EVENT_NONE;
delegate:
	bool cycled() const { return begin == end; }
	float phaseEnd() const { return period > 0 ? 1 : (period < 0 ? 0 : phase); }
};

// Набор анимационных цепочек для определенного узла
struct AnimationData
{
	AnimationGroupID groupID = ANIMATION_GROUP_ROOT;
	string groupName = 0;
	AnimationChain* chains = new AnimationChain[int size] {};
	int setPhaseRecursive = 0;
delegate:
	const AnimationChain* initial() const { return size ? &chains[0] : 0; }	// первая цепочка включается при создании объекта
	const AnimationChain* find(ChainID chainID) const  { for(int i = 0; i < size; i++) if(chains[i].chainID == chainID) return &chains[i]; return 0; }
};

// Массив наборов для модели
struct AnimationSetup
{
	AnimationData* data = new AnimationData[int size] {};
delegate:
	const AnimationData* root() const { return size ? &data[0] : 0; }
};

//---------------------------------------------------

struct DockingSlotData
{
	string position = "";
};

struct DockingSlotsSetup
{
	DockingSlotData* data = new DockingSlotData[int size] {};
};


//------------------------------------------------------

// Параметры анимации юнитов и зданий.

AnimationChain buildChain1 = { chainID = CHAIN_BUILD1; chainName = "build1"; phase = 0; period = 1000;
				begin = CHAIN_NODE_BUILD0; end = CHAIN_NODE_BUILD1; };
AnimationChain buildChain2 = { chainID = CHAIN_BUILD2; chainName = "build2"; phase = 0; period = 1000;
				begin = CHAIN_NODE_BUILD1; end = CHAIN_NODE_BUILD2; };
AnimationChain buildChain3 = { chainID = CHAIN_BUILD3; chainName = "build3"; phase = 0; period = 1000;
				begin = CHAIN_NODE_BUILD2; end = CHAIN_NODE_BUILD3; };
AnimationChain buildChain4 = { chainID = CHAIN_BUILD4; chainName = "build4"; phase = 0; period = 1000;
				begin = CHAIN_NODE_BUILD3; end = CHAIN_NODE_BUILD4; };
AnimationChain buildChain5 = { chainID = CHAIN_BUILD5; chainName = "build5"; phase = 0; period = 1000;
				begin = CHAIN_NODE_BUILD4; end = CHAIN_NODE_BUILD5; };
AnimationChain buildChain6 = { chainID = CHAIN_BUILD6; chainName = "build6"; phase = 0; period = 1000;
				begin = CHAIN_NODE_BUILD5; end = CHAIN_NODE_BUILD6; };
AnimationChain buildChain7 = { chainID = CHAIN_BUILD7; chainName = "build7"; phase = 0; period = 1000;
				begin = CHAIN_NODE_BUILD6; end = CHAIN_NODE_BUILD7; };
AnimationChain buildChain8 = { chainID = CHAIN_BUILD8; chainName = "build8"; phase = 0; period = 1000;
				begin = CHAIN_NODE_BUILD7; end = CHAIN_NODE_BUILD8; };

AnimationChain unbuildChain1 = { chainID = CHAIN_UNBUILD1; chainName = "build1"; phase = 1; period = -1000;
				begin = CHAIN_NODE_BUILD1; end = CHAIN_NODE_BUILD0; };
AnimationChain unbuildChain2 = { chainID = CHAIN_UNBUILD2; chainName = "build2"; phase = 1; period = -1000;
				begin = CHAIN_NODE_BUILD2; end = CHAIN_NODE_BUILD1; };
AnimationChain unbuildChain3 = { chainID = CHAIN_UNBUILD3; chainName = "build3"; phase = 1; period = -1000;
				begin = CHAIN_NODE_BUILD3; end = CHAIN_NODE_BUILD2; };
AnimationChain unbuildChain4 = { chainID = CHAIN_UNBUILD4; chainName = "build4"; phase = 1; period = -1000;
				begin = CHAIN_NODE_BUILD4; end = CHAIN_NODE_BUILD3; };
AnimationChain unbuildChain5 = { chainID = CHAIN_UNBUILD5; chainName = "build5"; phase = 1; period = -1000;
				begin = CHAIN_NODE_BUILD5; end = CHAIN_NODE_BUILD4; };
AnimationChain unbuildChain6 = { chainID = CHAIN_UNBUILD6; chainName = "build6"; phase = 1; period = -1000;
				begin = CHAIN_NODE_BUILD6; end = CHAIN_NODE_BUILD5; };
AnimationChain unbuildChain7 = { chainID = CHAIN_UNBUILD7; chainName = "build7"; phase = 1; period = -1000;
				begin = CHAIN_NODE_BUILD7; end = CHAIN_NODE_BUILD6; };
AnimationChain unbuildChain8 = { chainID = CHAIN_UNBUILD8; chainName = "build8"; phase = 1; period = -1000;
				begin = CHAIN_NODE_BUILD8; end = CHAIN_NODE_BUILD7; };


AnimationSetup coreAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "off position"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "off"; phase = 1; period = -1000;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "off"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 2000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_UPGRADE_PREPARE; chainName = "making"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_UPGRADE_START; },

				{ chainID = CHAIN_UPGRADE; chainName = "making"; phase = 0; period = 1000;
					begin = CHAIN_NODE_UPGRADE_START; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{}
			};
		},
		{
			groupID = ANIMATION_GROUP_LIGHTING;
			groupName = "group lighting";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCH_ON; chainName = "lighting_on"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "lighting_on"; phase = 1; period = -1000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		},
		{
			groupID = ANIMATION_GROUP_CHARGING;
			groupName = "group loading";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DISCHARGED; chainName = "loading_off"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DISCHARGED; end = CHAIN_NODE_DISCHARGED; },

				{ chainID = CHAIN_CHARGE; chainName = "loading_on"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DISCHARGED; end = CHAIN_NODE_CHARGED; enableReverse = 1; },
					
				{ chainID = CHAIN_CHARGED; chainName = "loading_full"; phase = 0; period = 1000;
				begin = CHAIN_NODE_CHARGED; end = CHAIN_NODE_CHARGED; },

				{ chainID = CHAIN_DISCHARGE; chainName = "loading_on"; phase = 1; period = -1000;
				begin = CHAIN_NODE_CHARGED; end = CHAIN_NODE_DISCHARGED; enableReverse = 1; }
			};
		}
	};
};

AnimationSetup transmitterAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "off position"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "off"; phase = 1; period = -1000;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "off"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 2000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{ end = CHAIN_NODE_SWITCHED_OFF; },
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{ begin = CHAIN_NODE_SWITCHED_OFF; }
			};
		},
		{
			groupID = ANIMATION_GROUP_LIGHTING;
			groupName = "group lighting";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCH_ON; chainName = "lighting_on"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "lighting_on"; phase = 1; period = -1000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup commandCenterAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
					
				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{}
			};
		}
	};
};

AnimationSetup staticBombAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
					
				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{}
			};
		}
	};
};

AnimationSetup lab1Animation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
					
				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{}, buildChain6{}, buildChain7{}, buildChain8{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{}, unbuildChain6{}, unbuildChain7{}, unbuildChain8{}
			};
		}
	};
};

AnimationSetup lab2Animation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_UPGRADE_PREPARE; chainName = "making1"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_UPGRADE_START; },

				{ chainID = CHAIN_UPGRADE; chainName = "making1"; phase = 0; period = 1000;
					begin = CHAIN_NODE_UPGRADE_START; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main1"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main1"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; }
			};
		}
	};
};

AnimationSetup lab3Animation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_UPGRADE_PREPARE; chainName = "making2"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_UPGRADE_START; },

				{ chainID = CHAIN_UPGRADE; chainName = "making2"; phase = 0; period = 1000;
					begin = CHAIN_NODE_UPGRADE_START; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main2"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main2"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; }
			};
		}
	};
};

AnimationSetup plantAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_PRODUCTION; chainName = "prodaction"; phase = 0; period = 2000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_OPEN; chainName = "out"; phase = 0; period = 500;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_OPENED; },

				{ chainID = CHAIN_CLOSE; chainName = "out"; phase = 0; period = 1;
					begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{}, buildChain6{}, buildChain7{}, buildChain8{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{}, unbuildChain6{}, unbuildChain7{}, unbuildChain8{}
			};
		}
	};
};

AnimationSetup filthNavigatorAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_FIRE; chainName = "filth"; phase = 0; period = 5000;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{}, 
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{} 
			};
		}
	};
};

AnimationSetup ballisticLauncherAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_PRODUCTION; chainName = "creation"; phase = 0; period = 4000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_OPEN; chainName = "open"; phase = 0; period = 2000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_OPENED; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "open stand"; phase = 0; period = 3000;
					begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_OPENED; },

				{ chainID = CHAIN_FIRE_PREPARE; chainName = "fire"; phase = 0; period = 3000;
					begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_FIRE; chainName = "open stand"; phase = 0; period = 3000;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_OPENED; },

				{ chainID = CHAIN_CLOSE; chainName = "open"; phase = 1; period = -2000;
					begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, 
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}
			};
		}
	};
};

AnimationSetup scumDisruptorAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "disruptor on"; phase = 0; period = 2000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_OPENED; },

				{ chainID = CHAIN_FIRE; chainName = "disruptor"; phase = 0; period = 3000;
					begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_OPENED; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "disruptor on"; phase = 1; period = -2000;
					begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{}, 
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{} 
			};
		}
	};
};

AnimationSetup collectorAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 3000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
						
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "off"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{}
			};
		}
	};
};

AnimationSetup corridorAlphaAnimation = {
	data = new AnimationData {
		groupID = ANIMATION_GROUP_ROOT;
		chains = new AnimationChain[] {
			{ chainID = CHAIN_CLOSED; chainName = "stop"; phase = 0; period = 1000; 
			begin = CHAIN_NODE_CLOSED; end = CHAIN_NODE_CLOSED; },
			{ chainID = CHAIN_OPEN; chainName = "main"; phase = 0; period = 1000; soundID = SOUND_EVENT_OPEN;
			begin = CHAIN_NODE_CLOSED; end = CHAIN_NODE_OPENED; },
			{ chainID = CHAIN_OPENED; chainName = "effect"; phase = 0; period = 1000; 
			begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_OPENED; },
			{ chainID = CHAIN_CLOSE; chainName = "main"; phase = 1; period = -1000; 
			begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_CLOSED; }
		};
	};   	  
};

AnimationSetup corridorOmegaAnimation = {
	data = new AnimationData {
		groupID = ANIMATION_GROUP_ROOT;
		chains = new AnimationChain[] {
			{ chainID = CHAIN_CLOSED; chainName = "main"; phase = 0; period = 1000; 
			begin = CHAIN_NODE_CLOSED; end = CHAIN_NODE_CLOSED; },
			{ chainID = CHAIN_OPEN; chainName = "mount"; phase = 0; period = 4000; soundID = SOUND_EVENT_OPEN;
			begin = CHAIN_NODE_CLOSED; end = CHAIN_NODE_OPENED; },
			{ chainID = CHAIN_OPENED; chainName = "effect"; phase = 0; period = -30000; 
			begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_OPENED; },
			{ chainID = CHAIN_CLOSE; chainName = "mount"; phase = 1; period = -4000; 
			begin = CHAIN_NODE_OPENED; end = CHAIN_NODE_CLOSED; }
		};
	};
};

AnimationSetup frameAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DETACHED; chainName = "fly"; phase = 0; period = 1000; 
				begin = CHAIN_NODE_DETACHED; end = CHAIN_NODE_DETACHED; },
					
				{ chainID = CHAIN_ATTACHED; chainName = "main"; phase = 0; period = 1000; 
				begin = CHAIN_NODE_ATTACHED; end = CHAIN_NODE_ATTACHED; },
					
				{ chainID = CHAIN_ATTACH; chainName = "mount"; phase = 0; period = 1000; 
				begin = CHAIN_NODE_DETACHED; end = CHAIN_NODE_ATTACHED; },
					
				{ chainID = CHAIN_DETACH; chainName = "mount"; phase = 1; period = -1000;
				begin = CHAIN_NODE_ATTACHED; end = CHAIN_NODE_DETACHED; },
					
				{ chainID = CHAIN_DETACHED_OUTPUT; chainName = "fly getout"; phase = 0; period = 3000; 
				begin = CHAIN_NODE_DETACHED; end = CHAIN_NODE_DETACHED_OPENED; },
					
				{ chainID = CHAIN_DETACHED_INPUT; chainName = "fly getout"; phase = 1; period = -3000; 
				begin = CHAIN_NODE_DETACHED_OPENED; end = CHAIN_NODE_DETACHED; },
					
				{ chainID = CHAIN_ATTACHED_OUTPUT; chainName = "mount getout"; phase = 0; period = 3000; 
				begin = CHAIN_NODE_ATTACHED; end = CHAIN_NODE_ATTACHED_OPENED; },
					
				{ chainID = CHAIN_ATTACHED_INPUT; chainName = "mount getout"; phase = 1; period = -3000; 
				begin = CHAIN_NODE_ATTACHED_OPENED; end = CHAIN_NODE_ATTACHED; }
			};
		},
			
		{	// Постоянно включенная анимация города
			groupID = ANIMATION_GROUP_CITY;
			groupName = "group rotate";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "rotate"; phase = 0; period = 2000; 
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		},
			
		{	// Колонны с огоньками
			groupID = ANIMATION_GROUP_PILLAR;
			groupName = "group lighting";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "lighting_off"; phase = 0; period = 2000; 
				begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },
					
				{ chainID = CHAIN_SWITCH_ON; chainName = "lighting_on"; phase = 0; period = 2000; 
				begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },
					
				{ chainID = CHAIN_SWITCHED_ON; chainName = "lighting_main"; phase = 0; period = 2000; 
				begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
					
				{ chainID = CHAIN_SWITCH_OFF; chainName = "lighting_on"; phase = 1; period = -2000; 
				begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		},

		{	// Оживляж
			groupID = ANIMATION_GROUP_ANIMATE;
			groupName = "group truck";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "truck_in"; phase = 0; period = 2000; 
				begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; },

				//{ chainID = CHAIN_SWITCHED_OFF; chainName = "truck_out"; phase = 0; period = 0; 
				//begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },
					
				{ chainID = CHAIN_SWITCH_ON; chainName = "truck_out"; phase = 0; period = 2000; 
				begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },
					
				{ chainID = CHAIN_SWITCHED_ON; chainName = "truck_anima"; phase = 0; period = 10000; 
				begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; }
					
			};
		}

	};
};

AnimationSetup laserGunAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
						
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "off"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 400;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup electroGunAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
						
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "off"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 400;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup howitzerGunAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
						
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 200;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup subchaserGunAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_ON; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },
						
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_SWITCHED_ON; },

				buildChain1{}, buildChain2{}, buildChain3{}, buildChain4{}, buildChain5{},
				unbuildChain1{}, unbuildChain2{}, unbuildChain3{}, unbuildChain4{}, unbuildChain5{}
			};
		}
	};
};

AnimationSetup rprojectorAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "stop"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		}
	};
};

AnimationSetup chaserAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "stop"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 200;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup sniperAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 200;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup leamoAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 400;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup impalerAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 500;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup mortarAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 700;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup buildMasterAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 5000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 5000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		}
	};
};

AnimationSetup terrainMasterAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 5000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 5000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		}
	};
};

AnimationSetup rocketAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "off"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "on"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		}
	};
};

AnimationSetup ballisticRocketAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "off"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "on"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		}
	};
};

AnimationSetup wargonAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 500;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 200;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup subterraneanAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "off"; phase = 0; period = 300;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_SWITCH_OFF; chainName = "main off"; phase = 0; period = 500;
					begin = CHAIN_NODE_UNDERGROUND; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "main off"; phase = 1; period = -500;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_UNDERGROUND; },

				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_UNDERGROUND; end = CHAIN_NODE_UNDERGROUND; }
			};
		}
	};
};

AnimationSetup ceptorAnimation = {
	data = new AnimationData[] {
		{
			setPhaseRecursive = 1;
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
/*
				{ chainID = CHAIN_STOP; chainName = "land"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_TAKEOFF; chainName = "flight"; phase = 0; period = 500;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_FLIGHT; },

				{ chainID = CHAIN_LANDING; chainName = "landing"; phase = 0; period = 500;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_GROUND; },
*/

				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; },

				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; }
			};
		}
	};
};

AnimationSetup gyroidAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; },

				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 200;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup eflairAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; },

				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 200;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup straferAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "fly"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "fly"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 300;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup officerAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			groupName = "group bashnia";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "land"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_TAKEOFF; chainName = "flight"; phase = 0; period = 500;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_FLIGHT; },

				{ chainID = CHAIN_LANDING; chainName = "flight"; phase = 1; period = -500;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_MOVE; chainName = "fly"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; }
			};
		}
	};
};

AnimationSetup soldierAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 500;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_TAKEOFF; chainName = "on"; phase = 0; period = 500;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_FLIGHT; },

				{ chainID = CHAIN_LANDING; chainName = "on"; phase = 1; period = -500;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_MOVE; chainName = "fly"; phase = 0; period = 500;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 200;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 1; period = -1;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup technicAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_SWITCH_ON; chainName = "start fire"; phase = 0; period = 500;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_SWITCHED_ON; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "start fire"; phase = 1; period = -500;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 400;
					begin = CHAIN_NODE_SWITCHED_ON; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }

			};
		}
	};
};

AnimationSetup bombieAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
/*
				{ chainID = CHAIN_STOP; chainName = "land"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_TAKEOFF; chainName = "landing"; phase = 1; period = -500;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_FLIGHT; },

				{ chainID = CHAIN_LANDING; chainName = "landing"; phase = 0; period = 500;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_GROUND; },
*/
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; },

				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_FLIGHT; end = CHAIN_NODE_FLIGHT; }
			};
		}
	};
};

AnimationSetup unseenAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "unseen"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		}
	};
};

AnimationSetup disintegratorAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "fly"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "fly"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_FIRE; chainName = "disintegrator"; phase = 0; period = 3000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		}
	};
};

AnimationSetup leechAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "leech up"; phase = 0; period = 2000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "leech up"; phase = 1; period = -2000;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_FIRE; chainName = "leech"; phase = 0; period = 2000;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_FIRE; }
			};
		}
	};
};

AnimationSetup extirpatorAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "attack"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "main"; phase = 0; period = 1;
				begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup scumSplitterAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_SWITCH_ON; chainName = "dismount"; phase = 0; period = 1000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_FIRE; chainName = "fire"; phase = 0; period = 2000;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "dismount"; phase = 1; period = -1000;
					begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_GROUND; }
			};
		}
	};
};

AnimationSetup scumTwisterAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_FIRE; chainName = "twister"; phase = 0; period = 1500;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		}
	};
};

AnimationSetup scumStormAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "twister"; phase = 0; period = 9000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "twister"; phase = 0; period = 9000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		}
	};
};

AnimationSetup scumHeaterAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 2000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 2000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_FIRE; chainName = "heater"; phase = 0; period = 2000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		}
	};
};

AnimationSetup conductorAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 2000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 2000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },

				{ chainID = CHAIN_FIRE; chainName = "heater"; phase = 0; period = 2000;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		}
	};
};

AnimationSetup scumThrowerAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; },
						
				{ chainID = CHAIN_MOVE; chainName = "main"; phase = 0; period = 100;
					begin = CHAIN_NODE_GROUND; end = CHAIN_NODE_GROUND; }
			};
		},
		{
			groupID = ANIMATION_GROUP_FIRE;
			groupName = "group fire";
			chains = new AnimationChain[] {
				{ chainID = CHAIN_SWITCHED_OFF; chainName = "main"; phase = 0; period = 1;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_SWITCHED_OFF; },

				{ chainID = CHAIN_FIRE; chainName = "attack"; phase = 0; period = 1000;
					begin = CHAIN_NODE_SWITCHED_OFF; end = CHAIN_NODE_FIRE; },

				{ chainID = CHAIN_SWITCH_OFF; chainName = "attack stnby"; phase = 1; period = -1;
				begin = CHAIN_NODE_FIRE; end = CHAIN_NODE_SWITCHED_OFF; }
			};
		}
	};
};

AnimationSetup crowAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "main"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
					
				{ chainID = CHAIN_CLOSE; chainName = "foldable"; phase = 0; period = 333;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_CLOSED; },
				{ chainID = CHAIN_DIE; chainName = "death"; phase = 0; period = 333;
				begin = CHAIN_NODE_CLOSED; end = CHAIN_NODE_DEAD; }
			};
		}
	};
};

AnimationSetup waspAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "fly"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
				{ chainID = CHAIN_DIE; chainName = "attack"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; }
			};
		}
	};
};

AnimationSetup waspEyeAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "fly"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
				{ chainID = CHAIN_DIE; chainName = "death"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; }
			};
		}
	};
};

AnimationSetup daemonAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "main"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
				{ chainID = CHAIN_SWITCHED_ON; chainName = "red"; phase = 0; period = 1000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		}
	};
};

AnimationSetup dragonHeadAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "main"; phase = 0; period = 2000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		}
	};

};

AnimationSetup ratAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "main"; phase = 0; period = 400;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
				{ chainID = CHAIN_DETACH; chainName = "death"; phase = 0; period = 800;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; }
			};
		}
	};
};

AnimationSetup snakeAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "move"; phase = 0; period = 400;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
				{ chainID = CHAIN_DETACH; chainName = "death"; phase = 0; period = 800;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; }
			};
		}
	};
};

AnimationSetup antAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "move"; phase = 0; period = 500;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
				{ chainID = CHAIN_DETACH; chainName = "death"; phase = 0; period = 800;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; }
			};
		}
	};
};

AnimationSetup ant2Animation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "move"; phase = 0; period = 500;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; },
				{ chainID = CHAIN_DETACH; chainName = "death"; phase = 0; period = 800;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; }
			};
		}
	};
};

AnimationSetup wormAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_BUILD1; chainName = "main"; phase = 0; period = 5000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; },
				{ chainID = CHAIN_BUILD2; chainName = "main"; phase = 1; period = -5000;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; },
				{ chainID = CHAIN_STOP; chainName = "main"; phase = 0; period = 1;
				begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEAD; }
			};
		}
	};
};

AnimationSetup blackHoleAnimation = {
	data = new AnimationData[] {
		{
			groupID = ANIMATION_GROUP_ROOT;
			chains = new AnimationChain[] {
				{ chainID = CHAIN_DEFAULT; chainName = "main"; phase = 0; period = 3000;
					begin = CHAIN_NODE_DEFAULT; end = CHAIN_NODE_DEFAULT; }
			};
		}
	};
};


/////////////////////////////////////////////////
//		Docking Slots
/////////////////////////////////////////////////
DockingSlotsSetup dockingSlotsFrame = { 
	data = new DockingSlotData[] {
		{ position = "way"; }
	};
};


DockingSlotsSetup dockingSlotsPlantSoldier = { 
	data = new DockingSlotData[] {
		{ position = "solider"; }
	};
};

DockingSlotsSetup dockingSlotsPlantOfficer = { 
	data = new DockingSlotData[] {
		{ position = "solider"; }
	};
};

DockingSlotsSetup dockingSlotsPlantTechnician = { 
	data = new DockingSlotData[] {
		{ position = "solider"; }
	};
};

DockingSlotsSetup dockingSlotsCeptor = { 
	data = new DockingSlotData[] {
		{ position = "targeting a 01"; },
		{ position = "targeting a 02"; },
		{ position = "targeting a 03"; },
		{ position = "targeting a 04"; }
	};
};

DockingSlotsSetup dockingSlotsRProjector = { 
	data = new DockingSlotData[] {
		{ position = "targeting a 01"; },
		{ position = "targeting a 02"; },
		{ position = "targeting a 03"; },
		{ position = "targeting a 04"; }
	};
};

DockingSlotsSetup dockingSlotsRocker = { 
	data = new DockingSlotData[] {
		{ position = "targeting a 01"; }
	};
};

DockingSlotsSetup dockingSlotsGunRocket = { 
	data = new DockingSlotData[] {
		{ position = "targeting a 01"; },
		{ position = "targeting a 02"; },
		{ position = "targeting a 03"; },
		{ position = "targeting a 04"; }
	};
};

DockingSlotsSetup dockingSlotsGunBallistic = { 
	data = new DockingSlotData[] {
		{ position = "targeting a"; }
	};
};

DockingSlotsSetup dockingSlotsDragonHead = { 
	data = new DockingSlotData[] {
		{ position = "targeting a"; }
	};
};

DockingSlotsSetup dockingSlotsDragonBody = { 
	data = new DockingSlotData[] {
		{ position = "targeting a"; }
	};
};

DockingSlotsSetup dockingSlotsDragonTail = { 
	data = new DockingSlotData[] {
		{ position = "targeting a"; }
	};
};

