	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(playerID);
		ar & WRAP_OBJECT(realPlayerType);
		ar & TRANSLATE_OBJECT(belligerent, "Сторона");
		ar & TRANSLATE_OBJECT(colorIndex, "Цвет");
		ar & TRANSLATE_OBJECT(clan, "Клан");
		ar & TRANSLATE_OBJECT(difficulty, "Сложность");
		ar & TRANSLATE_OBJECT(handicap, "Гандикап");

		ar & WRAP_OBJECT(flag_playerStartReady);
		ar & WRAP_OBJECT(flag_playerGameReady);
		ar & WRAP_OBJECT(compAndUserID);
		ar & WRAP_OBJECT(gameVersion);
		string name = playerName;
		ar & WRAP_OBJECT(name);
		setName(name.c_str());
		//ar & WRAP_OBJECT(playerName);

		if(ar.isInput() && !handicap)
			handicap = 100;
	}
