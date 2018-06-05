
 wxFreeChart Update Notes:
 ======================================
This repository extends the functionality of wxFreeChart by including clickable plots and tooltip support. 
 All existing wxFreeChart code still works... i.e. all changes are backwards compatible. See change notes for new features.  

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