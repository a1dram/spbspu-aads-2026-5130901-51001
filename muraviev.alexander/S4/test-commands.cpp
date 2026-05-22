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

BOOST_AUTO_TEST_CASE(test_commands_load_datasets_reads_pairs)
{
  const std::string filename = "test_input_s4.txt";
  {
    std::ofstream output(filename.c_str());
    output << "first 1 name 2 surname\n";
    output << "second 4 mouse 1 name 2 keyboard\n";
  }

  muraviev::DatasetTable datasets;

  BOOST_TEST(muraviev::loadDatasets(filename, datasets));
  BOOST_TEST(datasets.size() == 2);
  BOOST_TEST(datasets.get("first").get(1) == "name");
  BOOST_TEST(datasets.get("second").get(2) == "keyboard");

  std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_commands_load_datasets_rejects_bad_pairs)
{
  const std::string filename = "test_bad_input_s4.txt";
  {
    std::ofstream output(filename.c_str());
    output << "first 1 name 2\n";
  }

  muraviev::DatasetTable datasets;

  BOOST_TEST(!muraviev::loadDatasets(filename, datasets));

  std::remove(filename.c_str());
}
