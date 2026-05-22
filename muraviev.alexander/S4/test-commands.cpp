#include <boost/test/unit_test.hpp>

#include <cstdio>
#include <fstream>
#include <string>

#include "commands.hpp"

BOOST_AUTO_TEST_CASE(test_commands_load_empty_datasets)
{
  const std::string filename = "test_input_s4.txt";
  {
    std::ofstream output(filename.c_str());
    output << "first\n";
    output << "empty\n";
  }

  muraviev::DatasetTable datasets;

  BOOST_TEST(muraviev::loadDatasets(filename, datasets));
  BOOST_TEST(datasets.size() == 2);
  BOOST_TEST(datasets.contains("first"));
  BOOST_TEST(datasets.get("empty").empty());

  std::remove(filename.c_str());
}
