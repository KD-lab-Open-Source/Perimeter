#ifndef PERIMETER_SERIALIZATIONMACRO_H
#define PERIMETER_SERIALIZATIONMACRO_H

#define SERIALIZATION_TYPE_NAME \
    const char* type_name() const

#define SERIALIZATION_TYPE_CLASS_NAME \
    static const char* type_class_name();

#define SERIALIZATION_TYPE_NAME_VIRTUAL \
    virtual SERIALIZATION_TYPE_NAME = 0

#define SERIALIZATION_TYPE_NAME_OVERRIDE \
    SERIALIZATION_TYPE_NAME override

//Wraps pointer accepting serialize() into ref arg based
#define SERIALIZE_REF(VAR_AR) \
    template<class TA> \
    void serialize(TA* ar) { this->serialize_ref<TA>(*ar); } \
    template<class TA> \
    void serialize_ref(TA& VAR_AR)

/////////////////////////////////////////////////////////////////////////////

#define SERIALIZATION_TYPE_NAME_IMPL(className) \
    const char* className::type_name() const { return #className; } \
    const char* className::type_class_name() { return #className; }

#define SERIALIZE(VAR_AR) \
    SERIALIZATION_TYPE_NAME; \
    SERIALIZATION_TYPE_CLASS_NAME; \
    SERIALIZE_REF(VAR_AR)

#endif //PERIMETER_SERIALIZATIONMACRO_H
