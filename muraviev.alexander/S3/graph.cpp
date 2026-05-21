#include "graph.hpp"

#include <algorithm>

#include "sip_hash.hpp"

size_t muraviev::StringHash::operator()(const std::string& value) const
{
  const unsigned char* data = reinterpret_cast< const unsigned char* >(value.data());
  return static_cast< size_t >(sipHash24(data, value.size()));
}

bool muraviev::StringEqual::operator()(const std::string& lhs,
    const std::string& rhs) const
{
  return lhs == rhs;
}

muraviev::EdgeKey::EdgeKey():
  from(),
  to()
{}

muraviev::EdgeKey::EdgeKey(const std::string& edgeFrom, const std::string& edgeTo):
  from(edgeFrom),
  to(edgeTo)
{}

size_t muraviev::EdgeKeyHash::operator()(const EdgeKey& value) const
{
  std::string bytes = value.from;
  bytes += '\0';
  bytes += value.to;
  const unsigned char* data = reinterpret_cast< const unsigned char* >(bytes.data());
  return static_cast< size_t >(sipHash24(data, bytes.size()));
}

bool muraviev::EdgeKeyEqual::operator()(const EdgeKey& lhs, const EdgeKey& rhs) const
{
  return lhs.from == rhs.from && lhs.to == rhs.to;
}

muraviev::Graph::Graph(const std::string& name):
  name_(name),
  edges_(53),
  vertexes_(53)
{}

const std::string& muraviev::Graph::getName() const
{
  return name_;
}

void muraviev::Graph::addVertex(const std::string& vertex)
{
  vertexes_.add(vertex, true);
}

bool muraviev::Graph::hasVertex(const std::string& vertex) const
{
  return vertexes_.has(vertex);
}

void muraviev::Graph::addEdge(const std::string& from, const std::string& to,
    unsigned long long weight)
{
  addVertex(from);
  addVertex(to);

  const EdgeKey key(from, to);
  if (edges_.has(key)) {
    WeightList weights = edges_.at(key);
    if (weights.empty()) {
      weights.pushFront(weight);
    } else {
      weights.insert(weights.last(), weight);
    }
    edges_.add(key, weights);
  } else {
    WeightList weights;
    weights.pushFront(weight);
    edges_.add(key, weights);
  }
}

bool muraviev::Graph::removeEdge(const std::string& from, const std::string& to,
    unsigned long long weight)
{
  if (!hasVertex(from) || !hasVertex(to)) {
    return false;
  }

  const EdgeKey key(from, to);
  if (!edges_.has(key)) {
    return false;
  }

  WeightList weights = edges_.at(key);
  for (WeightList::iter it = weights.begin(); it != weights.end(); ++it) {
    if (*it == weight) {
      weights.erase(it);
      if (weights.empty()) {
        edges_.drop(key);
      } else {
        edges_.add(key, weights);
      }
      return true;
    }
  }

  return false;
}

std::vector< std::string > muraviev::Graph::collectVertexNames() const
{
  std::vector< std::string > result;
  for (VertexTable::const_iterator it = vertexes_.cbegin(); it != vertexes_.cend(); ++it) {
    result.push_back(it->key);
  }
  std::sort(result.begin(), result.end());
  return result;
}

std::vector< muraviev::EdgeKey > muraviev::Graph::collectEdgeKeys() const
{
  std::vector< EdgeKey > result;
  for (EdgeTable::const_iterator it = edges_.cbegin(); it != edges_.cend(); ++it) {
    result.push_back(it->key);
  }
  return result;
}

const muraviev::Graph::WeightList& muraviev::Graph::getWeights(const EdgeKey& key) const
{
  return edges_.at(key);
}
