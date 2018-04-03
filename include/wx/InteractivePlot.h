#pragma once
#include <wx/plot.h>

class Dataset;

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
   virtual Dataset* GetDataAtPoint( wxPoint& pt ) = 0;
};

