#include <wx/ClickableShape.h>
#include <cmath>

static double PointDist( wxPoint pt1, wxPoint pt2 )
{
   double x = pt1.x - pt2.x;
   double y = pt1.y - pt2.y;
   return std::sqrt( x*x + y * y );
}

bool 
SemiCircleHitBox::IsHit( wxPoint& pt )
{
   if( PointDist( pt, m_Area.GetCenter() ) > m_Area.m_Diameter )
   {
      return false;
   }
   else
   {
      auto center = m_Area.GetCenter();
      auto vecToPt = pt - center;
      auto vecToX = wxPoint( m_Area.m_Diameter, 0 );
      // Correct the orientation
      vecToPt.y *= -1;

      //dot = x1 * x2 + y1 * y2      // dot product between[x1, y1] and [x2, y2]
      //   det = x1 * y2 - y1 * x2      // determinant
      //   angle = atan2( det, dot )  // atan2( y, x ) or atan2( sin, cos )
      auto dot = vecToX.x * vecToPt.x + vecToX.y * vecToPt.y;
      auto det = vecToX.x * vecToPt.y - vecToX.y * vecToPt.x;
      auto angle = std::atan2( det, dot ) * 180 / M_PI;

      if( angle < 0 )
      {
         angle = 360 + angle;
      }

      return m_Area.m_ArcStart < angle && angle < (m_Area.m_ArcLength + m_Area.m_ArcStart);
   }
}

ClickableCategoryData::ClickableCategoryData( CategoryDataset* ptData )
   : ClickableData( ptData )
{
   // With a category dataset, you give it a list of categories. (These are the "Horizontal" groups)
   // Series' have names and they contain a data point for each category.
}

ClickableCategoryData::~ClickableCategoryData()
{

}

wxString 
ClickableCategoryData::GetSeriesName()
{
   auto dataset = GetDataset();
   return dataset->GetSerieName( m_ClickedSeries );
}

wxString 
ClickableCategoryData::GetCategoryName()
{
   // This would represent a bar in a bar chart.
   auto dataset = GetDataset();
   return dataset->GetName( m_ClickedCategory );
}

double
ClickableCategoryData::GetSeriesTotal()
{
   double iSumRetval = 0;
   auto dataset = GetDataset();
   for( size_t category = 0; category < dataset->GetCount(); category++ )
   {
      iSumRetval += dataset->GetValue( category, m_ClickedSeries );
   }

   return iSumRetval;
}

double
ClickableCategoryData::GetCategoryTotalOfAllSeries()
{
   double iSumRetval = 0;
   auto dataset = GetDataset();
   for( size_t series = 0; series < dataset->GetSerieCount(); series++ )
   {
      iSumRetval += dataset->GetValue( m_ClickedCategory, series );
   }

   return iSumRetval;
}

double
ClickableCategoryData::GetSeriesValue(  )
{
   auto dataset = GetDataset();
   return dataset->GetValue( m_ClickedCategory, m_ClickedSeries );
}


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

ClickableSemiCircleDraw::ClickableSemiCircleDraw( wxColour fill, wxColour borderPen, ClickableData* data )
   : MultiClickableShape(data), SemiCircleAreaDraw(borderPen, fill)
{
}

ClickableSemiCircleDraw::~ClickableSemiCircleDraw()
{

}

void
ClickableSemiCircleDraw::Draw( wxDC &dc, wxRect rc )
{

}

void
ClickableSemiCircleDraw::Draw( wxDC &dc, wxRect rc, size_t serie, size_t category )
{
}

void
ClickableSemiCircleDraw::Draw( wxDC & dc, SemiCircleAreaType & rc, size_t serie, size_t category )
{
   SemiCircleAreaDraw::Draw( dc, rc );
   AddHitBox( new SemiCircleHitBox(rc), serie, category );
}

ClickableRectangleDraw::ClickableRectangleDraw( ClickableData* data )
   : MultiClickableShape(data)
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

void 
ClickableAreaCollection::ClearDrawnHitBoxes()
{
   ClickableAreaDrawMap::iterator it;
   for( it = m_areas.begin(); it != m_areas.end(); it++ )
   {
      it->second->InitDraw();
   }
}

ClickableShape* 
ClickableAreaCollection::GetDataAtPoint( wxPoint& pt )
{
   ClickableAreaDrawMap::iterator it;

   // Assumes that the areas were drawn in order.
   ClickableShape* ptFoundShape = nullptr;

   bool bFoundHit = false;
   for( it = m_areas.begin(); it != m_areas.end(); ++it )
   {
      bFoundHit = it->second->IsHit( pt );
      if( bFoundHit )
      {
         ptFoundShape = it->second;
      }
   }

   return ptFoundShape;
}