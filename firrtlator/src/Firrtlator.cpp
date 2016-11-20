#include <IR.h>
#include <Firrtlator.h>

#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
	int c, idx;

	std::vector<std::string> input_files;
	std::string output_file = "out.fir";

	while ((c = getopt (argc, argv, "i:")) != -1) {
		switch(c) {
		case 'i':
			input_files.push_back(optarg);
		}
	}

	if (optind < argc) {
		output_file = argv[optind];
	}

	if ((optind+1)< argc) {
		std::cerr << "Only one output file possible. Ignore extra arguments" << std::endl;
	}

	if (input_files.size() > 1) {
		std::cerr << "Only one input file possible. Ignore extra arguments" << std::endl;
	}

	if (input_files.size() == 0) {
		std::cout << "No input file given." << std::endl;
		return 0;
	}

	Firrtlator::Firrtlator firrtlator;

	if (!firrtlator.parseFile(input_files[0], "fir")) {
		std::cout << "Failed parsing " << input_files[0] << std::endl;
	}

	firrtlator.generate(output_file, "fir");
}
