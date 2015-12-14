#include "NodePlacer.h"
#include "staticmap.h"
#include "NodeList.h"


void GenerateNodes()
{

}



NodePlacer::NodePlacer()
{

}


void NodePlacer::FillMapsWithNodes()
{
  Rectangle2D StartRec;
  StartRec.PlaceAt(Vector2D(-1700, -1700),Vector2D(1700, 1700));
  RectangleManager(StartRec);
}


void NodePlacer::RectangleManager(Rectangle2D Rec)
{

  if (Rec.GetArea() > MIN_NODE_PLACE_RECTANGLE_SIZE)
  {

    if (StaticMap::GetInstance()->IsInsideBlock(Rec))
    {

      Rectangle2D TopLeft, TopRight, BottomLeft, BottomRight;
      BottomLeft.PlaceAt(Rec.GetBottomLeft(), Rec.GetCentre());
      TopRight.PlaceAt(Rec.GetCentre(), Rec.GetTopRight());
      TopLeft.PlaceAt(BottomLeft.GetTopLeft(), TopRight.GetTopLeft());
      BottomRight.PlaceAt(BottomLeft.GetBottomRight(), TopRight.GetBottomRight());

      RectangleManager(TopLeft);
      RectangleManager(TopRight);
      RectangleManager(BottomLeft);
      RectangleManager(BottomRight);

    }
    else
    {
      // if rectangle is empty place a node in the center of it
      NodeList::GetInstance()->AddNode(Rec.GetCentre());
    }
  }
}

NodePlacer::~NodePlacer()
{

}
