#include "commands.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "parsing.hpp"

namespace
{
  using Tokens = std::vector< std::string >;
  using GraphTable = muraviev::GraphTable;
  using CommandHandler = bool (*)(GraphTable&, const Tokens&, std::ostream&);
  using CommandTable = muraviev::HashTable< std::string, CommandHandler,
      muraviev::StringHash, muraviev::StringEqual >;

  struct EdgeRow
  {
    std::string vertex;
    std::vector< unsigned long long > weights;
  };

  bool edgeRowLess(const EdgeRow& lhs, const EdgeRow& rhs)
  {
    return lhs.vertex < rhs.vertex;
  }

  bool validateNames(const Tokens& tokens, size_t begin)
  {
    for (size_t i = begin; i < tokens.size(); ++i) {
      if (!muraviev::isValidName(tokens[i])) {
        return false;
      }
    }
    return true;
  }

  std::vector< unsigned long long > collectSortedWeights(
      const muraviev::Graph::WeightList& weights)
  {
    std::vector< unsigned long long > result;
    for (muraviev::Graph::WeightList::c_iter it = weights.begin(); it != weights.end(); ++it) {
      result.push_back(*it);
    }
    std::sort(result.begin(), result.end());
    return result;
  }

  void printRows(std::ostream& output, std::vector< EdgeRow >& rows)
  {
    std::sort(rows.begin(), rows.end(), edgeRowLess);
    for (size_t i = 0; i < rows.size(); ++i) {
      output << rows[i].vertex;
      for (size_t j = 0; j < rows[i].weights.size(); ++j) {
        output << ' ' << rows[i].weights[j];
      }
      output << '\n';
    }
  }

  void copyGraphData(const muraviev::Graph& source, muraviev::Graph& target)
  {
    const std::vector< std::string > vertexes = source.collectVertexNames();
    for (size_t i = 0; i < vertexes.size(); ++i) {
      target.addVertex(vertexes[i]);
    }

    const std::vector< muraviev::EdgeKey > edges = source.collectEdgeKeys();
    for (size_t i = 0; i < edges.size(); ++i) {
      const muraviev::Graph::WeightList& weights = source.getWeights(edges[i]);
      for (muraviev::Graph::WeightList::c_iter it = weights.begin(); it != weights.end(); ++it) {
        target.addEdge(edges[i].from, edges[i].to, *it);
      }
    }
  }

  bool graphsCommand(GraphTable& graphs, const Tokens& tokens, std::ostream& output)
  {
    if (tokens.size() != 1) {
      return false;
    }

    std::vector< std::string > names;
    for (GraphTable::const_iterator it = graphs.cbegin(); it != graphs.cend(); ++it) {
      names.push_back(it->key);
    }
    std::sort(names.begin(), names.end());
    for (size_t i = 0; i < names.size(); ++i) {
      output << names[i] << '\n';
    }
    return true;
  }

  bool vertexesCommand(GraphTable& graphs, const Tokens& tokens, std::ostream& output)
  {
    if (tokens.size() != 2 || !validateNames(tokens, 1) || !graphs.has(tokens[1])) {
      return false;
    }

    const std::vector< std::string > vertexes = graphs.at(tokens[1]).collectVertexNames();
    for (size_t i = 0; i < vertexes.size(); ++i) {
      output << vertexes[i] << '\n';
    }
    return true;
  }

  bool outboundCommand(GraphTable& graphs, const Tokens& tokens, std::ostream& output)
  {
    if (tokens.size() != 3 || !validateNames(tokens, 1) || !graphs.has(tokens[1])) {
      return false;
    }

    const muraviev::Graph& graph = graphs.at(tokens[1]);
    if (!graph.hasVertex(tokens[2])) {
      return false;
    }

    std::vector< EdgeRow > rows;
    const std::vector< muraviev::EdgeKey > edges = graph.collectEdgeKeys();
    for (size_t i = 0; i < edges.size(); ++i) {
      if (edges[i].from == tokens[2]) {
        EdgeRow row = {edges[i].to, collectSortedWeights(graph.getWeights(edges[i]))};
        rows.push_back(row);
      }
    }
    printRows(output, rows);
    return true;
  }

  bool inboundCommand(GraphTable& graphs, const Tokens& tokens, std::ostream& output)
  {
    if (tokens.size() != 3 || !validateNames(tokens, 1) || !graphs.has(tokens[1])) {
      return false;
    }

    const muraviev::Graph& graph = graphs.at(tokens[1]);
    if (!graph.hasVertex(tokens[2])) {
      return false;
    }

    std::vector< EdgeRow > rows;
    const std::vector< muraviev::EdgeKey > edges = graph.collectEdgeKeys();
    for (size_t i = 0; i < edges.size(); ++i) {
      if (edges[i].to == tokens[2]) {
        EdgeRow row = {edges[i].from, collectSortedWeights(graph.getWeights(edges[i]))};
        rows.push_back(row);
      }
    }
    printRows(output, rows);
    return true;
  }

  bool bindCommand(GraphTable& graphs, const Tokens& tokens, std::ostream&)
  {
    unsigned long long weight = 0;
    if (tokens.size() != 5 || !muraviev::isValidName(tokens[1]) ||
        !muraviev::isValidName(tokens[2]) || !muraviev::isValidName(tokens[3]) ||
        !graphs.has(tokens[1]) ||
        !muraviev::parseUnsignedLongLong(tokens[4], weight)) {
      return false;
    }

    graphs.at(tokens[1]).addEdge(tokens[2], tokens[3], weight);
    return true;
  }

