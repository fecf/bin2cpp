// ref. https://mort.coffee/home/fast-cpp-embeds/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

std::string make_identifier(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::isalnum(c) ? c : '_'; });
  return result;
}

static void usage(const char* argv0) {
  printf("Usage: %s [options] [infile|\"stdin\"] [outfile|\"stdout\"]\n",
         argv0);
  puts(
      "Options:\n"
      "\t-h, --help                  Show this help text\n"
      "\t--no-const                  Output mutable variables instead of "
      "consts\n"
      "\t--always-escape             Unconditionally escape every character\n"
      "\t-l, --line-length <length>  Specify how long a line should be\n"
      "\t-i, --ident <ident>         Overwrite the identifier instead of using "
      "the file name");
}

int main(int argc, char** argv) {
  std::string ident = "";
  std::string conststr = "const ";
  bool alwaysescape = false;
  size_t maxlength = 100;
  bool use_stdin = false;
  bool use_stdout = false;

  int argidx = 1;
  for (; argidx < argc; ++argidx) {
    std::string arg = argv[argidx];
    if (arg[0] != '-' || arg == "--") {
      break;
    }

    if (arg == "--help" || arg == "-h") {
      usage(argv[0]);
      return 0;
    } else if (arg == "--no-const") {
      conststr = "";
    } else if (arg == "--always-escape") {
      alwaysescape = true;
    } else if (arg == "--line-length" || arg == "-l") {
      if (argidx + 1 >= argc) {
        std::cerr << arg << " requires an argument" << std::endl;
        return 1;
      }
      maxlength = (size_t)atoi(argv[++argidx]);
    } else if (arg == "--ident" || arg == "-i") {
      if (argidx + 1 >= argc) {
        std::cerr << arg << " requires an argument" << std::endl;
        return 1;
      }
      ident = make_identifier(argv[++argidx]);
    } else {
      std::cerr << "Unknown option: " << arg << std::endl;
      usage(argv[0]);
      return 1;
    }
  }

  if (!(argc - argidx == 1 || argc - argidx == 2)) {
    std::cerr << "Unexpected number of non-option arguments: "
              << (argc - argidx) << std::endl;
    return 1;
  }

  std::string inp = argv[argidx];
  std::string outp = argv[argidx + 1];

  std::istream& inf =
      (inp == "stdin")
          ? std::cin
          : (std::istream&)std::ifstream(inp, std::ios::in | std::ios::binary);
  if (inf.fail()) {
    std::cerr << inp;
    return 1;
  }

  std::ostream& outf = (outp == "stdout")
                           ? std::cout
                           : (std::ostream&)std::ofstream(outp, std::ios::out);
  if (outf.fail()) {
    std::cerr << outp;
    return 1;
  }

  if (ident.empty()) {
    ident = make_identifier(inp);
  }

  outf << "#ifdef __INTELLISENSE__" << std::endl;
  outf << conststr << "unsigned char " << ident << "[];" << std::endl;
  outf << conststr << "unsigned int " << ident << "_len;" << std::endl;
  outf << "#else" << std::endl;
  outf << conststr << "unsigned char " << ident << "[] =" << std::endl << "\t\"";
  std::string buffer(maxlength + 4, '\0');
  size_t linechar = 0;
  size_t length = 0;
  char c;
  while (inf.get(c)) {
    if (alwaysescape) {
      buffer[linechar++] = '\\';
      buffer[linechar++] = '0' + ((c & 0700) >> 6);
      buffer[linechar++] = '0' + ((c & 0070) >> 3);
      buffer[linechar++] = '0' + ((c & 0007) >> 0);
    } else if (c >= 32 && c <= 126 && c != '"' && c != '\\' && c != '?' &&
               c != ':' && c != '%') {
      buffer[linechar++] = (char)c;
    } else if (c == '\r') {
      buffer[linechar++] = '\\';
      buffer[linechar++] = 'r';
    } else if (c == '\n') {
      buffer[linechar++] = '\\';
      buffer[linechar++] = 'n';
    } else if (c == '\t') {
      buffer[linechar++] = '\\';
      buffer[linechar++] = 't';
    } else if (c == '\"') {
      buffer[linechar++] = '\\';
      buffer[linechar++] = '"';
    } else if (c == '\\') {
      buffer[linechar++] = '\\';
      buffer[linechar++] = '\\';
    } else {
      buffer[linechar++] = '\\';
      buffer[linechar++] = '0' + ((c & 0700) >> 6);
      buffer[linechar++] = '0' + ((c & 0070) >> 3);
      buffer[linechar++] = '0' + ((c & 0007) >> 0);
    }
    length += 1;

    if (linechar >= maxlength) {
      outf.write(buffer.data(), linechar);
      outf << "\"" << std::endl << "\t\"";
      linechar = 0;
    }
  }

  if (linechar >= 1) {
    outf.write(buffer.data(), linechar);
  }

  outf << "\";" << std::endl
       << conststr << "unsigned int " << ident << "_len = " << length << ";"
       << std::endl;
  outf << "#endif" << std::endl;

  return 0;
}
