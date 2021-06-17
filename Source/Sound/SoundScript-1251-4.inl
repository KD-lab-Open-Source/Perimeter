	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(table, "table");
		ar & TRANSLATE_OBJECT(belligerentPrefix, "Префикс имени файла для звуков, которые разные для разных воюющих сторон");
	}
