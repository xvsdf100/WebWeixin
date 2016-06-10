#include "ByteStream.h"
#include <assert.h>
#include <memory.h>


ByteStream::ByteStream()
{
    m_Buf = 0;
    m_Size = 0;
    m_Pos = 0;
}

ByteStream::ByteStream( char* buf,uint32_t len )
{
    m_Buf = buf;
    m_Size = len;
    m_Pos = 0;
    assert(0 != m_Buf);
}

ByteStream::~ByteStream()
{

}


void ByteStream::SetBuffer( char* buf,uint32_t len )
{
    m_Buf = buf;
    m_Size = len;
    m_Pos = 0;
    assert(0 != m_Buf);
}

bool ByteStream::Read( uint8_t& value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(&value,m_Buf+m_Pos,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Read( uint16_t& value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(&value,m_Buf+m_Pos,size);
        m_Pos += size;
    }

    return bRet;
}



bool ByteStream::Read( uint32_t& value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(&value,m_Buf+m_Pos,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Read( uint64_t& value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(&value,m_Buf+m_Pos,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Read(float& value)
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(&value,m_Buf+m_Pos,size);
        m_Pos += size;
    }
    return bRet;
}

bool ByteStream::Read( double& value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(&value,m_Buf+m_Pos,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Read( void* value,uint32_t size )
{
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(value,m_Buf+m_Pos,size);
        m_Pos += size;
    }

    return bRet;
}


bool ByteStream::Write( uint8_t value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(m_Buf+m_Pos,&value,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Write( uint16_t value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(m_Buf+m_Pos,&value,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Write( uint32_t value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(m_Buf+m_Pos,&value,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Write( uint64_t value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(m_Buf+m_Pos,&value,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Write( float value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(m_Buf+m_Pos,&value,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Write( double value )
{
    uint32_t size = sizeof(value);
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(m_Buf+m_Pos,&value,size);
        m_Pos += size;
    }

    return bRet;
}

bool ByteStream::Write( void* value,uint32_t size )
{
    bool bRet = IsCan(size);
    if(bRet)
    {
        memcpy(m_Buf+m_Pos,value,size);
        m_Pos += size;
    }

    return bRet;
}

char* ByteStream::GetBuffer()
{
    return m_Buf;
}

bool ByteStream::IsCan( uint32_t size )
{
    uint32_t NewPos = m_Pos + size;
    return (NewPos <= m_Size);
}

uint32_t ByteStream::GetSize()
{
    return m_Size;    
}

bool ByteStream::Move( uint32_t pos )
{
    bool bRet = false;
   if(IsCan(pos))
   {
       m_Pos += pos;
       bRet = true;
   }
   return bRet;
}

char* ByteStream::GetPosBuffer()
{
    return (m_Buf + m_Pos);
}

uint32_t ByteStream::GetRemainSize()
{
    return m_Size - m_Pos;
}
