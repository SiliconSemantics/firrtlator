#include <IR.h>
#include <Firrtlator.h>

#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>

void help() {
	std::cout << "Usage: firrtlator [options] <output>" << std::endl;
	std::cout << "  <output> is an output file name. The extension hints used backend (see below)." << std::endl;
	std::cout << std::endl;
	std::cout << "  options:" << std::endl;
	std::cout << "   -i <input>     Set input file. Currently only one file is supported." << std::endl;
	std::cout << std::endl;

	std::vector<::Firrtlator::Firrtlator::BackendDescriptor> desc;
	desc = ::Firrtlator::Firrtlator::getBackends();
	std::cout << "Supported backends:" << std::endl;
	for (auto b : desc) {
		std::cout << "  " << b.name << std::endl;
		std::cout << "    " << b.description << std::endl;
		std::cout << "    Filetypes:";
		for (auto t : b.filetypes) {
			std::cout << " " << t;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int main(int argc, char* argv[]) {
	int c, idx;

	std::vector<std::string> input_files;
	std::string output_file = "out.fir";

	while ((c = getopt (argc, argv, "hi:")) != -1) {
		switch(c) {
		case 'i':
			input_files.push_back(optarg);
			break;
		case 'h':
			help();
			exit(0);
			break;
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
