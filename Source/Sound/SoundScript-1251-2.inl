	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(volume, "Громкость звука 0..1 (По умолчанию: 1)");
		ar & TRANSLATE_OBJECT(volmin, "Минимальная громкость звука 0..1 (По умолчанию: 0)");
	
		ar & TRANSLATE_OBJECT(language_dependency, "звук разный для разных языков");
		ar & TRANSLATE_OBJECT(belligerent_dependency, "звук разный для разных воюющих сторон");

		ar & TRANSLATE_OBJECT(soundName, "Название звука");
		ar & TRANSLATE_OBJECT(additionalNames, "Дополнительные имена для RND");

		ar & TRANSLATE_OBJECT(radius, "Величина объекта (только в 3D) с этого момента громкость звука начинает убывать (По умолчанию: 50)");
		ar & TRANSLATE_OBJECT(max_radius, "Максимальное расстояние до объекта, после которого звук перестаёт быть слышимым (По умолчанию: oo)");

		ar & TRANSLATE_OBJECT(max_num_sound, "Максимальное количество одновременно звучащих звуков (По умолчанию: 5)");

		ar & TRANSLATE_OBJECT(delta, "Дельта");
	}
