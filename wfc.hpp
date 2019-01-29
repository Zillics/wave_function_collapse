#ifndef STRATEGY_WFC_H
#define STRATEGY_WFC_H
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <random>
#include <math.h>
#include <chrono>
#include "exceptions.hpp"
/*
HOW TO USE:
StringMap
	Description
		StringMap is a Class for containing and writing/reading data for Class Map.
		So far, the class only contains information about Tile type for each coordinate in Map.
		The tileType is marked by one letter (e.g. 'W' for water).
		The tiles are separated by delimiter ';'.
	The functions that are worth interacting with outside the class: 
		StringMap::import
		StringMap::print
		StringMap::erase
		StringMap::write_to_file

WFC
	Description
		WFC is a Class for autogenerating maps. It utilizes the Wave Function Collapse algorithm.
		The algorithm takes a sample tileMap (in the same format as StringMap) as input.
		It generates a tile map of user defined length. The generated tile map will contain the same tile types as in the sample
		and those types will be laid out in a similar (but not identical) pattern. E.g. an island in water as input will
		generate several islands of random size and shape surrounded by water. 
		The output is a StringMap that can for example be written to a file (which in turn can be read by Map).
		Some tweaking in the input sample file might help with
		obtaining satisfying results.		

	The functions that are worth interacting with outside the class:
		WFC::generate_map
		test_wfc()
		WFC::print_* (all print functions are especially useful for debugging)
	
	Example use case:
		WFC* wfc = new WFC("Maps/input_map_2.txt");
		StringMap sm = wfc->generate_map(60,60);
		sm.write_to_file("generated_map.txt"):
	Example of testing whether randomizing works (all the maps should be completely different)
		test_wfc("Maps/input_map.txt",80,50,10); //prints the maps to terminal (zoom out in terminal to see the patterns)
*/

class StringMap
{
public:
	// Constructor. Fills data based on text in filename. Format (for 2x2 map of element X): X;X\nX;X
	StringMap(std::string filename); 
	// Second constructor with parameters as dimensions 
	StringMap(size_t dim_x, size_t dim_y) : width(dim_x), height(dim_y) {}
 
 	/* Creates and stores the StringMap from file. 
 	* Parameters:
 	*tileConfiguration: File with text of a certain format that contains all info regarding Tile configuration required
 	*  Example: "F;F;G;G;\nF;F;G;F\nG;G;G;" (3x3)
 	* Returns:
 	* true, if the import was successful and false otherwise */
	bool import(std::string filename);
	/*
	*
	*/
	void print() const;
	/*
	*Erases data and returns width and height to 0
	*/
	void erase_data();
	/*
 	* Returns frequency value (in input_sample) for each tileType (based on tile_types)
	*/
	std::vector<double> calculate_frequency();
	/*
	* Analyzes the data and returns all the different Tile types in vector of std::string (order is based on occurance)
	*/
	std::vector<std::string> calculate_types();
	/*
	*  Inserts a new string element on the back of the data
	*/
	void push_back(std::string str) { data.push_back(str); }

	std::vector<std::string> get_types() const { return types; }

	std::map<std::string,int> get_type_map() const;

	size_t get_width() const {return width;}
	size_t get_height() const {return height;}

	void print_types();

	std::string operator[](unsigned int idx) {return data[idx];}
	/*
	* Writes the whole StringMap to a file. Each element separated by ";" and each row separated by \n
	*/
	void write_to_file(std::string filename) const;
private:
	std::vector<std::string> data;
	std::vector<std::string> types;
	size_t width;
	size_t height;
};

// Wave Function Collapse
//1. Import user defined tileMap (MxN) from file -> StringMap
//2. Calculate frequency of each Tile type -> std::vector<double> freq_vector = [0.25,0.02,0.5.....]
//3. Generate probability of neighbours - vector for each Tile type: neighbor_prob
//3. Initialize std::vector<std::vector<double>> WaveFunction = [freq_vector,freq_vector,freq_vector....] (MxN)
//4. Choose a random element of WaveFunction and choose randomly (weighted by double values) a Tile type
//5. Update all other elements according to neighbor_prob
//6. Repeat 4-5 until all Tiles have been set to one Tile type


class WFC
{
public:
	/*
 	* Constructor
 	* Takes filename of input map (same file format as StringMap and TileMap uses) as parameter.
 	* Initializes internal variables based on input map.
	*/		
	WFC(std::string filename);
	/*
 	* Print functions used for debugging
	*/	
	void print_input() const;
	void print_types() const;
	void print_freqs() const;
	void print_neigs() const;
	void print_wave_function(size_t dim_x,size_t dim_y) const;
	/*
 	* Calculates neigs based on input_sample
	*/	
	void calculate_neigs();
	/*
 	* One update step (counting sums) that is performed on each coordinate of input_sample in order to calculate neig_probs
	*/
	void neigs_rotation_increment(unsigned int idx);
	/*
 	* Transforms counted sums (in neig_probs) into probabilities for each tile_type for each neighbour.
	*/
	void neigs_normalize();
	/*
 	* Generate StringMap with dimensions dim_x*dim_y
	*/	
	StringMap generate_map(size_t dim_x, size_t dim_y);
	
	double shannon_entropy(std::vector<double>) const;
	/*
 	* Gets index among vector of probabilities (probs) that represents given double value (type)
	*/	
	int get_type_idx(std::vector<double> probs, double type) const;
	/*
 	* Sets tile type on *probs_ptr (second of queue) to type_idx (i.e. *probs_ptr[type_idx] = 1, rest to 0). Also updates wave_function of neighbours
	*/	
	void set_tile_type(unsigned int idx,unsigned int type_idx);
	/*
 	* Calculates indices of all 8 neighbours (all neighbours that don't exist are out of bounds) of index idx from 1D vector, which represents a dim_x*dim_y 2D surface
	*/	
	std::vector<unsigned int> get_neighbours(unsigned int idx, size_t dim_x, size_t dim_y);
	/*
 	* Updates wave_function of neighbours based on neig_probs[idx].second[type_idx]. Also takes care of updating the queue. 
	*/		
	void update_wave_neigs(unsigned int idx,unsigned int type_idx,size_t dim_x, size_t dim_y);
	/*
 	* Performs dot product of two vectors (of same size) and normalizes the result. Returns this normalized product.
	*/		
	std::vector<double> dot_and_normalize(std::vector<double> probs1, std::vector<double> probs2) const;
	/*
 	* Creates StringMap based on WaveFunction values and returns it
	*/		
	StringMap create_stringMap(size_t dim_x, size_t dim_y) const;
	// TODO: Remove print_vector and move to utils + implement Template version
	void print_vector(std::vector<double> v) const
	{
		for(double v_i : v)
		{
			std::cout << v_i << ",";
		}
	}

private:
	std::string m_filename;
	StringMap input_sample;
	std::vector<double> freq_vector;
	std::map<std::string, std::vector<std::vector<double>>> neig_probs;
	std::vector<std::vector<double>> wave_function;
	std::vector<std::pair<double,unsigned int>> queue; //first: shannon entropy, second: index to wave_function
	std::map<std::string,int> tile_type_map;
	std::vector<std::string> tile_types;
};

void test_wfc(std::string input_map,size_t dim_x, size_t dim_y, int n); // prints out n randomly generated maps Based on input_map 

#endif
