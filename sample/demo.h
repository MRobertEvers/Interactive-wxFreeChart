/////////////////////////////////////////////////////////////////////////////
// Name:    demo.h
// Purpose: demo application header
// Author:    Moskvichev Andrey V.
// Created:    2008/11/07
// Copyright:    (c) 2008-2009 Moskvichev Andrey V.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef DEMO_H_
#define DEMO_H_

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/treectrl.h>

#include "wx/wxfreechartdefs.h"
#include "wx/chartpanel.h"

#include <wx/aui/aui.h>

#include "democollection.h"
#include <wx/InteractivePlot.h>
#include <wx/tooltips.h>
#include <wx/ClickableShape.h>

class ClickMode : public ChartPanelMode
{
public:
   ClickMode() : m_ToolTip(nullptr){}
   ~ClickMode() {}

   void Init( wxChartPanel* panel )
   {
      m_Panel = panel;
   }
   
   void temp_draw_tooltip( wxPaintDC& dc )
   {
      if( m_ToolTip != nullptr )
      {
         m_ToolTip->Blit( dc, m_LastPoint );
      }
   }

   void ChartEnterWindow()
   {
   }

   void ChartMouseDown( wxPoint &pt, int key )
   {
      auto plot = m_Panel->GetChart()->GetPlot();
      auto intPlot = dynamic_cast<InteractivePlot*>(plot);
      if( intPlot != nullptr )
      {
         auto data = intPlot->GetDataAtPoint( pt );
         if( data != nullptr )
         {
            auto myData = data->GetData();
            m_LastPoint = pt;

            // TODO: This is proof of concept temp. Change this.
            m_ToolTip = new TextTooltip( {
               myData->GetCategoryName(),
               wxString(std::to_string(myData->GetSeriesValue()))
               } );
         }
      }
   }

   void ChartMouseUp( wxPoint &pt, int key )
   {
   }

   void ChartMouseMove( wxPoint &pt )
   {
   }

   void ChartMouseDrag( wxPoint &pt )
   {
   }

   void ChartMouseWheel( int rotation )
   {
   }

private:
   wxChartPanel* m_Panel;
   TextTooltip* m_ToolTip;
   wxPoint m_LastPoint;
};

/**
 * Chart demo select tree control.
 */
class ChartSelector : public wxTreeCtrl
{
public:
    ChartSelector(wxWindow *parent, wxChartPanel *chartPanel, DemoCollection *demoCollection);
    virtual ~ChartSelector();

private:
    void OnTreeItemActivated(wxTreeEvent &ev);

    wxChartPanel* m_chartPanel;
    ChartDemo* m_currentDemo;

    DECLARE_EVENT_TABLE()
};

/**
 * Demo application main frame.
 */
class MainFrame : public wxFrame
{
public:
    friend class ChartSelector;

    MainFrame();
    virtual ~MainFrame();

private:
    //
    // Event handlers
    //
    void OnSaveAsPNG(wxCommandEvent &ev);
    void OnEnableAntialias(wxCommandEvent &ev);
    void OnAbout(wxCommandEvent &ev);
    void OnExit(wxCommandEvent &ev);

    wxAuiManager m_mgr;
    wxChartPanel *m_chartPanel;

    DECLARE_EVENT_TABLE()
};

#endif /*DEMO_H_*/
