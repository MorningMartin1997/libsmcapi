//
// Created by User on 2021/5/10.
//

#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "../library.h"

unsigned int Factorial( unsigned int number ) {
    return 1;
}

TEST_CASE("Test hello world", "[factorial]"){
    REQUIRE( hello() == 0 );
}