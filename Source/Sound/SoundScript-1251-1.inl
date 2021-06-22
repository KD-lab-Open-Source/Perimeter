template<class Archive>
void serialize(Archive& ar) {
    ar & TRANSLATE_OBJECT(enable, "&включить");
    ar & TRANSLATE_OBJECT(random, "выбирать частоту звука случайно при его запуске");
    ar & TRANSLATE_OBJECT(up, "Во сколько раз частота звука может увеличиваться (0..2)");
    ar & TRANSLATE_OBJECT(down, "Во сколько раз частота звука может увеличиться (0..20)");
}
