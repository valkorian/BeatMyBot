#pragma once
#include <vector>
#define NETWORKFUNCTIONS
typedef char NetworkDataType;

// smart will try to update less and shorten the packet size
//#define SMARTNETWORK

#ifdef SMARTNETWORK

#define GENERATE_NETWORK_BODY() \
protected: \
  virtual void GetReplicatedValues(std::vector<ReplicatedData>* TheVector, int* DataRepByteSize) const override;\


struct ReplicatedData
{
  // owner of the data
  const void* Owner;

  // pointer to the data
  const void* Data;
  // the size of the data
  size_t   Size;

  ReplicatedData(const void * TheData, const size_t   TheSize , const void* TheOwner)
    :Data(TheData), Size(TheSize), Owner(TheOwner)
  {
    /*Empty*/
  }

  ReplicatedData& operator = (ReplicatedData& Other)
  {
    this->Data = Other.Data;
    this->Owner = Other.Owner;
    this->Size = Other.Size;
    return *this;
  }
};




#define RepData(Owner, Data) \
  {\
    ReplicatedData RepDat(&Data,sizeof(Data), Owner); \
    *DataRepByteSize += sizeof(Data);\
    TheVector->push_back(RepDat);\
  }

class NetworkFunctions
{
public:
  friend class Network;
protected:
  virtual void GetReplicatedValues(std::vector<ReplicatedData>* TheVector, int* DataRepByteSize) const;
  void OnSpawnedObject(const NetworkFunctions* Object) const ;
  void OnRemovedObject(const NetworkFunctions* Object) const;
};

#endif