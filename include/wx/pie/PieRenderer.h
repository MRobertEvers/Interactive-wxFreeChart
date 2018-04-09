#ifndef PIERENDERER_H_
#define PIERENDERER_H_

#include <wx/wxprec.h>
#include <wx/category/categorydataset.h>

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

   virtual SemiCircleAreaDraw* GetAreaDraw( size_t serie );

private:
   AreaDrawCollection m_PieDraws;
};

#endif