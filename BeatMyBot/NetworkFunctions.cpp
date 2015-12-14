#include "NetworkFunctions.h"
#include "Network.h"
#ifdef SMARTNETWORK
void NetworkFunctions::OnSpawnedObject(const NetworkFunctions* Object) const
{
  Network::GetInstance()->GetRepDataFrom(Object);
}

void NetworkFunctions::OnRemovedObject(const NetworkFunctions* Object) const
{
  Network::GetInstance()->OnObjectRemoved(Object);
}

void NetworkFunctions::GetReplicatedValues(std::vector<ReplicatedData>* TheVector, int* DataRepByteSize) const
{
  /*EMPTY*/
}
#endif