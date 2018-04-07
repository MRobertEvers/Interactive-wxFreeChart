/////////////////////////////////////////////////////////////////////////////
// Name:    tooltips.cpp
// Purpose: tooltips implementation
// Author:    Moskvichev Andrey V.
// Created:    2010/12/16
// Copyright:    (c) 2010 Moskvichev Andrey V.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/tooltips.h>
#include <wx/chartdc.h>
#include <wx/dcgraph.h>

Tooltip::Tooltip( wxPoint& pt )
   : m_tooltipItemPoint(pt)
{

}

Tooltip::~Tooltip()
{

}

wxPoint
Tooltip::GetDataDrawPoint()
{
   return m_tooltipItemPoint;
}

void 
Tooltip::Blit( wxDC &cdc, wxPoint &rc )
{
   cdc.DrawBitmap( m_backBitmap, rc );
}

TextTooltip::TextTooltip( wxPoint& pt, std::vector<wxString> textList )
   : Tooltip( pt ), m_ToolTipText(textList)
{
   ResizeBackBitmap(wxSize());
   RedrawBackBitmap();
}

TextTooltip::~TextTooltip()
{

}

void 
TextTooltip::Update( wxPoint& pt, std::vector<wxString> textList )
{
   m_ToolTipText = textList;
   m_tooltipItemPoint = ( pt );
}

void 
TextTooltip::RedrawBackBitmap()
{
   wxMemoryDC dc( m_backBitmap );
   wxGCDC mdc( dc );

   dc.SetBackground( *wxTRANSPARENT_BRUSH );
   dc.Clear();

   unsigned int iStartPoint = 0;
   for( auto& text : m_ToolTipText )
   {
      mdc.DrawText( text, wxPoint( 0, iStartPoint ) );
      iStartPoint += mdc.GetTextExtent( text ).GetHeight();
   }

   dc.SelectObject( wxNullBitmap );
}

void
TextTooltip::ResizeBackBitmap( wxSize size )
{
   m_backBitmap.Create( wxSize( 150, 150 ) );
   m_backBitmap.UseAlpha();
}
