#include <wx/ClickableShape.h>
#include <cmath>

ClickableShape::ClickableShape( ClickableData* data )
   : m_Data(data)
{
}


ClickableShape::~ClickableShape()
{
   if( m_Data != nullptr )
   {
      delete m_Data;
   }
}

ClickableSemiCircleDraw::ClickableSemiCircleDraw( wxColour fill, wxColour borderPen, unsigned int iTotalPie,
                                                  unsigned int iDrawnPie, unsigned int iThisSlice, ClickableData* data )
   : ClickableShape(data), m_FillColour( fill ), m_BorderColour(borderPen),
   m_TotalPie(iTotalPie), m_AlreadyEatenPie(iDrawnPie), m_DrawingPie(iThisSlice)
{
   m_ellipticAspect = 1.0f;
}

ClickableSemiCircleDraw::~ClickableSemiCircleDraw()
{

}

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

double PointDist( wxPoint pt1, wxPoint pt2 )
{
   double x = pt1.x - pt2.x;
   double y = pt1.y - pt2.y;
   return std::sqrt( x*x + y * y );
}

bool 
ClickableSemiCircleDraw::IsHit( wxPoint& pt )
{
   // Check if it is between the two angles and less than the distance of radius away from the center.
   // arccos( (P122 + P132 - P232) / (2 * P12 * P13) )

   // Distance to point
   double dist = PointDist( m_Center, pt );
   if( dist > m_Radius )
   {
      return false;
   }
   else
   {
      return true;// see todo.
   }

   // TODO; Calculate if point is between angles.
}

void
ClickableSemiCircleDraw::Draw( wxDC &dc, wxRect rc )
{
   m_Rect = rc;
   auto sum = m_TotalPie;
   auto nextSummed = m_DrawingPie;
   auto alreadySummed = m_AlreadyEatenPie;

   // THESE SHOULD ALWAYS BE THE SAME.
   // Draw the slice.
   int radHoriz = (int)(0.8 * wxMin( rc.width, rc.height ));
   int radVert = (int)(radHoriz * m_ellipticAspect);
   m_Radius = radVert;

   wxCoord x0 = rc.x + (rc.width - radHoriz) / 2;
   wxCoord y0 = rc.y + (rc.height - radVert) / 2;

   // Record the center
   m_Center = wxPoint( x0, y0 );

   double part = (alreadySummed / sum);
   double angle1 = 360 * part;
   m_AngleStart = angle1;

   part += nextSummed / sum;
   double angle2 = 360 * part;
   m_AngleEnd = angle2;

   dc.SetPen( m_BorderColour );
   dc.SetBrush( *wxTheBrushList->FindOrCreateBrush( m_FillColour ) );
   dc.DrawEllipticArc( x0, y0, radHoriz, radVert, angle1, angle2 );


   // draw edges
   dc.SetPen( m_BorderColour );
   dc.SetBrush( wxNoBrush );

   wxCoord x1, y1;
   EllipticEdge( x0, y0, radHoriz, radVert, angle1, x1, y1 );
   dc.DrawLine( x0 + radHoriz / 2, y0 + radVert / 2, x1, y1 );

   EllipticEdge( x0, y0, radHoriz, radVert, angle2, x1, y1 );
   dc.DrawLine( x0 + radHoriz / 2, y0 + radVert / 2, x1, y1 );
}

ClickableRectangleDraw::ClickableRectangleDraw( ClickableData* data )
   : ClickableShape(data)
{

}

ClickableRectangleDraw::~ClickableRectangleDraw()
{

}


ClickableGradientAreaDraw::ClickableGradientAreaDraw( wxPen borderPen, wxColour colour1,
                                                      wxColour colour2, wxDirection dir, ClickableData* data )
   : ClickableRectangleDraw(data), GradientAreaDraw(borderPen, colour1, colour2, dir)
{
}

ClickableGradientAreaDraw::~ClickableGradientAreaDraw()
{
}

void 
ClickableGradientAreaDraw::Draw( wxDC &dc, wxRect rc )
{
   GradientAreaDraw::Draw( dc, rc );
   SetHitBox( rc );
}

ClickableFillAreaDraw::ClickableFillAreaDraw( wxPen borderPen, wxBrush fillBrush, ClickableData* data )
   : ClickableRectangleDraw( data ), FillAreaDraw(borderPen, fillBrush)
{
}

ClickableFillAreaDraw::ClickableFillAreaDraw( wxColour borderColour, wxColour fillColour, ClickableData* data )
   : ClickableRectangleDraw( data ), FillAreaDraw( borderColour, fillColour )
{
}

ClickableFillAreaDraw::~ClickableFillAreaDraw()
{

}

void 
ClickableFillAreaDraw::Draw( wxDC &dc, wxRect rc )
{
   FillAreaDraw::Draw( dc, rc );
   SetHitBox( rc );
}

ClickableAreaCollection::ClickableAreaCollection()
{

}

ClickableAreaCollection::~ClickableAreaCollection()
{
   ClickableAreaDrawMap::iterator it;
   for( it = m_areas.begin(); it != m_areas.end(); it++ )
   {
      delete it->second;
   }
}

void 
ClickableAreaCollection::SetAreaDraw( int serie, ClickableShape* areaDraw )
{
   if( m_areas.find( serie ) != m_areas.end() )
   {
      ClickableShape* oldBarArea = m_areas[serie];
      //oldBarArea->RemoveObserver(this);
      delete oldBarArea;
   }

   m_areas[serie] = areaDraw;
}

ClickableShape* 
ClickableAreaCollection::GetAreaDraw( int serie )
{
   if( m_areas.find( serie ) != m_areas.end() )
   {
      return m_areas[serie];
   }
   return NULL;
}

ClickableShape* 
ClickableAreaCollection::GetDataAtPoint( wxPoint& pt )
{
   ClickableAreaDrawMap::iterator it;
   bool bFoundHit = false;
   for( it = m_areas.begin(); it != m_areas.end(); ++it )
   {
      bFoundHit = it->second->IsHit( pt );
      if( bFoundHit )
      {
         return it->second;
      }
   }

   return NULL;
}