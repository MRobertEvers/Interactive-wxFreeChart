/////////////////////////////////////////////////////////////////////////////
// Name:    ClickableRenderer.h
// Purpose: Provide a rendering class that utilizes clickable shapes.
// Author:    Evers, Matthew R.
// Created:    2018/04/12
// Copyright:    (c) 2018 Evers, Matthew R.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/chartrenderer.h>
#include <wx/ClickableShape.h>
#include <wx/bars/barrenderer.h> // For bartype
#include <wx/pie/PieRenderer.h>

class ClickableRenderer : public Renderer
{
   DECLARE_CLASS( ClickableRenderer )
public:
   ClickableRenderer();
   virtual ~ClickableRenderer();

   virtual ClickableShape* GetDataAtPoint( wxPoint& pt ) = 0;
};


/**
* Bar renderer.
*/
class ClickableBarRenderer : public virtual ClickableRenderer, public virtual BarRenderer
{
   DECLARE_CLASS( ClickableBarRenderer )
public:
   /**
   * Constructs new bar renderer.
   * @param barType bar type to be drawn by this renderer,
   * renderer takes ownership for bar type object
   */
   ClickableBarRenderer( BarType *barType );
   virtual ~ClickableBarRenderer();

   //
   // Renderer
   //
   virtual void DrawLegendSymbol( wxDC &dc, wxRect rcSymbol, size_t serie );

   /**
   * Draws dataset.
   * @param dc device context
   * @param horizAxis horizontal axis
   * @param vertAxis vertical axis
   * @param vertical true to draw vertical bars
   * @param dataset dataset to be drawn
   */
   void Draw( wxDC &dc, wxRect rc, Axis *horizAxis, Axis *vertAxis, bool vertical, CategoryDataset *dataset );

   virtual void DrawBar( wxDC& dc, wxRect& rc, size_t serie, size_t category );

   /**
   * Sets bar type, an object that performs bars drawing.
   * BarRenderer owns this object.
   * @param barType new bar type,
   * renderer takes ownership for bar type object
   */
   void SetBarType( BarType *barType );

   /**
   * Returns bar type.
   * @return bar type
   */
   BarType *GetBarType();

   /**
   * Sets area draw object to draw specified serie.
   * @param serie serie index
   * @param ad area draw for serie
   */
   void SetBarDraw( size_t serie, ClickableShape* areaDraw );

   // Overload barrenderer
   virtual void SetBarDraw( size_t serie, AreaDraw *areaDraw );

   /**
   * Returns area draw object, used to draw specified serie.
   * @param serie serie index
   * @return area draw object
   */
   ClickableShape* GetBarDraw( size_t serie );

   double GetMinValue( CategoryDataset *dataset );
   double GetMaxValue( CategoryDataset *dataset );

   ClickableShape* GetDataAtPoint( wxPoint& pt );

   operator BarRenderer()
   {
      return *(BarRenderer*)this;
   }

private:
   ClickableAreaCollection m_barClickDraws;
   bool m_bHasDrawn;
};

class ClickablePieRenderer : public virtual ClickableRenderer, public virtual PieRenderer
{
public:
   ClickablePieRenderer();
   virtual ~ClickablePieRenderer();

   virtual void Draw( wxDC &dc, wxRect rc, CategoryDataset *dataset );
   virtual void DrawShape( wxDC& dc, SemiCircleAreaType& area, size_t serie, size_t category );
   virtual void SetAreaDraw( size_t serie, ClickableShape* area );
   virtual ClickableShape* GetAreaDraw( size_t serie );
   virtual ClickableShape* GetDataAtPoint( wxPoint& pt );

private:
   ClickableAreaCollection m_PieClickDraws;
   bool m_bHasDrawn;
};