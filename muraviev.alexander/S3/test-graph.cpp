#include <boost/test/unit_test.hpp>

#include <vector>

#include "graph.hpp"

using namespace muraviev;

BOOST_AUTO_TEST_CASE(test_graph_adds_vertexes_with_edge)
{
  Graph graph("gr");

  graph.addEdge("a", "b", 10);

  BOOST_TEST(graph.hasVertex("a"));
  BOOST_TEST(graph.hasVertex("b"));
}

BOOST_AUTO_TEST_CASE(test_graph_stores_multiple_weights_for_edge)
{
  Graph graph("gr");
  EdgeKey key("a", "b");
  size_t count = 0;

  graph.addEdge("a", "b", 10);
  graph.addEdge("a", "b", 20);

  const Graph::WeightList& weights = graph.getWeights(key);
  for (Graph::WeightList::c_iter it = weights.begin(); it != weights.end(); ++it) {
    ++count;
  }

  BOOST_TEST(count == 2);
}

BOOST_AUTO_TEST_CASE(test_graph_removes_one_weight)
{
  Graph graph("gr");
  EdgeKey key("a", "b");
  size_t count = 0;

  graph.addEdge("a", "b", 10);
  graph.addEdge("a", "b", 10);

  BOOST_TEST(graph.removeEdge("a", "b", 10));

  const Graph::WeightList& weights = graph.getWeights(key);
  for (Graph::WeightList::c_iter it = weights.begin(); it != weights.end(); ++it) {
    ++count;
  }

  BOOST_TEST(count == 1);
}

BOOST_AUTO_TEST_CASE(test_graph_removes_edge_key_when_last_weight_deleted)
{
  Graph graph("gr");
  EdgeKey key("a", "b");

  graph.addEdge("a", "b", 10);

  BOOST_TEST(graph.removeEdge("a", "b", 10));
  BOOST_CHECK_THROW(graph.getWeights(key), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_graph_collects_sorted_vertexes)
{
  Graph graph("gr");
  std::vector< std::string > vertexes;

  graph.addVertex("c");
  graph.addVertex("a");
  graph.addVertex("b");
  vertexes = graph.collectVertexNames();

  BOOST_TEST(vertexes.size() == 3);
  BOOST_TEST(vertexes[0] == "a");
  BOOST_TEST(vertexes[1] == "b");
  BOOST_TEST(vertexes[2] == "c");
}
