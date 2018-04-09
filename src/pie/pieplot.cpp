/////////////////////////////////////////////////////////////////////////////
// Name:    pieplot.cpp
// Purpose: pie plot implementation
// Author:    Moskvichev Andrey V.
// Created:    2008/11/07
// Copyright:    (c) 2008-2010 Moskvichev Andrey V.
// Licence:    wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/pie/pieplot.h>
#include <wx/drawutils.h>
#include <wx/pie/PieRenderer.h>
#include <wx/drawobject.h>

#include <math.h>

/*
 * TODO Initial quick and dirty. Must be rewritten.
 */

static void Rotate(wxCoord &x, wxCoord &y, wxCoord xc, wxCoord yc, double rad, double angle)
{
    x = (wxCoord) (rad * cos(angle) + xc);
    y = (wxCoord) (-rad * sin(angle) + yc);
}

static void EllipticEgde(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double angle, wxCoord &outX, wxCoord &outY)
{
    double degs = angle * M_PI / 180;

    double w = width;
    double h = height;

    outX = (wxCoord) (w * cos(degs) / 2 + x + w / 2);
    outY = (wxCoord) (-h * sin(degs) / 2 + y + h / 2);
}

PiePlot::PiePlot()
{
    m_dataset = NULL;

    m_use3DView = false;
    m_ellipticAspect = 1.0f;
    //colorScheme = defaultColorScheme;
    m_outlinePen = *wxBLACK_PEN;
    m_labelsFont = *wxSMALL_FONT;

    m_serie = 0; // default behaviour - use first serie

    m_legendPlotGap = 2;
    m_legend = NULL;
}

PiePlot::~PiePlot()
{
    SAFE_REMOVE_OBSERVER(this, m_dataset);
    SAFE_UNREF(m_dataset);
    wxDELETE(m_legend);
}

void PiePlot::SetDataset(CategoryDataset *dataset)
{
    SAFE_REPLACE_OBSERVER(this, m_dataset, dataset);
    SAFE_REPLACE_UNREF(m_dataset, dataset);

    // TODO: Renderer should really be a plot level thing...
    // Add a default renderer if the dataset's renderer is not valid. 
    PieRenderer* pieRenderer = dynamic_cast<PieRenderer*>(m_dataset->GetRenderer());
    if( pieRenderer == nullptr )
    {
       m_dataset->SetRenderer( new PieRenderer() );
    }

    FirePlotNeedRedraw();
}

void PiePlot::SetColorScheme(ColorScheme *colorScheme)
{
    m_colorScheme = *colorScheme;
    FirePlotNeedRedraw();
}

void PiePlot::SetLegend(Legend *legend)
{
    wxREPLACE(m_legend, legend);
    FirePlotNeedRedraw();
}

bool PiePlot::HasData()
{
    return m_dataset != NULL && (m_dataset->GetSerieCount() >= m_serie);
}

ClickableShape*
PiePlot::GetDataAtPoint( wxPoint& pt )
{
   return nullptr;
}


void PiePlot::DatasetChanged(Dataset *WXUNUSED(dataset))
{
    FirePlotNeedRedraw();
}

void PiePlot::DrawData(ChartDC& cdc, wxRect rc)
{
    // TODO initial quick and dirty, need cleanup.
    //
    double sum = 0;
    
    wxDC& dc = cdc.GetDC();

    for (size_t n = 0; n < m_dataset->GetCount(); n++) {
        sum += m_dataset->GetValue(n, m_serie);
    }


    wxRect rcLegend;
    if (m_legend != NULL) {
        wxSize legendExtent = m_legend->GetExtent(dc, *m_dataset);

        switch (m_legend->GetHorizPosition()) {
        case wxLEFT:
            rcLegend.x = rc.x;

            rc.x += legendExtent.x + m_legendPlotGap;
            rc.width -= legendExtent.x + m_legendPlotGap;
            break;
        case wxRIGHT:
            rcLegend.x = rc.x + rc.width - legendExtent.x + m_legendPlotGap;

            rc.width -= legendExtent.x + m_legendPlotGap;
            break;
        case wxCENTER:
            rcLegend.x = rc.x + rc.width / 2 - legendExtent.x / 2;
            break;
        default:
            //(wxT("Invalid legend horizontal position"));
            return ;
        }

        switch (m_legend->GetVertPosition()) {
        case wxTOP:
            rcLegend.y = rc.y;

            rc.y += legendExtent.y + m_legendPlotGap;
            rc.height -= legendExtent.y + m_legendPlotGap;
            break;
        case wxBOTTOM:
            rcLegend.y = rc.y + rc.height - legendExtent.y + m_legendPlotGap;

            rc.height -= legendExtent.y + m_legendPlotGap;
            break;
        case wxCENTER:
            rcLegend.y = rc.y + rc.height / 2 - legendExtent.y / 2;
            break;
        default:
            //(wxT("Invalid legend vertical position"));
            return;
        }

        rcLegend.width = legendExtent.x;
        rcLegend.height = legendExtent.y;

        CheckFixRect(rcLegend);

        m_legend->Draw(dc, rcLegend, *m_dataset);
    }

    PieRenderer* renderer = (PieRenderer*)m_dataset->GetRenderer();
    renderer->Draw( dc, rc, m_dataset );
    
}

