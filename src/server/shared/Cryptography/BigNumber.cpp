/*
* This file is part of Project SkyFire https://www.projectskyfire.org.
* See LICENSE.md file for Copyright information
*/

#include <ace/Guard_T.h>

#include "Cryptography/BigNumber.h"
#include <algorithm>
#include <openssl/bn.h>
#include <openssl/crypto.h>

BigNumber::BigNumber()
    : _bn(BN_new())
{ }

BigNumber::BigNumber(BigNumber const& bn)
    : _bn(BN_dup(bn.BN()))
{
}

BigNumber::~BigNumber()
{
    BN_free(_bn);
}

void BigNumber::SetDword(int32 val)
{
    SetDword(uint32(abs(val)));
    if (val < 0)
        BN_set_negative(_bn, 1);
}

void BigNumber::SetDword(uint32 val)
{
    BN_set_word(_bn, val);
}

void BigNumber::SetQword(uint64 val)
{
    BN_set_word(_bn, (uint32)(val >> 32));
    BN_lshift(_bn, _bn, 32);
    BN_add_word(_bn, (uint32)(val & 0xFFFFFFFF));
}

void BigNumber::SetBinary(uint8 const* bytes, int32 len, bool littleEndian)
{
    if (littleEndian)
    {
#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER < 0x10100000L
        uint8* array = new uint8[len];

        for (int i = 0; i < len; i++)
            array[i] = bytes[len - 1 - i];

        BN_bin2bn(array, len, _bn);

        delete[] array;
#else
        BN_lebin2bn(bytes, len, _bn);
#endif
    }
    else
        BN_bin2bn(bytes, len, _bn);
}

void BigNumber::SetHexStr(char const* str)
{
    BN_hex2bn(&_bn, str);
}

void BigNumber::SetRand(int32 numbits)
{
    BN_rand(_bn, numbits, 0, 1);
}

BigNumber& BigNumber::operator=(BigNumber const& bn)
{
    if (this == &bn)
        return *this;

    BN_copy(_bn, bn._bn);
    return *this;
}

BigNumber BigNumber::operator+=(BigNumber const& bn)
{
    BN_add(_bn, _bn, bn._bn);
    return *this;
}

BigNumber BigNumber::operator-=(BigNumber const& bn)
{
    BN_sub(_bn, _bn, bn._bn);
    return *this;
}

BigNumber BigNumber::operator*=(BigNumber const& bn)
{
    BN_CTX* bnctx;

    bnctx = BN_CTX_new();
    BN_mul(_bn, _bn, bn._bn, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

BigNumber BigNumber::operator/=(BigNumber const& bn)
{
    BN_CTX* bnctx;

    bnctx = BN_CTX_new();
    BN_div(_bn, NULL, _bn, bn._bn, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

BigNumber BigNumber::operator%=(BigNumber const& bn)
{
    BN_CTX* bnctx;

    bnctx = BN_CTX_new();
    BN_mod(_bn, _bn, bn._bn, bnctx);
    BN_CTX_free(bnctx);

    return *this;
}

BigNumber BigNumber::Exp(BigNumber const& bn) const
{
    BigNumber ret;
    BN_CTX* bnctx;

    bnctx = BN_CTX_new();
    BN_exp(ret._bn, _bn, bn._bn, bnctx);
    BN_CTX_free(bnctx);

    return ret;
}

BigNumber BigNumber::ModExp(BigNumber const& bn1, BigNumber const& bn2) const
{
    BigNumber ret;
    BN_CTX* bnctx;

    bnctx = BN_CTX_new();
    BN_mod_exp(ret._bn, _bn, bn1._bn, bn2._bn, bnctx);
    BN_CTX_free(bnctx);

    return ret;
}

int32 BigNumber::GetNumBytes(void) const
{
    return BN_num_bytes(_bn);
}

uint32 BigNumber::AsDword()
{
    return (uint32)BN_get_word(_bn);
}

bool BigNumber::isZero() const
{
    return BN_is_zero(_bn);
}


void BigNumber::GetBytes(uint8* buf, size_t bufsize, bool littleEndian) const
{
#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER < 0x10100000L
    int nBytes = GetNumBytes();
    ASSERT(nBytes >= 0);
    std::size_t numBytes = static_cast<std::size_t>(nBytes);

    // too large to store
    ASSERT(numBytes <= bufsize);

    // If we need more bytes than length of BigNumber set the rest to 0
    if (numBytes < bufsize)
        memset((void*)buf, 0, bufsize);

    BN_bn2bin(_bn, buf + (bufsize - numBytes));

    // openssl's BN stores data internally in big endian format, reverse if little endian desired
    if (littleEndian)
        std::reverse(buf, buf + bufsize);
#else
    int res = littleEndian ? BN_bn2lebinpad(_bn, buf, bufsize) : BN_bn2binpad(_bn, buf, bufsize);
    ASSERT(res > 0);
#endif
}

std::vector<uint8> BigNumber::ToByteVector(int32 minSize, bool littleEndian) const
{
    std::size_t length = GetNumBytes() > minSize ? GetNumBytes() : minSize;
    std::vector<uint8> v;
    v.resize(length);
    GetBytes(v.data(), length, littleEndian);
    return v;
}

char* BigNumber::AsHexStr() const
{
    return BN_bn2hex(_bn);
}

char* BigNumber::AsDecStr() const
{
    return BN_bn2dec(_bn);
}

