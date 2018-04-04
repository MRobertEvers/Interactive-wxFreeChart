#include <wx/ClickableRenderer.h>
#include <wx/category/categorydataset.h>

IMPLEMENT_CLASS( ClickableRenderer, Renderer )

ClickableRenderer::ClickableRenderer()
{
}


ClickableRenderer::~ClickableRenderer()
{
}

IMPLEMENT_CLASS( ClickableBarRenderer, ClickableRenderer )

/**
* Constructs new bar renderer.
* @param barType bar type to be drawn by this renderer,
* renderer takes ownership for bar type object
*/
ClickableBarRenderer::ClickableBarRenderer( BarType *barType )
   : BarRenderer( barType ), m_bHasDrawn(false)
{
}

ClickableBarRenderer::~ClickableBarRenderer()
{
   wxDELETE( m_barType );
}

//
// Renderer
//
void 
ClickableBarRenderer::DrawLegendSymbol( wxDC &dc, wxRect rcSymbol, size_t serie )
{
   AreaDraw *barDraw = GetBarDraw( serie );
   barDraw->Draw( dc, rcSymbol );
}

/**
* Draws dataset.
* @param dc device context
* @param horizAxis horizontal axis
* @param vertAxis vertical axis
* @param vertical true to draw vertical bars
* @param dataset dataset to be drawn
*/
void 
ClickableBarRenderer::Draw( wxDC &dc, wxRect rc, Axis *horizAxis, Axis *vertAxis, bool vertical, CategoryDataset *dataset )
{
   m_bHasDrawn = false;
   m_barClickDraws.ClearDrawnHitBoxes();
   BarRenderer::Draw( dc, rc, horizAxis, vertAxis, vertical, dataset );
   m_bHasDrawn = true;
}

void 
ClickableBarRenderer::DrawBar( wxDC& dc, wxRect& rc, size_t serie, size_t category )
{
   ClickableShape* shape = GetBarDraw( serie );
   shape->Draw( dc, rc, serie, category );
}

/**
* Sets bar type, an object that performs bars drawing.
* BarRenderer owns this object.
* @param barType new bar type,
* renderer takes ownership for bar type object
*/
void 
ClickableBarRenderer::SetBarType( BarType *barType )
{
   BarRenderer::SetBarType( barType );
}

/**
* Returns bar type.
* @return bar type
*/
BarType *
ClickableBarRenderer::GetBarType()
{
   return m_barType;
}

/**
* Sets area draw object to draw specified serie.
* @param serie serie index
* @param ad area draw for serie
*/
void 
ClickableBarRenderer::SetBarDraw( size_t serie, ClickableShape *areaDraw )
{
   m_barClickDraws.SetAreaDraw( serie, areaDraw );
}

void 
ClickableBarRenderer::SetBarDraw( size_t serie, AreaDraw *areaDraw )
{
   // TODO: Convert to clickableshape?
   m_barClickDraws.SetAreaDraw( serie, new InertShape( areaDraw ) );
}

/**
* Returns area draw object, used to draw specified serie.
* @param serie serie index
* @return area draw object
*/
ClickableShape *
ClickableBarRenderer::GetBarDraw( size_t serie )
{
   ClickableShape* barDraw = m_barClickDraws.GetAreaDraw( serie );
   if( barDraw == NULL )
   {
      // barDraw = new FillAreaDraw(GetDefaultColour(serie), GetDefaultColour(serie));
      barDraw = new ClickableFillAreaDraw( *wxTRANSPARENT_PEN, GetDefaultColour( serie ) );

      m_barClickDraws.SetAreaDraw( serie, barDraw );
   }
   return barDraw;
}

double 
ClickableBarRenderer::GetMinValue( CategoryDataset *dataset )
{
   return m_barType->GetMinValue( dataset );
}

double 
ClickableBarRenderer::GetMaxValue( CategoryDataset *dataset )
{
   return m_barType->GetMaxValue( dataset );
}

ClickableShape* 
ClickableBarRenderer::GetDataAtPoint( wxPoint& pt )
{
   if( m_bHasDrawn )
   {
      return m_barClickDraws.GetDataAtPoint( pt );
   }
   else
   {
      return NULL;
   }
}
