#include "wfc.hpp"


StringMap::StringMap(std::string filename) : width(0), height(0) 
{
	try
	{
		import(filename);
	}
	catch(incorrect_map_lines& e)
	{
		std::cout << "Importing StringMap failed!\n" << e.what() << ". Emptying StringMap...." << std::endl;
		erase_data();
	}
	catch(std::exception& e)
	{
		std::cout << "Importing StringMap failed!\n" << e.what() << std::endl;
	}
	
	types = calculate_types();
}

bool StringMap::import(std::string filename)
{
	std::ifstream istr_row;
	std::string line;
	std::string word;
	istr_row.open(filename,std::ifstream::in);
	size_t width_temp; // Counting value for number of columns on each line
	erase_data();
	while(std::getline(istr_row,line))
	{
		width_temp = 0;
		std::stringstream istr_col(line);
		
		while(std::getline(istr_col,word,';'))
		{
			data.push_back(word);
			width_temp++;
		}
		if(width == 0){width = width_temp;} //If this is first row
		else
		{
			if(width != width_temp)
			{
				//IF lengths of lines in file are inconsistent
				throw incorrect_map_lines();
				return false;
			}
		}
		height += 1;
	}
	return true;
}

void StringMap::erase_data()
{
	//std::cout << "Erasing tileMap data...." << std::endl;
	data.clear();
	width = 0; height = 0;
}

void StringMap::print() const
{
	if(width == 0 | height == 0) { std::cout << "StringMap of size 0x0 (Empty)" << std::endl;}
	else
	{
		std::cout << "Map of size" << width << "x" << height << std::endl;
		unsigned int str_idx = 1;
		for(std::string str_i : data)
		{
			std::cout << str_i;
			if(str_idx % width == 0) { std::cout << "\n"; } else { std::cout << "-";}
			str_idx++;
		}
		std::cout << std::endl;
	}
}

std::vector<std::string> StringMap::calculate_types()
{
	std::vector<std::string> types;
	for(std::string str : data)
	{
		// If: not found
		if(std::find(types.begin(), types.end(), str) == types.end()) {types.push_back(str);}
	}
	return types;
}


std::vector<double> StringMap::calculate_frequency()
{
	std::vector<double> freqs;
	int sum = width*height;
	for(std::string str : types)
	{
		int occ_i = std::count(data.begin(),data.end(),str);
		double freq_i = (double)occ_i/(double)sum;
		freqs.push_back(freq_i);
	}
	return freqs;
}

void StringMap::print_types()
{
	for(auto str : types)
	{
		std::cout << str << " ";
	}
	std::cout << std::endl;
}

std::map<std::string,int> StringMap::get_type_map() const
{
	std::map<std::string,int> type_map;
	unsigned int i = 0;
	for(std::string type : types)
	{
		type_map.insert(std::make_pair(type,i));
		i++;
	}
	return type_map;
}

void StringMap::write_to_file(std::string filename) const
{
	std::ofstream ofs (filename, std::ofstream::out);
	std::cout << "Writing generated map to " << filename << std::endl;
	unsigned int str_idx = 1;
	for(std::string str_i : data)
	{
		ofs << str_i;
		if(str_idx % width == 0) { ofs << "\n"; } else { ofs << ";";}
		str_idx++;
	}
}

// Constructor
WFC::WFC(std::string filename) : m_filename(filename) , input_sample(filename) 
{
	tile_type_map = input_sample.get_type_map();
	tile_types = input_sample.get_types();
	freq_vector = input_sample.calculate_frequency();
	size_t n_types = tile_type_map.size();;
	for(auto it = tile_type_map.begin(); it != tile_type_map.end(); it++)
	{
		std::vector<double> zeros;
		std::vector<std::vector<double>> zeros2D;
		zeros.insert(zeros.begin(),n_types,0.0);
		zeros2D.insert(zeros2D.begin(),8,zeros);
		neig_probs.insert(std::make_pair(it->first,zeros2D));
	}
	calculate_neigs();
}

