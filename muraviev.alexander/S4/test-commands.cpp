#include <boost/test/unit_test.hpp>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "commands.hpp"

BOOST_AUTO_TEST_CASE(test_commands_load_datasets_reads_file)
{
  const std::string filename = "test_input_s4.txt";
  {
    std::ofstream output(filename.c_str());
    output << "first 1 name 2 surname\n";
    output << "second 4 mouse 1 name 2 keyboard\n";
    output << "empty\n";
  }

  muraviev::DatasetTable datasets;

  BOOST_TEST(muraviev::loadDatasets(filename, datasets));
  BOOST_TEST(datasets.size() == 3);
  BOOST_TEST(datasets.contains("first"));
  BOOST_TEST(datasets.get("first").get(1) == "name");
  BOOST_TEST(datasets.get("second").get(2) == "keyboard");
  BOOST_TEST(datasets.get("empty").empty());

  std::remove(filename.c_str());
}

BOOST_AUTO_TEST_CASE(test_commands_load_datasets_rejects_invalid_file)
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

BOOST_AUTO_TEST_CASE(test_commands_print_and_invalid)
{
  muraviev::DatasetTable datasets;
  muraviev::Dataset first;
  std::istringstream input("print first\nprint empty\nprint missing\nunknown\n");
  std::ostringstream output;

  first.push(2, "surname");
  first.push(1, "name");
  datasets.push("first", first);
  datasets.push("empty", muraviev::Dataset());

  muraviev::executeCommands(input, output, datasets);

  BOOST_TEST(output.str() ==
      "first 1 name 2 surname\n"
      "<EMPTY>\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_CASE(test_commands_complement_intersect_union)
{
  muraviev::DatasetTable datasets;
  muraviev::Dataset first;
  muraviev::Dataset second;
  std::istringstream input(
      "complement third second first\n"
      "print third\n"
      "intersect fourth first second\n"
      "print fourth\n"
      "intersect yafourth second first\n"
      "print yafourth\n"
      "union fifth first second\n"
      "print fifth\n"
      "union yafifth second first\n"
      "print yafifth\n"
      "union first first second\n"
      "union bad first missing\n");
  std::ostringstream output;

  first.push(1, "name");
  first.push(2, "surname");
  second.push(4, "mouse");
  second.push(1, "name");
  second.push(2, "keyboard");
  datasets.push("first", first);
  datasets.push("second", second);

  muraviev::executeCommands(input, output, datasets);

  BOOST_TEST(output.str() ==
      "third 4 mouse\n"
      "fourth 1 name 2 surname\n"
      "yafourth 1 name 2 keyboard\n"
      "fifth 1 name 2 surname 4 mouse\n"
      "yafifth 1 name 2 keyboard 4 mouse\n"
      "<INVALID COMMAND>\n"
      "<INVALID COMMAND>\n");
}
