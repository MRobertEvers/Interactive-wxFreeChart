#pragma once
class wxDC;
class wxPoint;

#include <wx/areadraw.h>
#include <wx/drawobject.h>
#include <wx/wxfreechartdefs.h>
#include <wx/hashmap.h>
#include <vector>

class ClickableData
{
   // GetDataset
   // GetRelevant data.
};

class ClickableShape : public AreaDraw
{
public:
   ClickableShape(ClickableData* data);
   ~ClickableShape();

   virtual bool IsHit( wxPoint& pt ) = 0;
   virtual void Draw( wxDC &dc, wxRect rc ) = 0;
   virtual void InitDraw() {};
   
   virtual ClickableData* GetData()
   {
      return m_Data;
   }

private:
   ClickableData * m_Data;
};

class InertShape : public ClickableShape
{
public:
   InertShape(AreaDraw* area) 
      : ClickableShape(nullptr), m_Area(area)
   {

   }

   ~InertShape() 
   {
      delete m_Area;
   }

   virtual bool IsHit( wxPoint& pt )
   {
      return false;
   }

   virtual void Draw( wxDC &dc, wxRect rc )
   {
      m_Area->Draw( dc, rc );
   }

private:
   AreaDraw * m_Area;
};



class ClickableSemiCircleDraw : public ClickableShape
{
public:
   ClickableSemiCircleDraw( wxColour fill, wxColour borderPen, unsigned int iTotalPie,
                            unsigned int iDrawnPie, unsigned int iThisSlice, ClickableData* data );
   ~ClickableSemiCircleDraw();

   virtual bool IsHit( wxPoint& pt );

   virtual void Draw( wxDC &dc, wxRect rc );
private:

   // Its really wasteful to duplicate the rectangle for each semicircle. But I think
   // this is the most flexible way to do it.
   wxRect m_Rect;
   wxColour m_FillColour;
   wxColour m_BorderColour;
   unsigned int m_TotalPie;
   unsigned int m_AlreadyEatenPie;
   unsigned int m_DrawingPie;
   float m_ellipticAspect;

   wxPoint m_Center;
   double m_Radius;
   double m_AngleStart;
   double m_AngleEnd;
};

class ClickableRectangleDraw : public ClickableShape
{
public:
   ClickableRectangleDraw( ClickableData* data );
   ~ClickableRectangleDraw();

   void ClearHitBoxes()
   {
      m_hitBoxes.clear();
   }

   bool IsHit( wxPoint& pt )
   {
      if( m_hitBoxes.size() == 0 )
      {
         return false;
      }

      for( auto& rect : m_hitBoxes )
      {
         if( rect.Contains( pt ) )
         {
            return true;
         }
      }

      return false;
   }

   void AddHitBox( wxRect& rc )
   {
      m_hitBoxes.push_back(rc);
   }

   virtual void InitDraw()
   { 
      ClearHitBoxes(); 
   }

private:
   // TODO: The particular data is lost since multiple hitboxes are tied
   // to one data. This needs to be rectified.
   std::vector<wxRect> m_hitBoxes;
};


class ClickableGradientAreaDraw : public virtual ClickableRectangleDraw, public virtual GradientAreaDraw
{
public:
   ClickableGradientAreaDraw( wxPen borderPen, wxColour colour1, wxColour colour2, wxDirection dir, ClickableData* data );
   ~ClickableGradientAreaDraw();

   virtual void Draw( wxDC &dc, wxRect rc );
};


class ClickableFillAreaDraw : public virtual ClickableRectangleDraw, public virtual FillAreaDraw
{
public:
   ClickableFillAreaDraw( wxPen borderPen = *wxBLACK_PEN, wxBrush fillBrush = *wxWHITE_BRUSH, ClickableData* data = nullptr );
   ClickableFillAreaDraw( wxColour borderColour, wxColour fillColour, ClickableData* data = nullptr );
   ~ClickableFillAreaDraw();

   virtual void Draw( wxDC &dc, wxRect rc );
};

WX_DECLARE_HASH_MAP( int, ClickableShape*, wxIntegerHash, wxIntegerEqual, ClickableAreaDrawMap );
// IY: Class declaration not required because already declared by above 
// declaration (on Linux at least). It may be needed in Windows.
// class WXDLLIMPEXP_FREECHART AreaDrawMap;

/**
* Collection of areadraws for series.
* Used by bar charts, etc.
*/
class WXDLLIMPEXP_FREECHART ClickableAreaCollection
{
public:
   ClickableAreaCollection();
   virtual ~ClickableAreaCollection();

   /**
   * Set areadraw for serie.
   * @param serie serie index
   * @param areaDraw areadraw for serie
   */
   void SetAreaDraw( int serie, ClickableShape*areaDraw );

   /**
   * Returns areadraw, if any, for serie.
   * @param serie serie index
   * @return areadraw
   */
   ClickableShape* GetAreaDraw( int serie );

   void ClearDrawnHitBoxes();

   ClickableShape* GetDataAtPoint( wxPoint& pt );

private:
   ClickableAreaDrawMap m_areas;
};
