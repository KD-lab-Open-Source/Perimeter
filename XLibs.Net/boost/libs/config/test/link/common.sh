#
# locate all the header dependencies:
header=""
#
# locate all the source files:
src=link_test.cpp

boost_version=$(grep 'define.*BOOST_LIB_VERSION' ../../../boost/version.hpp | sed 's/.*"\([^"]*\)".*/\1/')




