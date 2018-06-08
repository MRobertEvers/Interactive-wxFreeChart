
 wxFreeChart Update Notes:
 ======================================
This repository extends the functionality of wxFreeChart by including clickable plots and tooltip support. 
 All existing wxFreeChart code still works... i.e. all changes are backwards compatible. See change notes for new features.  


 Usage - Clickable Charts and Tooltips
 =====================
 A working example is present in `bardemos.cpp` and `pieplot.cpp`. See below of a basic clickable bar plot. 
 
 ## Clickable Charts
 
 The basic idea is to create an `InteractivePlot` that uses a `ClickableDataset`.
 Then, use a ChartMode that listens for mouse events! When the ChartMode receives an event, it asks the `InteractivePlot`, 
 what data is at the point that this event occured? Then it can do stuff with that data, such as displaying a tooltip!
 
 Displaying a tooltip is easy! I've added a basic `TextToolTip` that simply displays an array of strings that you can use as a starting point.
 Also in the example below, see the `TextToolTip` usage!
 
    // Step 1. Create a chartmode that receives mouse events from wxFreeChart! See wxChartPanel.cpp.
	class ClickMode : public ChartPanelMode
	{
	public:
	   ClickMode() {}
	   ~ClickMode() {}

	   // wxFreeChart will call this function and pass in the chartPanel object
	   void Init( wxChartPanel* panel )
	   {
		  m_Panel = panel;
	   }
	  
	   // Provide an easy function to show tooltip.
	   void ShowToolTip(ClickableShape* dataShape)
	   {
		  if( dataShape != nullptr )
		  {
			// Get the data from the passed in data shape.
			 auto myData = dataShape->GetData();
			 
			 // Get the string representations of the data.
			 auto categoryName = myData->GetCategoryName();
			 auto categoryValue = wxString( std::to_string( myData->GetSeriesValue() ) );
			 
			 // Create a TextTooltip object with our data. Note! Feel free to override the tooltip class!
			 // It exposes a draw interface that allows you to draw/display the data however you want!
			 auto myTooltip = new TextTooltip(m_LastPoint, {categoryName, categoryValue});
				
			// Tell the chart that the tooltip changed.
			 m_Panel->GetChart()->SetTooltip(myTooltip);
			 m_Panel->ChartChanged(nullptr);
		  }
	   }

	   // Step 2. Listen for mouse events!
	   // pt is the point that the event occured.
	   void ChartMouseDown( wxPoint &pt, int key )
	   {
		  m_LastPoint = pt;

		  // Get the plot object from the panel.
		  auto plot = m_Panel->GetChart()->GetPlot();
		  
		  // Check if the plot is an interactive plot. (It has to be dynamic cast.
		  //  This is a side effect of wanting to minimize changes to wxFreeChart while 
		  //  adding this functionality.)
		  auto intPlot = dynamic_cast<InteractivePlot*>(plot);
		  if( intPlot != nullptr )
		  {
		    // Step 3. Ask the interactive plot, "What data is at that point?"
			// This requires that your plot used a "ClickableDataset"
			 auto data = intPlot->GetDataAtPoint( pt );
			 
			 // See above.
			 ShowToolTip(data);
		  }
	   }

	private:
		// Keep track of the panel that this mode operates on. This is part of the ChartPanelMode
	    wxChartPanel* m_Panel;
	   
	    // Keep track of the point that was clicked last.
	    wxPoint m_LastPoint;
	};
 
 Then create the chart object with the `Clickable` components... You need to be sure to use a `ClickableRenderer` and a `ClickableDataset` as opposed to their 'non-clickable' counterparts.
 
	wxString names[] = {  wxT("Cat 1"), wxT("Cat 2"), wxT("Cat 3"), wxT("Cat 4"), wxT("Cat 5") };
	double values1[] = { 10, 20, 5, 50, 25 };
	double values2[] = { 16, 10, 15, 30, 45 };

	// Create dataset; This will be wrapped with the clickable decorator categorydata class!
	CategorySimpleDataset *dataset = new CategorySimpleDataset(names, WXSIZEOF(names));

	dataset->AddSerie(wxT("Serie 1"), values1, WXSIZEOF(values1));
	dataset->AddSerie(wxT("Serie 2"), values2, WXSIZEOF(values2));

	BarType *barType = new LayeredBarType(40, 0);

	// CLICKABLE! Use the clickable bar renderer here.
	// Set bar renderer for it, with layered bar type. 
	ClickableBarRenderer *renderer = new ClickableBarRenderer(barType);

	// CLICKABLE! Use a clickable dataset here!
	// Some eye-candy: gradient bars
	renderer->SetBarDraw(0, new ClickableGradientAreaDraw(DEFAULT_BAR_FILL_COLOUR_0, DEFAULT_BAR_FILL_COLOUR_0, 
														  DEFAULT_BAR_FILL_COLOUR_0.ChangeLightness(150), wxEAST,
														  new ClickableCategoryData( dataset )));
	renderer->SetBarDraw(1, new ClickableGradientAreaDraw(DEFAULT_BAR_FILL_COLOUR_1, DEFAULT_BAR_FILL_COLOUR_1,
														  DEFAULT_BAR_FILL_COLOUR_1.ChangeLightness(150), wxEAST, 
														  new ClickableCategoryData( dataset )));

	// Notice the cast!
	// assign renderer to dataset
	dataset->SetRenderer((BarRenderer*)renderer);

	BarPlot *plot = new BarPlot();

	// Add left category axis
	CategoryAxis *leftAxis = new CategoryAxis(AXIS_LEFT);
	leftAxis->SetMargins(20, 20);
	plot->AddAxis(leftAxis);

	// Add bottom number axis
	NumberAxis *bottomAxis = new NumberAxis(AXIS_BOTTOM);
	bottomAxis->SetMargins(0, 5);
	plot->AddAxis(bottomAxis);

	// Add dataset to plot
	plot->AddDataset(dataset);

	// Link first dataset with first horizontal axis
	plot->LinkDataHorizontalAxis(0, 0);

	// Link first dataset with first vertical axis
	plot->LinkDataVerticalAxis(0, 0);

	// and finally construct and return chart
	return new Chart(plot, GetName());
	
