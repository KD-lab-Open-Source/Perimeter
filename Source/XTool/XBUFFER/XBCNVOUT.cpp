#include <cstdlib>
#include <cstring>
#include <cstdint>
#include "xutl.h"
#include "xbuffer.h"
#include "xerrhand.h"

//ZLIB compression
namespace zlib {
#define ZLIB_CONST
#include <zlib.h>
};

XBuffer& XBuffer::operator>= (char& var)
{
    char* p = buf + offset;
    var = (char)strtol(p,&p,0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (int8_t& var)
{
    char* p = buf + offset;
    var = (int8_t)strtol(p,&p,0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (uint8_t& var)
{
	char* p = buf + offset;
	var = (uint8_t)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int16_t& var)
{
	char* p = buf + offset;
	var = (int16_t)strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (uint16_t& var)
{
	char* p = buf + offset;
	var = (uint16_t)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int32_t& var)
{
	char* p = buf + offset;
	var = (int32_t)strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (uint32_t& var)
{
	char* p = buf + offset;
	var = (uint32_t)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int64_t& var)
{
	char* p = buf + offset;
	var = strtoll(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (uint64_t& var)
{
	char* p = buf + offset;
	var = strtoull(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (double& var)
{
	char* p = buf + offset;
	var = string_to_double(p,&p);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (float& var)
{
	char* p = buf + offset;
	var = string_to_float(p,&p);
	offset += p - (buf + offset);
	return *this;
}

int XBuffer::compress(XBuffer& output, uint32_t* len) const {
    uint32_t original_len = tell();
    
    //Ensure destination can handle compressed data + 2 lens
    while (output.tell() + original_len + 512 + sizeof(uint32_t) * 2 > output.size) {
        output.handleOutOfSize();
    }
    
    //Store the original len
    output < original_len;

    //Do the compression
    size_t position = output.tell() + sizeof(uint32_t);
    zlib::uLongf compressed_len_out = output.size - position;
    const char* src = buf;
    char* dst = output.buf + position;
    int ret = zlib::compress(
            reinterpret_cast<zlib::Bytef*>(dst),
            &compressed_len_out,
            reinterpret_cast<const zlib::Bytef*>(src),
            original_len
    );
    if (ret != Z_OK) {
        xassert(0);
        fprintf(stderr, "XBuffer compression faild len %d ret %d\n", original_len, ret);
        return -1;
    }
    
    //Write the compressed len and advance cursors
    uint32_t compressed_len = static_cast<uint32_t>(compressed_len_out);
    if (len) {
        *len = compressed_len;
    }
    output < compressed_len;
    output.set(compressed_len, XB_CUR);

    return 0;
}

int XBuffer::uncompress(XBuffer& output, uint32_t* len) {
    //Get uncompressed and compressed sizes
    uint32_t original_len;
    uint32_t compressed_len;
    read(original_len);
    read(compressed_len);

    //Check if this buffer has enough data
    if (tell() + compressed_len > size) {
        fprintf(stderr, "XBuffer decompression incomplete data len %d %d %zu\n", original_len, compressed_len, size);
        return -1;
    }
    
    //Ensure destination can handle uncompressed data
    while (output.tell() + original_len > output.size) {
        if (output.automatic_realloc) {
            output.realloc(output.tell() + original_len);
        } else {
            //This will error
            output.handleOutOfSize();
        }
    }

    //Do the decompression
    zlib::uLongf uncompressed_len_out = output.size - output.tell();
    const char* src = buf + tell();
    char* dst = output.buf + output.tell();
    int ret = zlib::uncompress(
            reinterpret_cast<zlib::Bytef*>(dst),
            &uncompressed_len_out,
            reinterpret_cast<const zlib::Bytef*>(src),
            compressed_len
    );
    if (ret != Z_OK) {
        xassert(0);
        fprintf(stderr, "XBuffer decompression faild len %d ret %d\n", original_len, ret);
        return -2;
    }
    if (uncompressed_len_out != original_len) {
        fprintf(stderr, "XBuffer decompression len mismatch %lu %d\n", uncompressed_len_out, original_len);
        return -3;
    }
    
    //Set the len and advance cursors
    if (len) {
        *len = original_len;
    }
    output.set(original_len, XB_CUR);
    set(compressed_len, XB_CUR);
    
    return 0;
}


