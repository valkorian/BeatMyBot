#include "Helpers.h"
#include <string.h>
//#include <vector>


// same as memcpy but will not copy data sent to ignstart 
void* memcpy_ignore(void * _Dst, const void* _Src, size_t _Size, const void* _IgnStart, size_t IgnSize)
{
  // nothing to ignore lets just do a normal memcpy 
  if (_IgnStart == nullptr || IgnSize == 0)
  {
    return memcpy(_Dst, _Src, _Size);
  }


  // cast into chars to allow read and write 
  char* cdst = (char*)_Dst;
  char* csrc = (char*)_Src;
  char* cignst = (char*)_IgnStart;

  // size of first copy before ignore
  int cpy1Size = cignst - csrc;
  // size of second copy after ignore 
  int cpy2Size = _Size - (cpy1Size + IgnSize);

  // copy the first part upto the ignore
  memcpy(cdst, csrc, cpy1Size);
  // second copy from after the ignored data 
  memcpy(cdst + (cpy1Size + IgnSize), csrc + (cpy1Size + IgnSize), cpy2Size);

  return _Dst;
}



size_t sizeofvector(size_t SizeOfDataType, size_t SizeOfVector)
{
  return (SizeOfDataType * SizeOfVector);
}





