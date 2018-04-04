#pragma once
#include <wx/plot.h>

class ClickableShape;

class InteractivePlot
{
public:
   InteractivePlot();
   ~InteractivePlot();

   /**
   * Returns a set of data that is drawn at point pt.
   * @param pt Point to check
   * @return data drawn at point or nullptr if none.
   */
   virtual ClickableShape* GetDataAtPoint( wxPoint& pt ) = 0;
};

