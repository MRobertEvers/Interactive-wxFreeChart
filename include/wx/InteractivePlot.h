/////////////////////////////////////////////////////////////////////////////
// Name:    InteractivePlot.h
// Purpose: Provide interface for finding data at a drawn point on a plot.
// Author:    Evers, Matthew R.
// Created:    2018/04/12
// Copyright:    (c) 2018 Evers, Matthew R.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

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

