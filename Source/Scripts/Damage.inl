
// параметры повреждений, наносимых юниту
struct DamageData
{
	// ширина атаки
	int width = 0;
	// мощность атаки
	int power = 0;

	// фильтр на элементы, из которых берутся атомы для атаки
	// значения: DAMAGE_FILTER_ALL или комбинация из других значений DamageElementFilter
	int attackFilter = DAMAGE_FILTER_ALL;
	// фильтр на элементы, которые будут гибнуть при успешном попадании
	int damageFilter = DAMAGE_FILTER_ALL;

delegate:
	void clear() { width = power = 0; attackFilter = damageFilter = DAMAGE_FILTER_ALL; }
};

// параметры повреждений, наносимых миром (хаос, поврежденный зерослой под зданиями)
struct EnvironmentalDamageData
{
	// тип повреждений
	EnvironmentalDamageType damageType = ENV_DAMAGE_CHAOS;
	// параметры повреждений, [0] - минимум, [1] - максимум
	// если width == -1, то туда
	// подставляется общее количество атомов юнита
	DamageData damageData[2] = {};

	// периодичность нанесения повреждений, все в миллисекундах
	//
	// повреждения наносятся через время t = period + periodDelta * (1 - damage_ratio)
	// damage_ratio - величина, определяющая интенсивность повреждений,
	// отношение площади наносящей повреждение поверхности (хаос или нарушенный зерослой)
	// к общей площади, занимаемой юнитом
	int period = 0;
	int periodDelta = 0;

	// минимальное значение damage_ratio,
	// при котором юниту начинают наноситься повреждения
	float damageRatioMin = 0.01;

delegate:

	const DamageData damage(float damage_ratio) const 
	{
		DamageData data = damageData[0];

		if(data.width != -1)
			data.width += round(float(damageData[1].width - damageData[0].width) * damage_ratio);

		data.power += round(float(damageData[1].power - damageData[0].power) * damage_ratio);

		return data;
	}
};

// параметры наносимых юнитом повреждений
struct terUnitDamageData
{
	// повреждения, наносимые непосредственно в точке попадания
	DamageData mainDamage = { };

	int splashDamageRadius = 0;
	// повреждения, наносимые в радиусе splashDamageRadius от точки попадания
	DamageData splashDamage = { };

delegate:

	// оценка количества элементов, вышибаемых с одного раза
	int estimatedDamage() const { return ((mainDamage.power + splashDamage.power + 3)/8 + 1); }
};

// повреждения, которые хаос наносит юнитам
EnvironmentalDamageData chaosDamageDefault = 
{
	damageType = ENV_DAMAGE_CHAOS;
	damageData = {
		{ 
			power = 1; 
			width = 30; 
		},
		{ 
			power = 2; 
			width = 30; 
		}
	};

	period = 500;
	periodDelta = 100;

	damageRatioMin = 0.1;
};

// повреждения, которые хаос наносит подземным юнитам
EnvironmentalDamageData chaosDamageSubterranean = 
{
	damageType = ENV_DAMAGE_CHAOS;
	damageData = { 
		{ 
			power = 2; 
			width = 30; 
		},
		{ 
			power = 8; 
			width = 30; 
		}
	};

	period = 100;
	periodDelta = 100;

	damageRatioMin = 0.1;
};

// повреждения, которые получают здания от поврежденного зерослоя
EnvironmentalDamageData basementDamageDefault = 
{
	damageType = ENV_DAMAGE_BASEMENT;
	damageData = { 
		{ 
			power = 1; 
			width = 1; 
		},
		{ 
			power = 80; 
			width = 100; 
		}
	};

	period = 100;
	periodDelta = 1000;

	damageRatioMin = 0.01;
};
