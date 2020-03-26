// STUDENT NOTES: IMPLEMENTATION DETAILS
// I originally considered making my own linked list for
// the buckets so it could be easier to erase one key-value pair.
// Then I realized that we are not required to implement this
// functionality so what was I doing? (And I also realized
// I had no plan for how I was going to rehash my items.)

// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

// Start my code
#ifndef EXHASHMAP_INCLUDED
#define EXHASHMAP_INCLUDED

#include <list>
#include <vector>
#include <iostream>

const int initialBucketNum = 8;
const int expandingFactor = 2;

template<typename KeyType, typename ValueType>
struct MapNode
{
    KeyType key;
    ValueType value;
};

// End my code
template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);  // constructor; O(1)-ish
	~ExpandableHashMap();   // destructor; deletes all of the items in the hashmap; O(N)
	void reset();   // resets the hashmap back to 8 buckets, deletes all items; O(N)
	int size() const;   // return the number of associations in the hashmap; O(1)
    
    // The associate method associates one item (key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the hashmap with that key/value pair. If there is
    // already an association with that key in the hashmap, then the item
    // associated with that key is replaced by the second parameter (value).
    // Thus, the hashmap must contain no duplicate keys.
    // And the hashmap contains only unique keys.
	void associate(const KeyType& key, const ValueType& value);

    
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value, and if the hashmap is allowed to be modified, to
    // modify that value directly within the map (the second overload enables
    // this). Using a little C++ magic, we have implemented it in terms of the
    // first overload, which you must implement.
    
	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

    // REMOVE THIS LATER: IT IS FOR TESTING PURPOSES
    std::list<MapNode<KeyType, ValueType>*>* giveContents();
    
	  // C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    // buckets are lists of pointers to map nodes
    std::vector<std::list<MapNode<KeyType, ValueType>*>> m_map;
    
    // list that contains pointers to all items in the map
    std::list<MapNode<KeyType, ValueType>*> m_items;
    
    int m_totalBuckets;     // number of buckets in the map
    int m_size;             // number of items in the hashmap
    double m_maxLoadFactor; // if m_size/m_totalBuckets exceeds this, need to rehash
    
    void rehash();
};

// BEGIN MY CODE, THE IMPLEMENTATION OF ABOVE SPECIFIED INTERFACE

template<typename KeyType, typename ValueType>
std::list<MapNode<KeyType, ValueType>*>* ExpandableHashMap<KeyType, ValueType>::giveContents()
{
    return &m_items;
}
template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    // just initializing values to member variables
    m_maxLoadFactor = maximumLoadFactor;
    m_totalBuckets = initialBucketNum;
    m_size = 0;
    m_map.resize(m_totalBuckets);   // vector has this cool function
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    reset();
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    // delete the items through our item list, then clear the item list
    for (auto& it : m_items)
        delete it;
    m_items.clear();
    m_map.clear();      // clear the map vector
    
    // reset member variables
    m_totalBuckets = initialBucketNum;
    m_size = 0;
    m_map.resize(m_totalBuckets);
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_size;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    unsigned int hasher(const KeyType& k);
    
    // IF THE KEY ALREADY EXISTS IN THE MAP, OVERWRITE THE VALUE
    ValueType* existing = nullptr;
    existing = find(key);
    if (existing != nullptr) {
        *existing = value;
        return;
    }
    
    // ELSE IT IS COMPLETELY NEW ITEM
    m_size++;
    MapNode<KeyType, ValueType>* toAdd = new MapNode<KeyType, ValueType>;
    toAdd->key = key;
    toAdd->value = value;
    m_items.push_back(toAdd);
    
    // potentially rehash
    if ( (((double)m_size)/((double)m_totalBuckets)) > m_maxLoadFactor )
        rehash();
    else // insert the item according to its hash
    {
        unsigned int bucketNum = hasher(key) % m_totalBuckets;
        m_map[bucketNum].push_back(toAdd);
    }
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::rehash()
{
    unsigned int hasher(const KeyType& k);
    m_map.clear();      // clear the map vector
    // set new number of buckets
    m_totalBuckets = m_totalBuckets * expandingFactor;
    // resize map vector to new bucket size
    m_map.resize(m_totalBuckets);
    // rehash the items
    for (auto& it : m_items)
    {
        unsigned int bucketNum = hasher(it->key) % m_totalBuckets;
        m_map[bucketNum].push_back(it);
    }
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    unsigned int hasher(const KeyType& k);
    // find the bucket for the item
    unsigned int bucketNum = hasher(key) % m_totalBuckets;
    // look for it in the bucket
    for (auto& it : m_map[bucketNum])
    {
        // if found, return pointer to the value
        if (it->key == key)
            return &(it->value);
    }
    return nullptr;
}

#endif // EXHASHMAP_INCLUDED
