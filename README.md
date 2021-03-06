# Battery Collector Analytics Tool
![analyticsprogram](https://user-images.githubusercontent.com/9254173/28645441-6942c68c-7255-11e7-92dc-2a0ade52b2a4.png)

This tool is used to visualise, by way of trajectories and heatmaps, player movements and locations of battery pickups from the Battery Collector game. See the [repo for Battery Collector here](https://github.com/NevilleKing/Battery-Collector).

The easiest way to use this tool is to download the executable from the 
[releases page](https://github.com/NevilleKing/Game-Engine-Analytics/releases/latest), where the sample log files described below
are included.

## Log Files
2 Log files are provided:
- player_location.log - An example of player position data
- battery_location.log - An example of battery collection information

## Loading the Log Files
There are 2 ways to load log files: 
- Firstly you can pass the file path as a command line parameter e.g. `program.exe file1.log file2.log ...`
- Secondly you can drag and drop the file into the window

The program supports up to 5 log files at one time, with each having a different colour. The heatmap colour interpolates between the colours in a bin which has data for more than one log file.

## Controls
- Click and Drag - Pan the visualisation
- Mouse Wheel / + / Double-click - Zoom In
- Mouse Wheel / - - Zoom Out
- ] - Increase Line Width
- [ - Decrease Line Width

## Command Line Parameters
- `--binX [int]` - The number of bins for the heatmap on the x-axis
- `--binY [int]` - The number of bins for the heatmap on the y-axis

## Generating an sln
To generate the project files (sln) run the auto.bat file. This requires cmake and conan to be installed on the machine.
