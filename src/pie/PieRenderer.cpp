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

   auto pieDraw = GetAreaDraw( 0 );
   unsigned int iRunningSum = 0;
   // i is category
   for( size_t i = 0; i < dataset->GetCount(); i++ )
   {
      unsigned int arcStart, arcLength, radius;
      wxPoint left;

      SemiCircleAreaType sarea = pieDraw->GetPieArcValues( rc, iTotal, iRunningSum, dataset->GetValue(i, 0) );
      pieDraw->SetFillBrush( dataset->GetBaseRenderer()->GetSerieColour( i ) );
      iRunningSum += dataset->GetValue( i, 0 );

      pieDraw->Draw( dc, sarea );
   }
}

SemiCircleAreaDraw*
PieRenderer::GetAreaDraw( size_t serie )
{
   SemiCircleAreaDraw* barDraw = (SemiCircleAreaDraw*)m_PieDraws.GetAreaDraw( serie );
   if( barDraw == NULL )
   {
      // barDraw = new FillAreaDraw(GetDefaultColour(serie), GetDefaultColour(serie));
      barDraw = new SemiCircleAreaDraw( *wxTRANSPARENT_PEN, GetDefaultColour( serie ) );

      m_PieDraws.SetAreaDraw( serie, barDraw );
   }
   return barDraw;
}
