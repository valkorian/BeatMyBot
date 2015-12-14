#pragma once

// same as memcpy but will not copy data sent to ignstart 
void* memcpy_ignore(void * _Dst, const void* _Src, size_t _Size, const void* _IgnStart = nullptr, size_t IgnSize = 0);

// returns the size of the vectors data in bytes
size_t sizeofvector(size_t SizeOfDataType, size_t SizeOfVector);

