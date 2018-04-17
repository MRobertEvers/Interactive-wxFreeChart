/////////////////////////////////////////////////////////////////////////////
// Name:    ClickableShape.h
// Purpose: Extend areadraw and provide hitbox calculations of click 
//           detection
// Author:    Evers, Matthew R.
// Created:    2018/04/12
// Copyright:    (c) 2018 Evers, Matthew R.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#pragma once
class wxDC;
class wxPoint;

#include <wx/areadraw.h>
#include <wx/drawobject.h>
#include <wx/wxfreechartdefs.h>
#include <wx/hashmap.h>
#include <wx/category/categorydataset.h>
#include <wx/sharedptr.h>
#include <map>

// One of these should be made for each drawn shape.
// Although this is independent of how the shape is drawn,
// they usually match exactly.
class HitBox
{
public:
   HitBox() {};
   virtual ~HitBox() {};

   virtual bool IsHit( wxPoint& pt ) = 0;
};

class RectangleHitBox : public HitBox
{
public:
   RectangleHitBox( wxRect& rect ) : m_Rect( rect ) {};
   virtual ~RectangleHitBox() {};

   virtual bool IsHit( wxPoint& pt )
   {
      return m_Rect.GetRect().Contains( pt );
   }

private:
   RectangleAreaType m_Rect;
};

class SemiCircleHitBox : public HitBox
{
public:
   SemiCircleHitBox( SemiCircleAreaType areaType )
      : m_Area( areaType )
   {
   }

   SemiCircleHitBox( wxPoint& center, unsigned int arcStart, unsigned int arcEnd, unsigned int radius )
      : SemiCircleHitBox(SemiCircleAreaType(center, arcStart, arcEnd, radius))
   {
   };

   virtual ~SemiCircleHitBox() {};

   virtual bool IsHit( wxPoint& pt );

private:
   SemiCircleAreaType m_Area;
};

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

// The idea here is that this ties 3 things together.
// The drawing, the data, and "where" the data is.
// (Area Draw), (Clickable Data), (Hit Boxes)
// Hit boxes must be handled in derived classes.
class ClickableShape : public AreaDraw
{
public:
   // TAKES OWNERSHIP OF THE CLICKABLEDATA
   ClickableShape(ClickableData* data);
   virtual ~ClickableShape();

   // If IsHit returns true, then ClickableData will be configured to the correct data.
   virtual bool IsHit( wxPoint& pt ) = 0;
   // DEPRACATED next 2
   virtual void Draw( wxDC &dc, wxRect rc ) = 0; // Called by a non-clickable renderer
   virtual void Draw( wxDC &dc, wxRect rc, size_t serie, size_t category ) = 0; // Called by a clickable renderer.

   // Not pure virtual to maintain backwards compatibility.
   virtual void Draw( wxDC &dc, AreaType& rc, size_t serie, size_t category ) {}; // Called by a clickable renderer.

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

class MultiClickableShape : public ClickableShape
{
public:
   MultiClickableShape( ClickableData* data ) : ClickableShape( data ) {};
   virtual ~MultiClickableShape() {};

   virtual bool IsHit( wxPoint& pt )
   {
      if( m_hitBoxes.size() == 0 )
      {
         return false;
      }

      for( auto& drawn : m_hitBoxes )
      {
         if( drawn.second->IsHit( pt ) )
         {
            DataClicked( drawn.first.first, drawn.first.second );
            return true;
         }
      }

      return false;
   }

   void ClearHitBoxes()
   {
      m_hitBoxes.clear();
   }

   virtual void InitDraw()
   {
      ClearHitBoxes();
   }

   // Takes ownership of hitbox.
   void AddHitBox( HitBox* ptHitBox, size_t serie, size_t category ) 
   {
      // TODO: Tie the data to the hitbox.
      auto key = std::make_pair( serie, category );
      m_hitBoxes.insert( std::make_pair( key, ptHitBox ) );
   };

private:
   std::map<std::pair<size_t, size_t>, wxSharedPtr<HitBox>> m_hitBoxes;
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



class ClickableSemiCircleDraw : public virtual MultiClickableShape, public virtual SemiCircleAreaDraw
{
public:
   ClickableSemiCircleDraw( wxColour fill, wxColour borderPen, ClickableData* data = nullptr );
   virtual ~ClickableSemiCircleDraw();

   virtual void Draw( wxDC &dc, wxRect rc );
   virtual void Draw( wxDC &dc, wxRect rc, size_t serie, size_t category );
   virtual void Draw( wxDC &dc, SemiCircleAreaType& rc, size_t serie, size_t category );
private:

};

class ClickableRectangleDraw : public MultiClickableShape
{
public:
   ClickableRectangleDraw( ClickableData* data );
   ~ClickableRectangleDraw();

   virtual void Draw( wxDC &dc, wxRect rc )
   {
      // Empty virtual function. SHould be overloaded.
   }

   virtual void Draw( wxDC &dc, wxRect rc, size_t serie, size_t category )
   {
      Draw( dc, rc );
      AddHitBox( new RectangleHitBox( rc ), serie, category );
   }
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
