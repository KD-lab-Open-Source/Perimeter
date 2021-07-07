	SERIALIZE(ar) {
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(is3D, "звуки трёхмерные");
		ar & TRANSLATE_OBJECT(data, "звуки");
	}