void WFC::print_input() const
{
	input_sample.print();
}

void WFC::print_types() const
{
	std::cout << "Types: " << std::endl;
	if(tile_type_map.begin() == tile_type_map.end()) 
	{ 
		std::cout << "(Empty)" << std::endl; 
	}
	else
	{
		for(auto it = tile_type_map.begin(); it != tile_type_map.end();it++)
		{
			std::cout << it->first << "," << it->second << std::endl;
		}
	}
}

void WFC::print_freqs() const
{	
	std::cout << "Frequencies of all types: ";
	if(freq_vector.begin() == freq_vector.end())
	{
		std::cout << "(Empty)";
	}
	else
	{
		for(auto i : freq_vector)
		{
			std::cout << i << " "; 
		}			
	}
	std::cout << std::endl;
}

void WFC::print_neigs() const
{
	std::cout << "Neighbour probabilities for each tile type: " << std::endl;
	if(neig_probs.begin() == neig_probs.end())
	{
		std::cout << "(Empty)" << std::endl;
	}
	else
	{
		for(auto it = neig_probs.begin();it != neig_probs.end(); it++)
		{
			std::cout << it->first << ": " << std::endl;
			for(auto neig : it->second)
			{
				for(auto prob : neig)
					std::cout << prob << ", ";
				std::cout << std::endl;
			}
		}

	}
}

void WFC::print_wave_function(size_t dim_x,size_t dim_y) const
{
	std::cout << "Wave function: " << std::endl;
	if(wave_function.size() == 0) { std::cout << "(Empty)"; }
	else
	{
		unsigned int wave_idx = 1;
		for(std::vector<double> wave_i : wave_function)
		{
			print_vector(wave_i);
			if(wave_idx % dim_x == 0) { std::cout << "\n"; } else { std::cout << "|";}
		}		
	}
	std::cout << std::endl;

}

void WFC::calculate_neigs()
{	
	for(unsigned idx = 0; idx < input_sample.get_width()*input_sample.get_height();idx++)
	{
		neigs_rotation_increment(idx);
	}
	neigs_normalize();
}
// Rotates clock-wise starting from 9 o' Clock. Utilized for the input sample alone when initializing neighbour probs.
// TODO: Avoid access out of range
void WFC::neigs_rotation_increment(unsigned int idx)
{
	std::string cur_type = input_sample[idx];
	size_t dim_x = input_sample.get_width();
	size_t dim_y = input_sample.get_height();
	std::string cur_neig_type; // The neighbour we currently are looking at (the one rotated)
	unsigned int count = 0;
	for(unsigned int neig_idx : get_neighbours(idx, dim_x, dim_y))
	{
		// Check whether neighbour is valid. Else: skip
		if(neig_idx < dim_x*dim_y - 1)
		{
			cur_neig_type = input_sample[neig_idx];
			neig_probs[cur_type][count][tile_type_map[cur_neig_type]] += 1;			
		}
		count++;
	}
}
// TODO: fix it so it actually updates neig_probs
void WFC::neigs_normalize()
{
	for(auto it = neig_probs.begin();it != neig_probs.end(); it++)
	{
		for(std::vector<double> neig : it->second)
		{
			double sum = 0;
			for(double prob : neig) { sum += prob; }
			for(unsigned int i = 0; i < neig.size();i++) { neig[i] = neig[i]/sum; } // Normalization step
		}
	}
}

