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
    virtual void serialize(TYPE_O& VAR_AR) { abort(); } \
    virtual void serialize(TYPE_I& VAR_AR) { abort(); }

//Forwarders that override virtual serialize to call serialize_template
#define VIRTUAL_SERIALIZE_FORWARDERS(TYPE_I, TYPE_O, VAR_AR) \
    void serialize(TYPE_O& VAR_AR) override { this->serialize_template(VAR_AR); } \
    void serialize(TYPE_I& VAR_AR) override { this->serialize_template(VAR_AR); }

//We need this to not mess vtables as each header would have different layout otherwise, these shouldn't be called in any form
#define VIRTUAL_SERIALIZE_PLACEHOLDER(TYPE_I, TYPE_O, VAR_AR) \
    virtual void serialize_placeholder_##TYPE_I(Placeholder##TYPE_I& VAR_AR) { abort(); } \
    virtual void serialize_placeholder_##TYPE_O(Placeholder##TYPE_O& VAR_AR) { abort(); }

#ifdef __EDIT_ARCHIVE_H__
#define VIRTUAL_SERIALIZE_FORWARDERS_EDIT(VAR_AR) VIRTUAL_SERIALIZE_FORWARDERS(EditIArchive, EditOArchive, VAR_AR)
#else
class PlaceholderEditIArchive;
class PlaceholderEditOArchive;
#define VIRTUAL_SERIALIZE_FORWARDERS_EDIT(VAR_AR)
#endif

#ifdef __BINARY_ARCHIVE_H__
#define VIRTUAL_SERIALIZE_FORWARDERS_BINARY(VAR_AR) VIRTUAL_SERIALIZE_FORWARDERS(BinaryIArchive, BinaryOArchive, VAR_AR)
#else
class PlaceholderBinaryIArchive;
class PlaceholderBinaryOArchive;
#define VIRTUAL_SERIALIZE_FORWARDERS_BINARY(VAR_AR)
#endif

#ifdef __XPRM_ARCHIVE_H__
#define VIRTUAL_SERIALIZE_FORWARDERS_XPRM(VAR_AR) VIRTUAL_SERIALIZE_FORWARDERS(XPrmIArchive, XPrmOArchive, VAR_AR)
#else
class PlaceholderXPrmIArchive;
class PlaceholderXPrmOArchive;
#define VIRTUAL_SERIALIZE_FORWARDERS_XPRM(VAR_AR)
#endif

#if defined(PERIMETER_SERIALIZATION_ARCHIVE_NEED) && !defined(__EDIT_ARCHIVE_H__) && !defined(__XPRM_ARCHIVE_H__) && !defined(__BINARY_ARCHIVE_H__)
#error included without adding any *Archive.h, this is required to provide one serialize forwarding at least
#endif

///////////////////////////////////////////////////////////////////////////
// Serialization types and macros
// Only these should be public available
///////////////////////////////////////////////////////////////////////////

#define VIRTUAL_SERIALIZE(VAR_AR) \
public: \
    SERIALIZATION_TYPE_CLASS_NAME; \
    SERIALIZATION_TYPE_NAME_OVERRIDE; \
    VIRTUAL_SERIALIZE_FORWARDERS_EDIT(VAR_AR) \
    VIRTUAL_SERIALIZE_FORWARDERS_BINARY(VAR_AR) \
    VIRTUAL_SERIALIZE_FORWARDERS_XPRM(VAR_AR) \
protected: \
    template<class Archive> \
    void serialize_template(Archive& VAR_AR)

// Types that declare virtuals

struct SerializeVirtual {
public:
    SERIALIZATION_TYPE_NAME_VIRTUAL;

#ifdef __EDIT_ARCHIVE_H__
    VIRTUAL_SERIALIZE_VIRTUAL(EditIArchive, EditOArchive, ar);
#else
    VIRTUAL_SERIALIZE_PLACEHOLDER(EditIArchive, EditOArchive, ar);
#endif

#ifdef __BINARY_ARCHIVE_H__
    VIRTUAL_SERIALIZE_VIRTUAL(BinaryIArchive, BinaryOArchive, ar);
#else
    VIRTUAL_SERIALIZE_PLACEHOLDER(BinaryIArchive, BinaryOArchive, ar);
#endif

#ifdef __XPRM_ARCHIVE_H__
    VIRTUAL_SERIALIZE_VIRTUAL(XPrmIArchive, XPrmOArchive, ar);
#else
    VIRTUAL_SERIALIZE_PLACEHOLDER(XPrmIArchive, XPrmOArchive, ar);
#endif
};

struct ShareHandleBaseSerializeVirtual: ShareHandleBase {
    SERIALIZATION_TYPE_NAME_VIRTUAL;

#ifdef __EDIT_ARCHIVE_H__
    VIRTUAL_SERIALIZE_VIRTUAL(EditIArchive, EditOArchive, ar);
#else
    VIRTUAL_SERIALIZE_PLACEHOLDER(EditIArchive, EditOArchive, ar);
#endif

#ifdef __BINARY_ARCHIVE_H__
    VIRTUAL_SERIALIZE_VIRTUAL(BinaryIArchive, BinaryOArchive, ar);
#else
    VIRTUAL_SERIALIZE_PLACEHOLDER(BinaryIArchive, BinaryOArchive, ar);
#endif

#ifdef __XPRM_ARCHIVE_H__
    VIRTUAL_SERIALIZE_VIRTUAL(XPrmIArchive, XPrmOArchive, ar);
#else
    VIRTUAL_SERIALIZE_PLACEHOLDER(XPrmIArchive, XPrmOArchive, ar);
#endif
};


#endif //PERIMETER_SERIALIZATIONVIRTUAL_H
