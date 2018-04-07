/////////////////////////////////////////////////////////////////////////////
// Name:    tooltips.h
// Purpose: tooltips declaration
// Author:    Moskvichev Andrey V.
// Created:    2010/12/16
// Copyright:    (c) 2010 Moskvichev Andrey V.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef TOOLTIPS_H_
#define TOOLTIPS_H_

#include <wx/wxfreechartdefs.h>

// TODO use wxList of strings
#include <vector> 

class ChartDC;

// Usually drawn by the chart object.
class WXDLLIMPEXP_FREECHART Tooltip
{
public:
   Tooltip( wxPoint& pt );
   virtual ~Tooltip();

   virtual void RedrawBackBitmap() = 0;
   virtual void ResizeBackBitmap( wxSize size ) = 0;
   virtual wxPoint GetDataDrawPoint();
   virtual void Blit( wxDC &cdc, wxPoint &rc );

protected:
   wxBitmap m_backBitmap;
   wxPoint m_tooltipItemPoint;
};

class WXDLLIMPEXP_FREECHART TextTooltip : public Tooltip
{
public:
   TextTooltip( wxPoint& pt, std::vector<wxString> textList);
   virtual ~TextTooltip();

   virtual void Update( wxPoint& pt, std::vector<wxString> textList );
   virtual wxSize GetSuggestedTextExtent( wxString& szText, wxDC& dc, wxSize& margins = wxSize(3, 1));
   virtual void RedrawBackBitmap();
   virtual void ResizeBackBitmap( wxSize size );

protected:
   std::vector<wxString> m_ToolTipText;
};

#endif /* TOOLTIPS_H_ */
