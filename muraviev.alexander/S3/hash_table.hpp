#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <cstddef>
#include <stdexcept>

#include "../common/list.hpp"

namespace muraviev
{
  template< class Key, class Value, class Hash, class Equal >
  class HashTable
  {
  public:
    struct Entry
    {
      Key key;
      Value value;

      Entry();
      Entry(const Key& entryKey, const Value& entryValue);
    };

    class Iterator;
    class ConstIterator;

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    explicit HashTable(size_t slots = 17);
    ~HashTable();
    HashTable(const HashTable& other);
    HashTable& operator=(const HashTable& other);

    void add(const Key& key, const Value& value);
    Value drop(const Key& key);
    bool has(const Key& key) const;
    Value& at(const Key& key);
    const Value& at(const Key& key) const;
    void rehash(size_t slots);
    size_t size() const;
    bool empty() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

  private:
    using Bucket = List< Entry >;
    using BucketIter = typename Bucket::iter;
    using BucketConstIter = typename Bucket::c_iter;

    Bucket* buckets_;
    size_t bucketsCount_;
    size_t size_;
    Hash hash_;
    Equal equal_;

    size_t getIndex(const Key& key) const;
    void swap(HashTable& other);
  };

  template< class Key, class Value, class Hash, class Equal >
  class HashTable< Key, Value, Hash, Equal >::Iterator
  {
    friend class HashTable< Key, Value, Hash, Equal >;
  public:
    Iterator();

    Entry& operator*() const;
    Entry* operator->() const;

    Iterator& operator++();

    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

  private:
    Iterator(HashTable* table, size_t bucket, BucketIter iter);

    void moveToNextFilledBucket();

    HashTable* table_;
    size_t bucket_;
    BucketIter iter_;
  };

  template< class Key, class Value, class Hash, class Equal >
  class HashTable< Key, Value, Hash, Equal >::ConstIterator
  {
    friend class HashTable< Key, Value, Hash, Equal >;
  public:
    ConstIterator();
    ConstIterator(const Iterator& other);

    const Entry& operator*() const;
    const Entry* operator->() const;

    ConstIterator& operator++();

    bool operator==(const ConstIterator& other) const;
    bool operator!=(const ConstIterator& other) const;

  private:
    ConstIterator(const HashTable* table, size_t bucket, BucketConstIter iter);

    void moveToNextFilledBucket();

