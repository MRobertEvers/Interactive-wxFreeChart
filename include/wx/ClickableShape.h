#pragma once
class wxDC;
class wxPoint;

#include <wx/areadraw.h>
#include <wx/drawobject.h>
#include <wx/wxfreechartdefs.h>
#include <wx/hashmap.h>
#include <wx/category/categorydataset.h>
#include <map>

class ClickableData
{
public:
   ClickableData( Dataset* ptData ):m_Dataset(ptData), m_ClickedCategory(0), m_ClickedSeries(0){}
   virtual ~ClickableData() {};

   virtual void Clicked( size_t series, size_t group )
   {
      m_ClickedCategory = group;
      m_ClickedSeries = series;
   }

   // Returns the name of the category that the clicked data belonged.
   virtual wxString GetCategoryName() = 0;

   // Returns the name of the series that the clicked data belongs
   virtual wxString GetSeriesName() = 0;

   // Returns the sum value of all the categories in the clicked series. (Horizontal counting)
   virtual double GetSeriesTotal() = 0;

   // Returns the sum value of all series for the clicked category. (Vertical Counting)
   virtual double GetCategoryTotalOfAllSeries() = 0;

   // Returns the value of the series for the clicked category
   virtual double GetSeriesValue() = 0;

   virtual Dataset* GetDataset()
   {
      return m_Dataset;
   }

protected:
   unsigned int m_ClickedCategory; // A group is a category that the series' contain data for.
   unsigned int m_ClickedSeries; // Series' are the sets of data
   Dataset* m_Dataset;
};

class ClickableCategoryData : public ClickableData
{
public:
   // Does not take ownership
   ClickableCategoryData( CategoryDataset* ptData );
   ~ClickableCategoryData();

   virtual wxString GetSeriesName();
   virtual wxString GetCategoryName();
   virtual double GetSeriesTotal();
   virtual double GetCategoryTotalOfAllSeries();
   virtual double GetSeriesValue();

   CategoryDataset* GetDataset()
   {
      return ((CategoryDataset*)m_Dataset);
   }
};


class ClickableShape : public AreaDraw
{
public:
   // TAKES OWNERSHIP OF THE CLICKABLEDATA
   ClickableShape(ClickableData* data);
   virtual ~ClickableShape();

   // If IsHit returns true, then ClickableData will be configured to the correct data.
   virtual bool IsHit( wxPoint& pt ) = 0;
   virtual void Draw( wxDC &dc, wxRect rc ) = 0;
   virtual void Draw( wxDC &dc, wxRect rc, size_t serie, size_t category ) = 0;
   virtual void InitDraw() {};
   
   virtual ClickableData* GetData()
   {
      return m_Data;
   }

protected:
   ClickableData* m_Data;
   size_t m_Serie;
   void DataClicked( size_t serie, size_t category )
   {
      if( m_Data != nullptr )
      {
         m_Data->Clicked( serie, category );
      }
   }
};

class InertShape : public ClickableShape
{
public:
   InertShape(AreaDraw* area) 
      : ClickableShape(nullptr), m_Area(area)
   {

   }

   virtual ~InertShape() 
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

   virtual void Draw( wxDC &dc, wxRect rc, size_t, size_t )
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
   virtual ~ClickableSemiCircleDraw();

   virtual bool IsHit( wxPoint& pt );

   virtual void Draw( wxDC &dc, wxRect rc );
   virtual void Draw( wxDC &dc, wxRect rc, size_t serie, size_t category );

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

   virtual bool IsHit( wxPoint& pt )
   {
      if( m_hitBoxes.size() == 0 )
      {
         return false;
      }

      for( auto& drawn : m_hitBoxes )
      {
         if( drawn.second.Contains( pt ) )
         {
            DataClicked( drawn.first.first, drawn.first.second );
            return true;
         }
      }

      return false;
   }

   virtual void Draw( wxDC &dc, wxRect rc )
   {

   }

   virtual void Draw( wxDC &dc, wxRect rc, size_t serie, size_t category )
   {
      Draw( dc, rc );
      AddHitBox( rc, serie, category );
   }

   void AddHitBox( wxRect& rc, size_t serie, size_t category )
   {
      // TODO: Tie the data to the hitbox.
      auto key = std::make_pair( serie, category );
      m_hitBoxes.insert(std::make_pair(key, rc));
   }

   virtual void InitDraw()
   { 
      ClearHitBoxes(); 
   }

private:
   // TODO: The particular data is lost since multiple hitboxes are tied
   // to one data. This needs to be rectified.
   std::map<std::pair<size_t, size_t>,wxRect> m_hitBoxes;
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
