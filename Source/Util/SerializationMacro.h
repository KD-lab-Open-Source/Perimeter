#ifndef PERIMETER_SERIALIZATIONMACRO_H
#define PERIMETER_SERIALIZATIONMACRO_H

#define SERIALIZATION_TYPE_NAME \
    const char* type_name() const

#define SERIALIZATION_TYPE_CLASS_NAME \
    const char* type_class_name() const

#define SERIALIZATION_TYPE_NAME_VIRTUAL \
    virtual SERIALIZATION_TYPE_NAME = 0

#define SERIALIZATION_TYPE_NAME_OVERRIDE \
    SERIALIZATION_TYPE_NAME override

/////////////////////////////////////////////////////////////////////////////

#define SERIALIZATION_TYPE_NAME_IMPL(className) \
    const char* className::type_name() const { return #className; } \
    const char* className::type_class_name() const { return #className; }

#define SERIALIZE(VAR_AR) \
    SERIALIZATION_TYPE_NAME; \
    SERIALIZATION_TYPE_CLASS_NAME; \
    template<class Archive> \
    void serialize(Archive& VAR_AR)

#endif //PERIMETER_SERIALIZATIONMACRO_H