StringMap WFC::generate_map(size_t dim_x, size_t dim_y)
{
	//0. Erase old data in containers
	// TODO: swap with empty vector in order to completely release all memory (?)
	wave_function.clear();
	if(freq_vector.size() == 0) { throw freq_vector_empty(); }
	
	//1. Initialize WaveFunction with dimensions dim_x*dim_y. Set each value to freq_vector. Initialize queue.
	//TODO: What if freq_vector is empty?
	std::cout << "Generating map of dimensions: " << dim_x << "x" << dim_y << " ......" << std::endl;
	double sh_entropy = shannon_entropy(freq_vector);
	for(unsigned int i = 0; i < dim_x*dim_y;i++)
	{
		wave_function.push_back(freq_vector);
		queue.push_back(std::make_pair(10000,i));
	}
	// Initialize parameters
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<double> real_distribution(0.0,1.0); // For generating tile types
	int queue_idx; double type; int wave_idx; int type_idx;
	//2. Set First n random Tiles to tiletype (weighted by probs)
	unsigned int first_n = (dim_x*dim_y)/90; // divisor a magic value that is inversely proportional to the number of initial randomized tiles
	//std::cout << "first_n = " << first_n << std::endl;
	for(unsigned int k = 0; k < first_n;k++)
	{
		unsigned int max_idx = queue.size()-1;
		if(max_idx >= queue.size() || max_idx >= wave_function.size()) { throw std::out_of_range("generate_map: max_idx larger than queue/wave_function size"); }
		std::uniform_int_distribution<int> int_distribution(0,max_idx); // For generating first queue index
		queue_idx = int_distribution(generator);
		queue_idx = int_distribution(generator); // Generate one time extra (0 is generated for some reason the first time always)
		type = real_distribution(generator);
		//std::cout << "Randomly choosing first queue index..... " << queue_idx << std::endl;
		//std::cout << "Randomly choosing first type..... " << type << std::endl;  
		wave_idx = queue[queue_idx].second;
		type_idx = get_type_idx(freq_vector,type);
		set_tile_type(wave_idx,type_idx);
		//update_neighbours(wave_idx,dim_x,dim_y);
		//3. Update probabilities of all affected neighbours
		update_wave_neigs(wave_idx,type_idx,dim_x,dim_y);	
		//4. Remove from queue
		queue.erase(queue.begin() + queue_idx);
	}
	//5. Loop through the queue
	size_t n_iterations = queue.size();
	for(unsigned int count = 0; count < n_iterations; count++)
	{
		//std::cout << "-----------ITERATION-" << count << "-----------" << std::endl; 
		// 1. Sort the queue according to shannon entropy (after entropy values have been updated in update_wave_neigs)
		std::sort(queue.begin(), queue.end());
		// 2. Choose Tile with lowest entropy
		if(queue.size() == 0) { throw std::out_of_range("generate_map: queue is empty before it should be. bug in code?"); }
		wave_idx = queue[0].second;
		// 3. Choose a tiletype randomly and set it to wave_idx
		type = real_distribution(generator);
		if(wave_idx > wave_function.size()){ throw std::out_of_range("generate_map: wave_idx > wave_function.size()"); }
		type_idx = get_type_idx(wave_function[wave_idx],type);
		set_tile_type(wave_idx,type_idx);
		// 4. Update wave function of neighbours
		update_wave_neigs(wave_idx,type_idx,dim_x,dim_y);
		// 5. Erase first element in queue
		queue.erase(queue.begin());
	}
	//std::cout << "WAVEFUNCTION READY:" << std::endl;
	//print_wave_function(dim_x,dim_y);
	// Wavefunction is ready!
	// Create StringMap based on Wave function
	StringMap sm = create_stringMap(dim_x,dim_y);
	//std::cout << "STRINGMAP READY: " << std::endl;
	//sm.print();
	std::cout << "Generation successful! " << std::endl;
	return sm;
}

