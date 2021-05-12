//
// Created by User on 2021/5/10.
//

#define CATCH_CONFIG_MAIN
#include "catch2/catch_all.hpp"
#include "../library.h"
#include "../examples/example.h"
#include <boost/thread/thread.hpp>


TEST_CASE("Test hello world", "[helloWorld]"){
    REQUIRE( hello() == 0 );
}

TEST_CASE("Test Basic Communication", "[communication]"){
    boost::thread t1([]() {Comm("1", "CommConfig.txt");});
    boost::thread t2([]() {Comm("2", "CommConfig.txt");});
    t1.join();
    t2.join();
    REQUIRE( hello() == 0);
}