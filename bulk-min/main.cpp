#include "base_method.h"
#include "parser.h" 
#include "genetic.h" 


int main()
{
	parser p;
	auto data = p.load("..\\input_files\\test\\BULK_1.txt");
	data.randomize_conductivities();

	/*database data({ 1.0, 2, 2 });

	for (int i = 0; i < 2; i++)
	{
	  data.m_conductivities[i] = 0.0;
	}*/

	auto data_ptr = std::make_shared<database>(data);

	//data.print();

	genetic method;
	std::cout << method.test_run(data_ptr) << std::endl;
	method.run(data_ptr);

	return 0;
}