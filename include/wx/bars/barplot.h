/////////////////////////////////////////////////////////////////////////////
// Name:    barplot.h
// Purpose: bar plot declaration
// Author:    Moskvichev Andrey V.
// Created:    2008/11/07
// Copyright:    (c) 2008-2010 Moskvichev Andrey V.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef BARPLOT_H_
#define BARPLOT_H_

//#include <wx/axisplot.h>
#include <wx/xy/xyplot.h>
#include <wx/InteractivePlot.h>

#include <wx/category/categorydataset.h>

#include <vector>

class ClickableShape;

/**
 * Plot for displaying histograms with values at one axis and
 * category names at other.
 *
 * Virtual inheritance is necessary because both XYPlot and InteractivePlot inherit from Plot.
 */
class WXDLLIMPEXP_FREECHART BarPlot : public XYPlot, public InteractivePlot
{
public:
    BarPlot();
    virtual ~BarPlot();

    //
    // InteractivePlot
    //
    virtual Dataset* GetDataAtPoint( wxPoint& pt );

protected:
    virtual bool AcceptAxis(Axis *axis);

    virtual bool AcceptDataset(Dataset *dataset);

    virtual void DrawDatasets(wxDC &dc, wxRect rc);

private:

};

#endif /*BARPLOT_H_*/
