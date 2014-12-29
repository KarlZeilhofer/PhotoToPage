# PhotoToPage
Karl Zeilhofer, 29.12.2014


## Idea:
You have many photos of let's say your finance documents. Instead of
scanning them in a flat bed scanner, you used your handy smartphone
or your digicam.
Now you want to crop and enhance the images, and put them together
into a single PDF. Then PhotoToPage could be your solution.
Main Focus of the program is on usability, so **you can be very fast on many images**!

## Status:
For now, the software is in alpha status. Some very important
features are still missing.

## Usage:
Open a jpg-file in a folder.
Press 'F' to define the region of the page.
Click on the left border of the page, move curosr to the right edge of the page and click,
then set top and bottom edge (Important: first left, then rigth. Otherwise the
image will exported upside down. Top/bottom in any order).
Then enter the page-number (or any name).
Hit enter, and the defined page(s) in the image get exported to the subfolder /pages.
You can add thumb-hiding rectangles with the shortcut 'T'.

## Features:
- open jpg-file
- next/prev file in folder
- autozoom to windowsize
- QSettings for recentPageNumber, last opened directory and fileindex
- define rectangular page-regions (can be rotated). Input is intuitive
  and fast with 4 mouse-klicks.
- define rectangular regions, which are exported as a white area
  (for e.g. thumb-hiding on the edge of a page). Same input as for apage.
- pagename (if it's a number, it gets autoincremented)fit
- export defined page-regions to the subfolder /pages as a jpg-file
- select and delete page or thumb

## TODOs:
- Status Line with current tool, or tool hints
  perhaps also the cursor should change (like in Sketchup)
- Automated Image-Enhancement.
  The colored image should be reduced to grayscale, and then the used
  valuerange should be trimmed (see the software Shotwell Viewer
  for Linux)
- Keyboard shortcuts are productive, but a bit messy. Tidy them up.
  Communicate them to the user.
- batch process all files in the folder.
- put the output files into one PDF

## Source code:
The software was developed on Kubuntu using the Qt Creator and the Qt SDK Version 4.8.6
The software is published under the GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