  bool cutCommand(GraphTable& graphs, const Tokens& tokens, std::ostream&)
  {
    unsigned long long weight = 0;
    if (tokens.size() != 5 || !muraviev::isValidName(tokens[1]) ||
        !muraviev::isValidName(tokens[2]) || !muraviev::isValidName(tokens[3]) ||
        !graphs.has(tokens[1]) ||
        !muraviev::parseUnsignedLongLong(tokens[4], weight)) {
      return false;
    }

    return graphs.at(tokens[1]).removeEdge(tokens[2], tokens[3], weight);
  }

  bool createCommand(GraphTable& graphs, const Tokens& tokens, std::ostream&)
  {
    if ((tokens.size() != 2 && tokens.size() < 3) || !muraviev::isValidName(tokens[1]) ||
        graphs.has(tokens[1])) {
      return false;
    }

    muraviev::Graph graph(tokens[1]);
    if (tokens.size() > 2) {
      size_t count = 0;
      if (!muraviev::parseSize(tokens[2], count) || tokens.size() != count + 3 ||
          !validateNames(tokens, 3)) {
        return false;
      }
      for (size_t i = 0; i < count; ++i) {
        graph.addVertex(tokens[i + 3]);
      }
    }

    graphs.add(tokens[1], graph);
    return true;
  }

  bool mergeCommand(GraphTable& graphs, const Tokens& tokens, std::ostream&)
  {
    if (tokens.size() != 4 || !validateNames(tokens, 1) || graphs.has(tokens[1]) ||
        !graphs.has(tokens[2]) || !graphs.has(tokens[3])) {
      return false;
    }

    muraviev::Graph merged(tokens[1]);
    copyGraphData(graphs.at(tokens[2]), merged);
    copyGraphData(graphs.at(tokens[3]), merged);
    graphs.add(tokens[1], merged);
    return true;
  }

  bool extractCommand(GraphTable& graphs, const Tokens& tokens, std::ostream&)
  {
    if (tokens.size() < 4 || !muraviev::isValidName(tokens[1]) ||
        !muraviev::isValidName(tokens[2]) || graphs.has(tokens[1]) ||
        !graphs.has(tokens[2])) {
      return false;
    }

    size_t count = 0;
    if (!muraviev::parseSize(tokens[3], count) || tokens.size() != count + 4 ||
        !validateNames(tokens, 4)) {
      return false;
    }

    const muraviev::Graph& oldGraph = graphs.at(tokens[2]);
    muraviev::HashTable< std::string, bool, muraviev::StringHash,
        muraviev::StringEqual > selected(53);
    muraviev::Graph extracted(tokens[1]);
    for (size_t i = 0; i < count; ++i) {
      const std::string& vertex = tokens[i + 4];
      if (!oldGraph.hasVertex(vertex)) {
        return false;
      }
      selected.add(vertex, true);
      extracted.addVertex(vertex);
    }

    const std::vector< muraviev::EdgeKey > edges = oldGraph.collectEdgeKeys();
    for (size_t i = 0; i < edges.size(); ++i) {
      if (selected.has(edges[i].from) && selected.has(edges[i].to)) {
        const muraviev::Graph::WeightList& weights = oldGraph.getWeights(edges[i]);
        for (muraviev::Graph::WeightList::c_iter it = weights.begin();
            it != weights.end(); ++it) {
          extracted.addEdge(edges[i].from, edges[i].to, *it);
        }
      }
    }

    graphs.add(tokens[1], extracted);
    return true;
  }

  CommandTable createCommandTable()
  {
    CommandTable commands(17);
    commands.add("graphs", graphsCommand);
    commands.add("vertexes", vertexesCommand);
    commands.add("outbound", outboundCommand);
    commands.add("inbound", inboundCommand);
    commands.add("bind", bindCommand);
    commands.add("cut", cutCommand);
    commands.add("create", createCommand);
    commands.add("merge", mergeCommand);
    commands.add("extract", extractCommand);
    return commands;
  }
}

bool muraviev::loadGraphs(const std::string& filename, GraphTable& graphs)
{
  std::ifstream input(filename.c_str());
  if (!input) {
    return false;
  }

  std::string line;
  while (std::getline(input, line)) {
    if (line.empty()) {
      continue;
    }

    Tokens tokens;
    if (!splitStrictSpaces(line, tokens) || tokens.size() != 2 || !isValidName(tokens[0]) ||
        graphs.has(tokens[0])) {
      return false;
    }

    size_t edgesCount = 0;
    if (!parseSize(tokens[1], edgesCount)) {
      return false;
    }

    Graph graph(tokens[0]);
    for (size_t i = 0; i < edgesCount; ++i) {
      if (!std::getline(input, line)) {
        return false;
      }
      if (line.empty()) {
        --i;
        continue;
      }

      Tokens edgeTokens;
      unsigned long long weight = 0;
      if (!splitStrictSpaces(line, edgeTokens) || edgeTokens.size() != 3 ||
          !isValidName(edgeTokens[0]) || !isValidName(edgeTokens[1]) ||
          !parseUnsignedLongLong(edgeTokens[2], weight)) {
        return false;
      }
      graph.addEdge(edgeTokens[0], edgeTokens[1], weight);
    }
    graphs.add(tokens[0], graph);
  }

  return true;
}

void muraviev::executeCommands(std::istream& input, std::ostream& output,
    GraphTable& graphs)
{
  CommandTable commands = createCommandTable();
  std::string line;
  while (std::getline(input, line)) {
    if (line.empty()) {
      continue;
    }

    Tokens tokens;
    if (!splitStrictSpaces(line, tokens) || tokens.empty() || !commands.has(tokens[0])) {
      output << "INVALID COMMAND\n";
      continue;
    }

    const CommandHandler handler = commands.at(tokens[0]);
    if (!handler(graphs, tokens, output)) {
      output << "INVALID COMMAND\n";
    }
  }
}
