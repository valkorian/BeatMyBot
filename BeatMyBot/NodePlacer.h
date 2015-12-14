#pragma once
#include "Shapes.h"
#include <Windows.h>


class NodePlacer
{
public:
  friend class NodeList;
  NodePlacer();
  static void FillMapsWithNodes();
  static void RectangleManager(Rectangle2D Rec);
private:
  ~NodePlacer();
};

