// Copyright (C) 2014 by Sebastian Lipponer.
// 
// This code is free software: you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this code. If not, see <http://www.gnu.org/licenses/>.

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
