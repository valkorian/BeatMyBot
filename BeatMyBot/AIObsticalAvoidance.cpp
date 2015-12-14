#include "AIObsticalAvoidance.h"
#include "Shapes.h"
#include "staticmap.h"
#include "rules.h"
#include "Renderer.h"
#include "mydrawengine.h"

#define CollisionCircleSize 70.0f
#define CollisionCircleOffSet 50.0f

AIObsticalAvoidance::AIObsticalAvoidance()
{
  
}

Vector2D AIObsticalAvoidance::WallAvoid(const Vector2D &CurrentPosition, const Vector2D &CurrentVelo)
{
  Circle2D SafetyCircle;
  Vector2D VSafetyCircleOffSet = CurrentVelo.unitVector() * CollisionCircleOffSet;
  SafetyCircle.PlaceAt(CurrentPosition + VSafetyCircleOffSet, CollisionCircleSize);
  //MyDrawEngine::GetInstance()->FillCircle(CurrentPosition + VSafetyCircleOffSet, CollisionCircleSize, 600);
  StaticMap* pStaticMap = StaticMap::GetInstance();
  // is our safety circle inside a wall
  return pStaticMap->IsInsideBlock(SafetyCircle) ? (pStaticMap->GetNormalToSurface(SafetyCircle)) /*Circle is inside block get the normal to it*/
    : Vector2D(0, 0); /*Circle did not hit a block return zero vector*/
}





AIObsticalAvoidance::~AIObsticalAvoidance()
{
}
