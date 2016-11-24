#include <IR.h>
#include <Firrtlator.h>

#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>

void help(void);

int main(int argc, char* argv[]) {
	int c;

	std::vector<std::string> input_files;
	std::vector<std::string> passes;
	std::string output_file = "out.fir";

	while ((c = getopt (argc, argv, "hi:p:")) != -1) {
		switch(c) {
		case 'i':
			input_files.push_back(optarg);
			break;
		case 'p':
			passes.push_back(optarg);
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

	std::string::size_type pos = input_files[0].find_last_of(".");
	if (pos == std::string::npos) {
		std::cout << "Cannot determine the output file type" << std::endl;
		exit(1);
	}

	std::string ext = input_files[0].substr(pos+1, -1);

	if (!firrtlator.parseFile(input_files[0], firrtlator.getFrontend(ext))) {
		std::cout << "Failed parsing " << input_files[0] << std::endl;
	}

	for (auto p : passes) {
		firrtlator.pass(p);
	}

	pos = output_file.find_last_of(".");
	if (pos == std::string::npos) {
		std::cout << "Cannot determine the output file type" << std::endl;
		exit(1);
	}

	ext = output_file.substr(pos+1, -1);

	firrtlator.generate(output_file, firrtlator.getBackend(ext));
}

void help(void) {
	std::cout << "Usage: firrtlator [options] <output>" << std::endl;
	std::cout << "  <output> is an output file name. The extension hints used backend (see below)." << std::endl;
	std::cout << std::endl;
	std::cout << "  options:" << std::endl;
	std::cout << "   -i <input>     Set input file. Currently only one file is supported." << std::endl;
	std::cout << std::endl;

	std::vector<::Firrtlator::Firrtlator::FrontendDescriptor> fdesc;
	fdesc = ::Firrtlator::Firrtlator::getFrontends();
	std::cout << "Supported frontends:" << std::endl;
	for (auto f : fdesc) {
		std::cout << "  " << f.name << std::endl;
		std::cout << "    " << f.description << std::endl;
		std::cout << "    Filetypes:";
		for (auto t : f.filetypes) {
			std::cout << " " << t;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::vector<::Firrtlator::Firrtlator::BackendDescriptor> bdesc;
	bdesc = ::Firrtlator::Firrtlator::getBackends();
	std::cout << "Supported backends:" << std::endl;
	for (auto b : bdesc) {
		std::cout << "  " << b.name << std::endl;
		std::cout << "    " << b.description << std::endl;
		std::cout << "    Filetypes:";
		for (auto t : b.filetypes) {
			std::cout << " " << t;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::vector<::Firrtlator::Firrtlator::PassDescriptor> pdesc;
	pdesc = ::Firrtlator::Firrtlator::getPasses();
	std::cout << "Supported passes:" << std::endl;
	for (auto p : pdesc) {
		std::cout << "  " << p.name << std::endl;
		std::cout << "    " << p.description << std::endl;
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
