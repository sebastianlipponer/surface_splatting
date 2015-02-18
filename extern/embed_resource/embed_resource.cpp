// Copyright(c) 2014 Sebastian Lipponer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cstdint>
#include <algorithm>

inline void
write_hex(std::ofstream& out, uint8_t c)
{
    const char base16_digits[] = "0123456789abcdef";
    
    out << "0x";
    if (c < 16)
    {
        out << "0";
        out << base16_digits[c];
    }
    else
    {
        out << base16_digits[c >> 4];
        out << base16_digits[c & 0xf];
    }
}

int
main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "USAGE: " << argv[0] << " {sym} {rsrc}\n\n"
                  << "  Creates {sym}.cpp from the contents of {rsrc}\n"
                  << std::endl;

        return EXIT_FAILURE;
    }

    std::string sym(argv[1]), rsrc(argv[2]);

    std::ifstream in(rsrc, std::ifstream::binary);
    if (in.fail())
    {
        std::cerr << "Could not open file " << rsrc
                  << " for reading." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::ofstream out(sym + ".cpp");
    if (out.fail())
    {
        std::cerr << "Could not open file " << sym
                  << " for writing." << std::endl;
        exit(EXIT_FAILURE);
    }

    out << "#include <cstdlib>\n\n";
    out << "extern const unsigned char " << sym << "[] = {";

    in.seekg(0, in.end);
    std::size_t size = in.tellg();
    in.seekg(0, in.beg);

    char buf[1024];
    std::size_t i(0);

    while (size > 0)
    {
		std::size_t n = std::min(size, static_cast<std::size_t>(1024));
        in.read(buf, n);
        size = size - n;

        if (in.fail())
        {
            std::cerr << "Could not read from file." << std::endl;
            exit(EXIT_FAILURE);
        }

        for (unsigned int j(0); j < static_cast<unsigned int>(n); ++j)
        {
            if (i % 12 == 0)
            {
                out << "\n    ";
            }

            write_hex(out, *reinterpret_cast<uint8_t*>(&buf[j]));
            out << ", ";
            
            ++i;
        }
    }

    out << "0x0 };\n\n";
    out << "const std::size_t " << sym << "_len = sizeof(" << sym << ");\n\n";

    in.close();
    out.close();

    return EXIT_SUCCESS;
}
