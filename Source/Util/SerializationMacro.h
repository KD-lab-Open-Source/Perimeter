#ifndef PERIMETER_SERIALIZATIONMACRO_H
#define PERIMETER_SERIALIZATIONMACRO_H

#define SERIALIZE(VAR_AR) \
    template<class Archive> \
    void serialize(Archive& VAR_AR)

#endif //PERIMETER_SERIALIZATIONMACRO_H
