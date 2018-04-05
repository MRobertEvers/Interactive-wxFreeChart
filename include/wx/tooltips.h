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

class WXDLLIMPEXP_FREECHART Tooltip
{
public:
   Tooltip();
   virtual ~Tooltip();

   virtual void RedrawBackBitmap() = 0;
   virtual void ResizeBackBitmap( wxSize size ) = 0;
   virtual void Blit( wxDC &cdc, wxPoint &rc );

protected:
   wxBitmap m_backBitmap;
};

class WXDLLIMPEXP_FREECHART TextTooltip : public Tooltip
{
public:
   TextTooltip(std::vector<wxString> textList);
   virtual ~TextTooltip();

   virtual void RedrawBackBitmap();
   virtual void ResizeBackBitmap( wxSize size );

protected:
   std::vector<wxString> m_ToolTipText;
};

#endif /* TOOLTIPS_H_ */
