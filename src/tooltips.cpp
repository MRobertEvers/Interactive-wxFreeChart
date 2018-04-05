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

Tooltip::Tooltip()
{

}

Tooltip::~Tooltip()
{

}

void 
Tooltip::Blit( wxDC &cdc, wxPoint &rc )
{
   cdc.DrawBitmap( m_backBitmap, rc );
}

TextTooltip::TextTooltip( std::vector<wxString> textList )
   : m_ToolTipText(textList)
{
   ResizeBackBitmap(wxSize());
   RedrawBackBitmap();
}

TextTooltip::~TextTooltip()
{

}

void 
TextTooltip::RedrawBackBitmap()
{
   wxMemoryDC mdc;
   mdc.SelectObject( m_backBitmap );
   unsigned int iStartPoint = 0;
   for( auto& text : m_ToolTipText )
   {
      mdc.DrawText( text, wxPoint( 0, iStartPoint ) );
      iStartPoint += mdc.GetTextExtent( text ).GetHeight();
   }

   mdc.SelectObject( wxNullBitmap );
}

void
TextTooltip::ResizeBackBitmap( wxSize size )
{
   m_backBitmap.Create( wxSize( 150, 150 ) );
}
