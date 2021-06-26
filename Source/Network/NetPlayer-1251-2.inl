	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(version);
		ar & TRANSLATE_OBJECT(worldName, "Имя мира");
		ar & TRANSLATE_NAME(missionDescriptionID, "missionDescription", "Описание миссии");
		ar & TRANSLATE_OBJECT(difficulty, "Уровень сложности");
		ar & TRANSLATE_OBJECT(playersData, "Игроки");
		ar & WRAP_OBJECT(missionNumber);
		
		ar & TRANSLATE_OBJECT(playerAmountScenarioMax, "Максимальное количество игроков");
		ar & WRAP_OBJECT(playersShufflingIndices);
		ar & WRAP_OBJECT(activePlayerID);
		
		ar & WRAP_OBJECT(globalTime);
		ar & WRAP_OBJECT(gameSpeed);
		ar & WRAP_OBJECT(gamePaused);

		ar & WRAP_OBJECT(originalSaveName);
	}
