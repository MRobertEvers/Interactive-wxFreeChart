/////////////////////////////////////////////////////////////////////////////
// Name:    areadraw.cpp
// Purpose: area draw classes implementation
// Author:    Moskvichev Andrey V.
// Created:    2008/11/07
// Copyright:    (c) 2008-2010 Moskvichev Andrey V.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/math.h>
#include <wx/wxprec.h>

/*
* TODO Initial quick and dirty. Must be rewritten.
*/

static void Rotate( wxCoord &x, wxCoord &y, wxCoord xc, wxCoord yc, double rad, double angle )
{
   x = (wxCoord)(rad * cos( angle ) + xc);
   y = (wxCoord)(-rad * sin( angle ) + yc);
}

static void EllipticEdge( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double angle, wxCoord &outX, wxCoord &outY )
{
   double degs = angle * M_PI / 180;

   double w = width;
   double h = height;

   outX = (wxCoord)(w * cos( degs ) / 2 + x + w / 2);
   outY = (wxCoord)(-h * sin( degs ) / 2 + y + h / 2);
}

#include <wx/areadraw.h>

AreaDraw::AreaDraw()
{
}

AreaDraw::~AreaDraw()
{
}

NoAreaDraw::NoAreaDraw()
{
}

NoAreaDraw::~NoAreaDraw()
{
}

void NoAreaDraw::Draw(wxDC &WXUNUSED(dc), wxRect WXUNUSED(rc))
{
    // do nothing
}


FillAreaDraw::FillAreaDraw(wxPen borderPen, wxBrush fillBrush)
{
    m_borderPen = borderPen;
    m_fillBrush = fillBrush;
}

FillAreaDraw::FillAreaDraw(wxColour borderColour, wxColour fillColour)
{
    m_borderPen = *wxThePenList->FindOrCreatePen(borderColour, 1, wxPENSTYLE_SOLID);
    m_fillBrush = *wxTheBrushList->FindOrCreateBrush(fillColour, wxBRUSHSTYLE_SOLID);
}

FillAreaDraw::~FillAreaDraw()
{
}

void FillAreaDraw::Draw(wxDC &dc, wxRect rc)
{
    dc.SetPen(m_borderPen);
    dc.SetBrush(m_fillBrush);
    dc.DrawRectangle(rc);
}

SemiCircleAreaDraw::SemiCircleAreaDraw( wxPen borderPen, wxBrush fillBrush )
   : FillAreaDraw(borderPen, fillBrush)
{

}

SemiCircleAreaDraw::SemiCircleAreaDraw( wxColour borderColour, wxColour fillColour )
   : FillAreaDraw(borderColour, fillColour)
{

}
 
SemiCircleAreaDraw::~SemiCircleAreaDraw()
{

}

void 
SemiCircleAreaDraw::Draw( wxDC &dc, SemiCircleAreaType& areaTypeData)//wxPoint& center, double arcStart, double arcEnd, double radius )
{
   wxPoint& upperLeft = areaTypeData.m_UpperLeftPoint;
   double arcStart = areaTypeData.m_ArcStart; 
   double arcLength = areaTypeData.m_ArcLength;
   double diameter = areaTypeData.m_Diameter;

   // THESE SHOULD ALWAYS BE THE SAME.
   // Draw the slice.
   int radHoriz = diameter;
   int radVert = (int)(radHoriz);

   // These specify upper left corner of the rectangle that contains the pie chart.
   wxCoord x0 = upperLeft.x;
   wxCoord y0 = upperLeft.y;

   // Round the start and the upper angle down.
   double angle1 = std::floor(arcStart);
   // Add one degree, then round down this removes breaks between the drawn areas.
   double angle2 = std::ceil( arcStart + arcLength + 3.14/180.0 );

   dc.SetPen( m_borderPen );
   dc.SetBrush( m_fillBrush );
   dc.DrawEllipticArc( x0, y0, radHoriz, radVert, angle1, angle2 );

   // draw edges
   dc.SetPen( m_borderPen );
   dc.SetBrush( wxNoBrush );

   wxCoord x1, y1;
   EllipticEdge( x0, y0, radHoriz, radVert, angle1, x1, y1 );
   dc.DrawLine( x0 + radHoriz / 2, y0 + radVert / 2, x1, y1 );

   EllipticEdge( x0, y0, radHoriz, radVert, angle2, x1, y1 );
   dc.DrawLine( x0 + radHoriz / 2, y0 + radVert / 2, x1, y1 );
}

void
SemiCircleAreaDraw::GetPieArcValues( wxRect& rc, unsigned int iTotalPie, unsigned int iDrawnPie, unsigned int iThisSlice,
                                     wxPoint& ptUpperLeft, unsigned int& arcStart, unsigned int& arcEnd, unsigned int& radies )
{
   radies = (int)(0.8 * wxMin( rc.width, rc.height ));
   ptUpperLeft = wxPoint( rc.x + (rc.width - radies) / 2, rc.y + (rc.height - radies) / 2 );
   arcStart = (360 * iDrawnPie) / iTotalPie;
   arcEnd = (360 * iThisSlice) / iTotalPie;
}

SemiCircleAreaType
SemiCircleAreaDraw::GetPieArcValues( wxRect& rc, unsigned int iTotalPie,
                                     unsigned int iDrawnPie, unsigned int iThisSlice )
{
   wxPoint pt;
   unsigned int arcstart, arclength, radius;
   GetPieArcValues( rc, iTotalPie, iDrawnPie, iThisSlice, pt, arcstart, arclength, radius );
   
   return SemiCircleAreaType( pt, arcstart, arclength, radius );
}

GradientAreaDraw::GradientAreaDraw(wxPen borderPen, wxColour colour1, wxColour colour2, wxDirection dir)
{
    m_borderPen = borderPen;
    m_colour1 = colour1;
    m_colour2 = colour2;
    m_dir = dir;
}

GradientAreaDraw::~GradientAreaDraw()
{
}

void GradientAreaDraw::Draw(wxDC &dc, wxRect rc)
{
    if (m_dir == wxALL)
        dc.GradientFillConcentric(rc, m_colour1, m_colour2);
    else
        dc.GradientFillLinear(rc, m_colour1, m_colour2, m_dir);

    dc.SetPen(m_borderPen);
    dc.SetBrush(wxNoBrush);
    dc.DrawRectangle(rc);
}

AreaDrawCollection::AreaDrawCollection()
{
}

AreaDrawCollection::~AreaDrawCollection()
{
    AreaDrawMap::iterator it;
    for (it = m_areas.begin(); it != m_areas.end(); it++) {
        delete it->second;
    }
}

void AreaDrawCollection::SetAreaDraw(int serie, AreaDraw *barArea)
{
    if (m_areas.find(serie) != m_areas.end()) {
        AreaDraw *oldBarArea = m_areas[serie];
        //oldBarArea->RemoveObserver(this);
        delete oldBarArea;
    }

    m_areas[serie] = barArea;
    //FireNeedRedraw();
}

AreaDraw *AreaDrawCollection::GetAreaDraw(int serie)
{
    if (m_areas.find(serie) != m_areas.end()) {
        return m_areas[serie];
    }
    return NULL;
}
