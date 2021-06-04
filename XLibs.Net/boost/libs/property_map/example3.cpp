
#include <boost/vector_property_map.hpp>
#include <string>
#include <iostream>

int main()
{
    boost::vector_property_map<std::string> m;
    
    // Assign string to '4'. 
    m[4] = "e";
    std::cout << "'" << m[4] << "'\n";
    
    // Grab string from '10'. Since none is associated,
    // "" will be returned.
    std::cout << "'" << m[10] << "'\n";
}