Finally! When setting up the `ChartPanel`, use the Chart (that has an interactive plot) and use a ChartMode that handles click events.

    m_chartPanel = new wxChartPanel(parent);
	
	// ClickMode is defined above. It is a ChartMode that handles mouse events!
    m_chartPanel->SetMode(new ClickMode()); 
	
	// m_currentDemo->Create() is the function in the code block above!
	// This chart uses an interactive plot.
	m_chartPanel->SetChart(m_currentDemo->Create());
	
## Tooltips

This repo also adds a `ToolTip` base class! I've added a simple `TextToolTip` to demonstrate usage! The basic idea is to override the pure virtual functions in the tooltip class!
Override the `RedrawBackBitmap` function to change the way your tooltip appears! That function exposes a wxDC (Draw Context) so you can display your ToolTip's data however
you want!

	// Usually drawn by the chart object.
	class WXDLLIMPEXP_FREECHART Tooltip
	{
	public:
	   Tooltip( wxPoint& pt );
	   virtual ~Tooltip();

	   virtual void RedrawBackBitmap() = 0; // OVERRIDE ME!
	   virtual void ResizeBackBitmap( wxSize size ) = 0; // OVERRIDE ME!
	   virtual wxPoint GetDataDrawPoint();
	   virtual void Blit( wxDC &cdc, wxPoint &rc );

	protected:
	   wxBitmap m_backBitmap;
	   wxPoint m_tooltipItemPoint;
	};

 
 Implementation Issues
 =====================
I only made a slight effort to use wxContainers. E.g. I used `std::map`, `std::pair`, as well as some functions like `std::floor` and `std::to_string`. This could easily be changed but I found that wxWidgets already used some STL containers in the `Observable` classes. I figured if I was going to remove some `std::` I might as well do all, but I never got around to it.
 
 Changes Notes
 ===============
- Fixed Stacked Bar Chart bug when using more than 2 series'.  
- Added PieRenderer class and ClickablePieRenderer (and other supporting classes) for interactive functionality with Pie Charts.  
- Add ClickableBarRenderer (and supporting classes) for interactive bar charts. See demo...  

 Change Design Notes
 ======
- Areadraw class used to create "ClickableShape" class. ClickableShape class combines an AreaDraw, a HitBox, and associated data.  
- ClickableShape offers an interface for returning data that was drawn at a point... i.e. It checks if a the point is contained in a HitBox, and if so, returns the data that that HitBox maps. Whatever function is used to draw the AreaDraw for the plot NEEDS TO BE OVERLOADED to call the version of Draw on the ClickableShape that maps the HitBox to the data and drawn area.
- ChartPanel passes the click event (and only the click event currently) to the chart mode. The chart mode queries' the InteractivePlot (a new class) if it contains any shapes that were drawn at the clicked point.  
- Changed the concept of drawing rectangle slightly. Now, instead of a drawing rect, renderer's should uses an AreaSpecification. All functions that use wxRect however still work.  
- An AreaSpecification is an object that contains all the necessary data required to draw a polygon. This class was added to share data between the AreaDraw class and the HitBox class. This way, the HitBox can know the exact data that the AreaDraw used to draw its shape.  
- There is some ugly virtual inheritance utilized; this was an attempt to minimize code reproduction while also maintaining complete backwards compatibility.  

 Future
 ======
I have no plan to add more interactive functionality to other charts. I only need BarChart and PieChart to be interactive, and those are the charts I worked on.  

 Credits
 =======
 See [iwbnwif](https://github.com/iwbnwif/wxFreeChart)'s wxFreeChart repo for Credits. This repo is a clone of that.