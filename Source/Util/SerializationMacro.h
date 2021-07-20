#ifndef PERIMETER_SERIALIZATIONMACRO_H
#define PERIMETER_SERIALIZATIONMACRO_H

#define SERIALIZATION_TYPE_NAME \
    std::string type_name() const { return get_type_id(this); }

#define SERIALIZE(VAR_AR) \
    SERIALIZATION_TYPE_NAME; \
    template<class Archive> \
    void serialize(Archive& VAR_AR)

#endif //PERIMETER_SERIALIZATIONMACRO_H
