/////////////////////////////////////////////////////////////////////////////
// Name:    tooltips.h
// Purpose: tooltips declaration
// Author:    Evers, Matthew R.
// Created:    2018/04/12
// Copyright:    (c) 2018 Evers, Matthew R.
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
   virtual wxSize GetSuggestedTextExtent( wxString& szText, wxDC& dc, wxSize& margins );
   virtual void RedrawBackBitmap();
   virtual void ResizeBackBitmap( wxSize size );

protected:
   std::vector<wxString> m_ToolTipText;
};

#endif /* TOOLTIPS_H_ */
