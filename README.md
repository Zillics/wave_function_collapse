#HOW TO USE:
##StringMap
	Description
		StringMap is a Class for containing and writing/reading data, a 2D array of tiles
		The tile type is marked by one letter (e.g. 'W' for water).
		The tiles are separated by delimiter ';'.
	The functions that are worth interacting with outside the class: 
		StringMap::import
		StringMap::print
		StringMap::erase
		StringMap::write_to_file

##WFC
Description
	WFC is a Class for autogenerating maps. It utilizes the Wave Function Collapse algorithm.
	The algorithm takes a sample tileMap (in the same format as StringMap) as input.
	It generates a tile map of user defined length. The generated tile map will contain the same tile types as in the sample and those types will be laid out in a similar (but not identical) pattern. E.g. an island in water as input will
	generate several islands of random size and shape surrounded by water. 
	The output is a StringMap that can for example be written to a file (which in turn can be read by Map).
	Some tweaking in the input sample file might help with
	obtaining satisfying results.		
		
The functions that are worth interacting with outside the class:
- WFC::generate_map()
- test_wfc()
- WFC::print_* (all print functions are especially useful for debugging)
	
##Example use case:
	WFC* wfc = new WFC("Maps/input_map_2.txt");
	StringMap sm = wfc->generate_map(60,60);
	sm.write_to_file("generated_map.txt"):
Example of testing whether randomizing works (all the maps should be completely different)
	test_wfc("Maps/input_map.txt",80,50,10); //prints the maps to terminal (zoom out in terminal to see the patterns)
