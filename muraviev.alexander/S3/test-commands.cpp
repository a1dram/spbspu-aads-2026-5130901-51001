#include <boost/test/unit_test.hpp>

#include <sstream>
#include <string>

#include "commands.hpp"

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_commands_graphs_and_vertexes)
{
  GraphTable graphs(17);
  std::istringstream input("graphs\nvertexes gr1\n");
  std::ostringstream output;
  Graph graph("gr1");

  graph.addEdge("b", "c", 50);
  graph.addEdge("a", "b", 40);
  graphs.add("gr1", graph);

  executeCommands(input, output, graphs);

  BOOST_TEST(output.str() == "gr1\na\nb\nc\n");
}

BOOST_AUTO_TEST_CASE(test_commands_outbound_and_inbound)
{
  GraphTable graphs(17);
  std::istringstream input("outbound gr1 c\ninbound gr1 b\n");
  std::ostringstream output;
  Graph graph("gr1");

  graph.addEdge("a", "b", 40);
  graph.addEdge("c", "a", 30);
  graph.addEdge("c", "b", 20);
  graphs.add("gr1", graph);

  executeCommands(input, output, graphs);

  BOOST_TEST(output.str() == "a 30\nb 20\na 40\nc 20\n");
}

BOOST_AUTO_TEST_CASE(test_commands_bind_and_cut)
{
  GraphTable graphs(17);
  std::istringstream input("bind gr2 b d 200\ncut gr2 b d 200\noutbound gr2 b\n");
  std::ostringstream output;
  Graph graph("gr2");

  graph.addEdge("b", "b", 2);
  graphs.add("gr2", graph);

  executeCommands(input, output, graphs);

  BOOST_TEST(output.str() == "b 2\n");
}

BOOST_AUTO_TEST_CASE(test_commands_create_rejects_existing_graph)
{
  GraphTable graphs(17);
  std::istringstream input("create gr3\ncreate gr3\n");
  std::ostringstream output;

  executeCommands(input, output, graphs);

  BOOST_TEST(output.str() == "INVALID COMMAND\n");
}

BOOST_AUTO_TEST_CASE(test_commands_merge_duplicates_same_graph_edges)
{
  GraphTable graphs(17);
  std::istringstream input("merge gr3 gr2 gr2\ninbound gr3 b\n");
  std::ostringstream output;
  Graph graph("gr2");

  graph.addEdge("a", "b", 1);
  graph.addEdge("b", "b", 2);
  graphs.add("gr2", graph);

  executeCommands(input, output, graphs);

  BOOST_TEST(output.str() == "a 1 1\nb 2 2\n");
}

BOOST_AUTO_TEST_CASE(test_commands_extract_keeps_only_selected_vertexes)
{
  GraphTable graphs(17);
  std::istringstream input("extract gr3 gr2 2 a c\noutbound gr3 a\n");
  std::ostringstream output;
  Graph graph("gr2");

  graph.addEdge("a", "b", 1);
  graph.addEdge("a", "c", 3);
  graph.addEdge("c", "a", 4);
  graphs.add("gr2", graph);

  executeCommands(input, output, graphs);

  BOOST_TEST(output.str() == "c 3\n");
}

BOOST_AUTO_TEST_CASE(test_commands_unknown_command_is_invalid)
{
  GraphTable graphs(17);
  std::istringstream input("unknown\n");
  std::ostringstream output;

  executeCommands(input, output, graphs);

  BOOST_TEST(output.str() == "INVALID COMMAND\n");
}