double WFC::shannon_entropy(std::vector<double> probs) const
{
	// H = -sum(p_i*log_2(p_i))
	double H = 0;
	for(double p_i : probs)
	{
		H += p_i*log2(p_i);
	}
	return -H;
}
// Gets type_idx from randomly generated double value type (0-1)
int WFC::get_type_idx(std::vector<double> probs,double type) const
{
	//std::cout << "|||||||||||| get_type_idx ||||||||||||" << std::endl;
	//std::cout << "INPUTS probs: "; print_vector(probs); std::cout << "type: " << type << std::endl; 
	double current = 0;
	for(unsigned int type_idx = 0; type_idx < probs.size(); type_idx++)
	{
		current += probs[type_idx];
		//std::cout << "current: " << current << ", type: " << type << std::endl;
		if(type < current) 
		{	
			//std::cout << "------> return: " << type_idx << std::endl;
			return type_idx; 
		}
	}
	throw std::out_of_range("get_type_idx: exited loop before type < current. This should never happen. Bug in code?");
}

void WFC::set_tile_type(unsigned int wave_idx,unsigned int type_idx)
{
	//std::cout << "|||||||||||| set_tile_type ||||||||||||" << std::endl;
	//std::cout << "INPUTS idx: " << wave_idx << ", type_idx: " << type_idx << std::endl; 
	if(wave_idx < wave_function.size())
	{
		std::fill(wave_function[wave_idx].begin(), wave_function[wave_idx].end(), 0);
		wave_function[wave_idx][type_idx] = 1;
		//std::cout << "------> return: "; print_vector(wave_function[wave_idx]); std::cout << std::endl;		
	}
	else
	{
		throw std::out_of_range("set_tile_type: wave_idx >= wave_function.size()");
	}
}

std::vector<unsigned int> WFC::get_neighbours(unsigned int idx, size_t dim_x, size_t dim_y)
{
	std::vector<unsigned int> indices;
	size_t max_idx = dim_x*dim_y - 1;
	unsigned int n_idx;
	// W
	n_idx = idx - 1;
	if((n_idx < max_idx) & (n_idx/dim_x == idx/dim_x)){indices.push_back(n_idx);} else {indices.push_back(-1);}
	//NW
	n_idx = idx - dim_x - 1;
	if((n_idx < max_idx) & (n_idx/dim_x == idx/dim_x - 1)){indices.push_back(n_idx);} else {indices.push_back(-1);}
	//N
	n_idx = idx - dim_x;
	if((n_idx < max_idx) & (n_idx/dim_x == idx/dim_x - 1)){indices.push_back(n_idx);} else {indices.push_back(-1);}
	//NE
	n_idx = idx - dim_x + 1;
	if((n_idx < max_idx) & (n_idx/dim_x == idx/dim_x - 1)){indices.push_back(n_idx);} else {indices.push_back(-1);}
	//E
	n_idx = idx + 1;
	if((n_idx < max_idx) & (n_idx/dim_x == idx/dim_x)){indices.push_back(n_idx);} else {indices.push_back(-1);}
	//SE
	n_idx = idx + dim_x + 1;
	if((n_idx < max_idx) & (n_idx/dim_x == idx/dim_x + 1)){indices.push_back(n_idx);} else {indices.push_back(-1);}
	//S
	n_idx = idx + dim_x;
	if((n_idx < max_idx) & (n_idx/dim_x == idx/dim_x + 1)){indices.push_back(n_idx);} else {indices.push_back(-1);}
	//SW
	n_idx = idx + dim_x - 1;
	if((n_idx < max_idx) & (n_idx/dim_x == idx/dim_x + 1)){indices.push_back(n_idx);} else {indices.push_back(-1);}
	return indices;
}

