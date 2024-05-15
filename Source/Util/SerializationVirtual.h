#ifndef PERIMETER_SERIALIZATIONVIRTUAL_H
#define PERIMETER_SERIALIZATIONVIRTUAL_H

// Cursed workaround to make serialize virtual while keeping templates
// These macros create virtual serialize impls that forward calls to current class templated serialize methods
// each Archive type needs its own methods, hence the I and O types

#include "Handle.h"
#include "Serialization.h"
#include "SerializationMacro.h"

///////////////////////////////////////////////////////////////////////////
// Serialize virtual macros
///////////////////////////////////////////////////////////////////////////

//Virtual declarations for base classes/structs
#define VIRTUAL_SERIALIZE_VIRTUAL(TYPE_I, TYPE_O, VAR_AR) \
    virtual void serialize(class TYPE_O* VAR_AR) { xassert(0); abort(); } \
    virtual void serialize(class TYPE_I* VAR_AR) { xassert(0); abort(); }

//Forwarders that override virtual serialize to call serialize_template
#define VIRTUAL_SERIALIZE_FORWARDERS(TYPE_I, TYPE_O, VAR_AR) \
    void serialize(class TYPE_O* VAR_AR) override { this->serialize_template(*VAR_AR); } \
    void serialize(class TYPE_I* VAR_AR) override { this->serialize_template(*VAR_AR); }

///////////////////////////////////////////////////////////////////////////
// Serialization types and macros
// Only these should be public available
///////////////////////////////////////////////////////////////////////////

#define VIRTUAL_SERIALIZE(VAR_AR) \
public: \
    SERIALIZATION_TYPE_CLASS_NAME; \
    SERIALIZATION_TYPE_NAME_OVERRIDE; \
    VIRTUAL_SERIALIZE_FORWARDERS(EditIArchive, EditOArchive, VAR_AR) \
    VIRTUAL_SERIALIZE_FORWARDERS(BinaryIArchive, BinaryOArchive, VAR_AR) \
    VIRTUAL_SERIALIZE_FORWARDERS(XPrmIArchive, XPrmOArchive, VAR_AR) \
protected: \
    template<class TA> \
    void serialize_template(TA& VAR_AR)

// Types that declare virtuals

struct SerializeVirtual {
public:
    virtual ~SerializeVirtual() = default;
    SERIALIZATION_TYPE_NAME_VIRTUAL;

    VIRTUAL_SERIALIZE_VIRTUAL(EditIArchive, EditOArchive, ar);
    VIRTUAL_SERIALIZE_VIRTUAL(BinaryIArchive, BinaryOArchive, ar);
    VIRTUAL_SERIALIZE_VIRTUAL(XPrmIArchive, XPrmOArchive, ar);
};

struct ShareHandleBaseSerializeVirtual: ShareHandleBase {
    SERIALIZATION_TYPE_NAME_VIRTUAL;

    VIRTUAL_SERIALIZE_VIRTUAL(EditIArchive, EditOArchive, ar);
    VIRTUAL_SERIALIZE_VIRTUAL(BinaryIArchive, BinaryOArchive, ar);
    VIRTUAL_SERIALIZE_VIRTUAL(XPrmIArchive, XPrmOArchive, ar);
};

#endif //PERIMETER_SERIALIZATIONVIRTUAL_H