    const HashTable* table_;
    size_t bucket_;
    BucketConstIter iter_;
  };

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::Entry::Entry():
    key(),
    value()
  {}

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::Entry::Entry(const Key& entryKey,
      const Value& entryValue):
    key(entryKey),
    value(entryValue)
  {}

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::Iterator::Iterator():
    table_(nullptr),
    bucket_(0),
    iter_()
  {}

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::Entry&
  HashTable< Key, Value, Hash, Equal >::Iterator::operator*() const
  {
    return *iter_;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::Entry*
  HashTable< Key, Value, Hash, Equal >::Iterator::operator->() const
  {
    return &(*iter_);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::Iterator&
  HashTable< Key, Value, Hash, Equal >::Iterator::operator++()
  {
    if (table_ == nullptr || bucket_ >= table_->bucketsCount_) {
      return *this;
    }

    ++iter_;
    if (iter_ == table_->buckets_[bucket_].end()) {
      ++bucket_;
      moveToNextFilledBucket();
    }

    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::Iterator::operator==(
      const Iterator& other) const
  {
    if (table_ != other.table_ || bucket_ != other.bucket_) {
      return false;
    }
    if (table_ == nullptr || bucket_ >= table_->bucketsCount_) {
      return true;
    }
    return iter_ == other.iter_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::Iterator::operator!=(
      const Iterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::Iterator::Iterator(HashTable* table,
      size_t bucket, BucketIter iter):
    table_(table),
    bucket_(bucket),
    iter_(iter)
  {}

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::Iterator::moveToNextFilledBucket()
  {
    while (bucket_ < table_->bucketsCount_ && table_->buckets_[bucket_].empty()) {
      ++bucket_;
    }
    if (bucket_ < table_->bucketsCount_) {
      iter_ = table_->buckets_[bucket_].begin();
    } else {
      iter_ = BucketIter();
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::ConstIterator::ConstIterator():
    table_(nullptr),
    bucket_(0),
    iter_()
  {}

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::ConstIterator::ConstIterator(
      const Iterator& other):
    table_(other.table_),
    bucket_(other.bucket_),
    iter_(other.iter_)
  {}

  template< class Key, class Value, class Hash, class Equal >
  const typename HashTable< Key, Value, Hash, Equal >::Entry&
  HashTable< Key, Value, Hash, Equal >::ConstIterator::operator*() const
  {
    return *iter_;
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename HashTable< Key, Value, Hash, Equal >::Entry*
  HashTable< Key, Value, Hash, Equal >::ConstIterator::operator->() const
  {
    return &(*iter_);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::ConstIterator&
  HashTable< Key, Value, Hash, Equal >::ConstIterator::operator++()
  {
    if (table_ == nullptr || bucket_ >= table_->bucketsCount_) {
      return *this;
    }

    ++iter_;
    if (iter_ == table_->buckets_[bucket_].end()) {
      ++bucket_;
      moveToNextFilledBucket();
    }

    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::ConstIterator::operator==(
      const ConstIterator& other) const
  {
    if (table_ != other.table_ || bucket_ != other.bucket_) {
      return false;
    }
    if (table_ == nullptr || bucket_ >= table_->bucketsCount_) {
      return true;
    }
    return iter_ == other.iter_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::ConstIterator::operator!=(
      const ConstIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::ConstIterator::ConstIterator(
      const HashTable* table, size_t bucket, BucketConstIter iter):
    table_(table),
    bucket_(bucket),
    iter_(iter)
  {}

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::ConstIterator::moveToNextFilledBucket()
  {
    while (bucket_ < table_->bucketsCount_ && table_->buckets_[bucket_].empty()) {
      ++bucket_;
    }
    if (bucket_ < table_->bucketsCount_) {
      iter_ = table_->buckets_[bucket_].begin();
    } else {
      iter_ = BucketConstIter();
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(size_t slots):
    buckets_(nullptr),
    bucketsCount_(slots),
    size_(0),
    hash_(),
    equal_()
  {
    if (slots == 0) {
      throw std::invalid_argument("zero hash table size");
    }
    buckets_ = new Bucket[slots];
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::~HashTable()
  {
    delete[] buckets_;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(const HashTable& other):
    buckets_(nullptr),
    bucketsCount_(other.bucketsCount_),
    size_(0),
    hash_(other.hash_),
    equal_(other.equal_)
  {
    buckets_ = new Bucket[bucketsCount_];
    try {
      for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
        add(it->key, it->value);
      }
    } catch (...) {
      delete[] buckets_;
      buckets_ = nullptr;
      throw;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >&
  HashTable< Key, Value, Hash, Equal >::operator=(const HashTable& other)
  {
    if (this == &other) {
      return *this;
    }

    HashTable tmp(other);
    swap(tmp);
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::add(const Key& key, const Value& value)
  {
    const size_t index = getIndex(key);
    for (BucketIter it = buckets_[index].begin(); it != buckets_[index].end(); ++it) {
      if (equal_(it->key, key)) {
        it->value = value;
        return;
      }
    }

    const Entry entry(key, value);
    if (buckets_[index].empty()) {
      buckets_[index].pushFront(entry);
    } else {
      buckets_[index].insert(buckets_[index].last(), entry);
    }
    ++size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value HashTable< Key, Value, Hash, Equal >::drop(const Key& key)
  {
    const size_t index = getIndex(key);
    for (BucketIter it = buckets_[index].begin(); it != buckets_[index].end(); ++it) {
      if (equal_(it->key, key)) {
        Value value = it->value;
        buckets_[index].erase(it);
        --size_;
        return value;
      }
    }
    throw std::out_of_range("key not found");
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::has(const Key& key) const
  {
    const size_t index = getIndex(key);
    for (BucketConstIter it = buckets_[index].begin(); it != buckets_[index].end(); ++it) {
      if (equal_(it->key, key)) {
        return true;
      }
    }
    return false;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value& HashTable< Key, Value, Hash, Equal >::at(const Key& key)
  {
    const size_t index = getIndex(key);
    for (BucketIter it = buckets_[index].begin(); it != buckets_[index].end(); ++it) {
      if (equal_(it->key, key)) {
        return it->value;
      }
    }
    throw std::out_of_range("key not found");
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value& HashTable< Key, Value, Hash, Equal >::at(const Key& key) const
  {
    const size_t index = getIndex(key);
    for (BucketConstIter it = buckets_[index].begin(); it != buckets_[index].end(); ++it) {
      if (equal_(it->key, key)) {
        return it->value;
      }
    }
    throw std::out_of_range("key not found");
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::rehash(size_t slots)
  {
    if (slots == 0) {
      throw std::invalid_argument("zero hash table size");
    }

    HashTable tmp(slots);
    for (const_iterator it = cbegin(); it != cend(); ++it) {
      tmp.add(it->key, it->value);
    }
    swap(tmp);
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t HashTable< Key, Value, Hash, Equal >::size() const
  {
    return size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::empty() const
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::iterator
  HashTable< Key, Value, Hash, Equal >::begin()
  {
    Iterator it(this, 0, BucketIter());
    it.moveToNextFilledBucket();
    return it;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::iterator
  HashTable< Key, Value, Hash, Equal >::end()
  {
    return Iterator(this, bucketsCount_, BucketIter());
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator
  HashTable< Key, Value, Hash, Equal >::begin() const
  {
    return cbegin();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator
  HashTable< Key, Value, Hash, Equal >::end() const
  {
    return cend();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator
  HashTable< Key, Value, Hash, Equal >::cbegin() const
  {
    ConstIterator it(this, 0, BucketConstIter());
    it.moveToNextFilledBucket();
    return it;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::const_iterator
  HashTable< Key, Value, Hash, Equal >::cend() const
  {
    return ConstIterator(this, bucketsCount_, BucketConstIter());
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t HashTable< Key, Value, Hash, Equal >::getIndex(const Key& key) const
  {
    return hash_(key) % bucketsCount_;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::swap(HashTable& other)
  {
    Bucket* tmpBuckets = buckets_;
    buckets_ = other.buckets_;
    other.buckets_ = tmpBuckets;

    const size_t tmpBucketsCount = bucketsCount_;
    bucketsCount_ = other.bucketsCount_;
    other.bucketsCount_ = tmpBucketsCount;

    const size_t tmpSize = size_;
    size_ = other.size_;
    other.size_ = tmpSize;

    const Hash tmpHash = hash_;
    hash_ = other.hash_;
    other.hash_ = tmpHash;

    const Equal tmpEqual = equal_;
    equal_ = other.equal_;
    other.equal_ = tmpEqual;
  }
}

#endif
