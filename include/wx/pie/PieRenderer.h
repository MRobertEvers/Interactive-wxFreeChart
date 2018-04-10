#ifndef PIERENDERER_H_
#define PIERENDERER_H_

#include <wx/wxprec.h>
#include <wx/category/categorydataset.h>
#include <wx/areadraw.h>

class WXDLLIMPEXP_FREECHART PieRenderer : public Renderer
{
public:
   PieRenderer();
   virtual ~PieRenderer();

   /**
   * Draws dataset.
   * @param dc device context
   * @param horizAxis horizontal axis
   * @param vertAxis vertical axis
   * @param vertical true to draw vertical bars
   * @param dataset dataset to be drawn
   */
   virtual void Draw( wxDC &dc, wxRect rc, CategoryDataset *dataset );

   // The draw function passes the "Drawing" act back up to the renderer so it can 
   // call the correct version of the draw function.
   virtual void DrawShape( wxDC& dc, SemiCircleAreaType& area, size_t serie, size_t category );

   virtual void SetAreaDraw( size_t serie, AreaDraw* area );
   virtual AreaDraw* GetAreaDraw( size_t serie );

private:
   AreaDrawCollection m_PieDraws;
};

#endif