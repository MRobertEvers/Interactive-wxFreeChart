#include <wx/pie/PieRenderer.h>


PieRenderer::PieRenderer()
{

};

PieRenderer::~PieRenderer()
{

};

void
PieRenderer::Draw( wxDC &dc, wxRect rc, CategoryDataset *dataset )
{
   // There should be only one serie for pieplot
   //for( size_t n = 0; n < dataset->GetCount(); n++ )
   //{
   //   // The bartype preps the draw-context and the drawing rectangle.
   //   //m_barType->Draw( this, dc, rc, n, dataset );
   //   auto pieDraw = GetAreaDraw( n );
   //   for(  )
   //}
   unsigned int iTotal = 0;
   for( size_t i = 0; i < dataset->GetCount(); i++ )
   {
      iTotal += dataset->GetValue( i, 0 );
   }

   // Need to use dynamic cast to correctly find virtual base class.
   SemiCircleAreaDraw* pieDraw = dynamic_cast<SemiCircleAreaDraw*>(GetAreaDraw( 0 ));


   unsigned int iRunningSum = 0;
   // i is category
   for( size_t i = 0; i < dataset->GetCount(); i++ )
   {
      // Set the color for the shape. Do this here because "dataset" is needed.
      pieDraw->SetFillBrush( dataset->GetBaseRenderer()->GetSerieColour( i ) );
      pieDraw->SetBorderPen( *wxBLACK_PEN );

      SemiCircleAreaType sarea = SemiCircleAreaDraw::GetPieArcValues( rc, iTotal, iRunningSum, dataset->GetValue(i, 0) );

      iRunningSum += dataset->GetValue( i, 0 );
      DrawShape( dc, sarea, 0, i );
   }
}

void 
PieRenderer::DrawShape( wxDC& dc, SemiCircleAreaType& area, size_t serie, size_t category )
{
   SemiCircleAreaDraw* pieDraw = dynamic_cast<SemiCircleAreaDraw*>(GetAreaDraw( 0 ));
   pieDraw->Draw( dc, area );
}

void 
PieRenderer::SetAreaDraw( size_t serie, AreaDraw* area )
{
   m_PieDraws.SetAreaDraw( serie, area );
}

AreaDraw*
PieRenderer::GetAreaDraw( size_t serie )
{
   AreaDraw* barDraw = m_PieDraws.GetAreaDraw( serie );
   if( barDraw == NULL )
   {
      // barDraw = new FillAreaDraw(GetDefaultColour(serie), GetDefaultColour(serie));
      barDraw = new SemiCircleAreaDraw( *wxTRANSPARENT_PEN, GetDefaultColour( serie ) );

      m_PieDraws.SetAreaDraw( serie, barDraw );
   }
   return barDraw;
}
