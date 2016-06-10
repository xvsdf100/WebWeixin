#ifndef BYTESTREAM_H_2016_3_3
#define BYTESTREAM_H_2016_3_3
#include <stdint.h>

class ByteStream
{
public:
    ByteStream();
    ByteStream(char* buf,uint32_t len);
    ~ByteStream();

public:
    void SetBuffer(char* buf,uint32_t len);

public:
    bool Read(uint8_t& value);
    bool Read(uint16_t& value);
    bool Read(uint32_t& value);
    bool Read(uint64_t& value);
    bool Read(float& value);
    bool Read(double& value);
    bool Read(void* value,uint32_t size);
    bool Write(uint8_t value);
    bool Write(uint16_t value);
    bool Write(uint32_t value);
    bool Write(uint64_t value);
    bool Write(float value);
    bool Write(double value);
    bool Write(void* value,uint32_t size);
    bool Move(uint32_t pos);

    char* GetBuffer();          //获取m_buffer
    char* GetPosBuffer();       //获取当前位置bufeer. m_Buf + m_Pos
    uint32_t GetSize();
    uint32_t GetRemainSize();   //获取剩下的大小

private:
    bool IsCan(uint32_t size);

private:
    char*       m_Buf;
    uint32_t    m_Size;
    uint32_t    m_Pos;
};

#endif