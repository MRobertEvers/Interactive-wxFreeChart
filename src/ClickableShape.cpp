#include <wx/ClickableShape.h>
#include <cmath>


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

ClickableSemiCircleDraw::ClickableSemiCircleDraw( wxColour fill, wxColour borderPen, unsigned int iTotalPie,
                                                  unsigned int iDrawnPie, unsigned int iThisSlice, ClickableData* data )
   : MultiClickableShape(data), m_FillColour( fill ), m_BorderColour(borderPen),
   m_TotalPie(iTotalPie), m_AlreadyEatenPie(iDrawnPie), m_DrawingPie(iThisSlice)
{
   m_ellipticAspect = 1.0f;
}

ClickableSemiCircleDraw::~ClickableSemiCircleDraw()
{

}

bool 
ClickableSemiCircleDraw::IsHit( wxPoint& pt )
{
   // Check if it is between the two angles and less than the distance of radius away from the center.
   // arccos( (P122 + P132 - P232) / (2 * P12 * P13) )

   // Distance to point
   //double dist = PointDist( m_Center, pt );
   //if( dist > m_Radius )
   //{
   //   return false;
   //}
   //else
   {
      return false;// see todo.
   }

   // TODO; Calculate if point is between angles.
}

void
ClickableSemiCircleDraw::Draw( wxDC &dc, wxRect rc )
{

}

void
ClickableSemiCircleDraw::Draw( wxDC &dc, wxRect rc, size_t serie, size_t category )
{
   Draw( dc, rc );
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