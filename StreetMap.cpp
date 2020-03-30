#include "provided.h"
#include <string>
#include <vector>
#include <functional>

// Start my code
#include <iostream> // needed for any I/O
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include "ExpandableHashMap.h"
// End my code

using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return (unsigned int)std::hash<std::string>()(g.latitudeText + g.longitudeText);
}
unsigned int hasher(const string& g)
{
    return (unsigned int)std::hash<std::string>()(g);
}
class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
    
// Start my code
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment*>> m_map;
    list<StreetSegment*> m_segments;
// End my code
};


// BEGIN MY CODE, THE IMPLEMENTATION OF ABOVE SPECIFIED INTERFACE

// constructor
StreetMapImpl::StreetMapImpl(){}

// destructor: Deletes dynamically allocated StreetSegments
StreetMapImpl::~StreetMapImpl()
{
    for (auto& it : m_segments)
        delete it;
}

// load: Loads a file formatted as Westwood street data
// (a format unfortunately unique to this project)
// and stores that info in a hash map for efficient route-finding
bool StreetMapImpl::load(string mapFile)
{
    // Initialize the input file stream and other objects
    ifstream inf(mapFile);
    if (!inf)
        return false;
    string line, slat, slon, elat, elon, streetName;
    int numSegLines = 0; // number of street segments associated with a street
    vector<StreetSegment*>* found = nullptr;
    
    // 1st line: Street name
    getline(inf, streetName);
    // 2nd line: Number of lines following this line that are associated with this street
    inf >> numSegLines;
    inf.ignore(10000, '\n');
    // Following lines: Get the info and create StreetSegments with that info
    while (getline(inf, line))
    {
        // Get info
        istringstream iss(line);
        iss >> slat >> slon >> elat >> elon;
        
        // 0 index: forward direction; 1 index: backward direction
        GeoCoord start_end[2] = {GeoCoord(slat, slon), GeoCoord(elat, elon)};
        StreetSegment* segs[2] = {new StreetSegment(start_end[0], start_end[1], streetName),
                                 new StreetSegment(start_end[1], start_end[0], streetName)};
        for (int i = 0; i < 2; i++)
        {
            // Associate the GeoCoord to the segment it is the start of
            // add the segment data to a collection
            found = m_map.find(start_end[i]);
            if (!found) {
                m_map.associate(start_end[i], vector<StreetSegment*>());
                found = m_map.find(start_end[i]);
            }
            found->push_back(segs[i]);
            m_segments.push_back(segs[i]);
        }
        numSegLines--;
        // Moving on to the next set of street data
        if (numSegLines == 0)
        {
            getline(inf, streetName);
            inf >> numSegLines;
            inf.ignore(10000, '\n');
        }
    }
    return true;
}

// getSegmentsThatStartWith: utilizes the hashmap
bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment*>* found = m_map.find(gc);
    if (!found)
        return false;
    segs.clear();
    for (const auto& it : *found)
        segs.push_back(*it);
    return true;
}

// END MY IMPLEMENTATION

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