void WFC::update_wave_neigs(unsigned int wave_idx,unsigned int type_idx,size_t dim_x, size_t dim_y)
{
	//std::cout << "|||||||||||| update_wave_neigs ||||||||||||" << std::endl;
	//std::cout << "INPUTS idx: " << wave_idx << " ,type_idx: " << type_idx << ",dim_x: " << dim_x << " ,dim_y: " << dim_y << std::endl; 
	size_t max_idx = dim_x*dim_y;
	unsigned int queue_idx = 0;
	unsigned int neig_i = 0; // Specifies index in neig_probs
	for(unsigned int i : get_neighbours(wave_idx, dim_x, dim_y))
	{
		if(i < max_idx) // Check whether neighbour is valid
		{
			//std::cout << "UPDATING i=" << i << " ......" << std::endl;
			// prob vector of type idx for neighbour #neig_i: neig_probs[tile_types[type_idx]][neig_i];	
			std::vector<double> probs1;
			try{ probs1 = neig_probs.at(tile_types[type_idx])[neig_i]; } catch(std::exception& e) { std::cerr << "update_wave_neigs: neig_probs out of range."; }
			if(i >= wave_function.size()) { throw std::out_of_range("update_wave_neigs: i >= wave_function.size()"); }
			std::vector<double> probs2 = wave_function[i];
			//print_vector(probs1); std::cout << " ; "; print_vector(probs2); std::cout << std::endl;
			std::vector<double> new_wave = dot_and_normalize(probs1,probs2);
			queue_idx = 0;
			for(auto it = queue.begin(); it != queue.end(); it++){ queue_idx++; if(it->second == wave_idx){ break;} } //Find queue idx corresponding to wave_idx
			queue[queue_idx].first = shannon_entropy(new_wave);// Update shannon entropy for queue
			wave_function[i] = new_wave;
		}
		neig_i++;
	}
	//std::cout << "-------> ready" << std::endl;
}

std::vector<double> WFC::dot_and_normalize(std::vector<double> probs1, std::vector<double> probs2) const
{
	//std::cout << "|||||||||||| dot_and_normalize ||||||||||||" << std::endl;
	std::vector<double> norm_dot;
	//checks whether probs1 and probs2 have same size
	if(probs1.size() != probs2.size()) { throw std::invalid_argument("dot_and_normalize: Vectors not of same size! "); }
	// Dot product
	double sum = 0;
	double prod;
	for(unsigned int i = 0; i < probs1.size(); i++)
	{
		prod = probs1[i]*probs2[i];
		sum += prod;
		norm_dot.push_back(prod);
	}
	//If conflict occured, i.e. all tile types getting 0 probability
	if(sum <= 0)
	{
		std::cout << "Contradiction occured! Setting uniform probability for all types" << std::endl;
		for(unsigned int j = 0; j < probs1.size(); j++) { norm_dot[j] = 1.0/probs1.size(); }
	}
	//else: Normalization
	else
	{
		for(unsigned int j = 0; j < probs1.size(); j++) { norm_dot[j] = norm_dot[j]/sum; }		
	}

	if(std::isnan(norm_dot[0]) | std::isnan(norm_dot[1]) | std::isnan(norm_dot[2]) )
	{
		print_vector(probs1); std::cout << " * "; print_vector(probs2); std::cout << " -----> "; print_vector(norm_dot); std::cout << std::endl;
		throw std::invalid_argument("dot_and_normalize: nan value in norm_dot. This should not happen. Bug in code?");
	}
	return norm_dot;
}

StringMap WFC::create_stringMap(size_t dim_x, size_t dim_y) const
{
	StringMap sm(dim_x,dim_y);
	int max = 0;
	int type_idx = 0;
	unsigned int cur_idx = 0;
	for(std::vector<double> wave_i : wave_function)
	{
		for(double type_value : wave_i)
		{
			if(type_value > max) {max = type_value; type_idx = cur_idx; } // Find the type_idx with the largest prob
			max = 0;
			cur_idx++;			
		}
		cur_idx = 0;
		if(type_idx < tile_types.size())
		{
			sm.push_back(tile_types[type_idx]);	
		}
		else
		{
			throw std::out_of_range("create_stringMap: type_idx >= tile_types.size()");
		}
	}
	return sm;
}

// prints out n randomly generated maps Based on input_map 
void test_wfc(std::string input_map,size_t dim_x, size_t dim_y, int n)
{
	WFC* wfc = new WFC(input_map);
	for(unsigned int i = 0; i < n; i++)
	{
		StringMap sm = wfc->generate_map(dim_x,dim_y);
		sm.print();
	}
} 
