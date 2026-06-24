#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "../common/list.hpp"
#include "hash_table.hpp"

namespace muraviev
{
  struct StringHash
  {
    size_t operator()(const std::string& value) const;
  };

  struct StringEqual
  {
    bool operator()(const std::string& lhs, const std::string& rhs) const;
  };

  struct EdgeKey
  {
    std::string from;
    std::string to;

    EdgeKey();
    EdgeKey(const std::string& edgeFrom, const std::string& edgeTo);
  };

  struct EdgeKeyHash
  {
    size_t operator()(const EdgeKey& value) const;
  };

  struct EdgeKeyEqual
  {
    bool operator()(const EdgeKey& lhs, const EdgeKey& rhs) const;
  };

  class Graph
  {
  public:
    using WeightList = List< unsigned long long >;
    using EdgeTable = HashTable< EdgeKey, WeightList, EdgeKeyHash, EdgeKeyEqual >;
    using VertexTable = HashTable< std::string, bool, StringHash, StringEqual >;

    explicit Graph(const std::string& name = "");

    const std::string& getName() const;
    void addVertex(const std::string& vertex);
    bool hasVertex(const std::string& vertex) const;
    void addEdge(const std::string& from, const std::string& to, unsigned long long weight);
    bool removeEdge(const std::string& from, const std::string& to,
        unsigned long long weight);

    std::vector< std::string > collectVertexNames() const;
    std::vector< EdgeKey > collectEdgeKeys() const;
    const WeightList& getWeights(const EdgeKey& key) const;

  private:
    std::string name_;
    EdgeTable edges_;
    VertexTable vertexes_;
  };
}

#endif
