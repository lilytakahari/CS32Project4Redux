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
StreetMapImpl::StreetMapImpl(){}

StreetMapImpl::~StreetMapImpl()
{
    for (auto& it : m_segments)
        delete it;
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream inf(mapFile);
    if (!inf)
        return false;
    string line, slat, slon, elat, elon, streetName;
    int numSegLines = 0;
    vector<StreetSegment*>* found = nullptr;
    
    getline(inf, streetName);
    inf >> numSegLines;
    inf.ignore(10000, '\n');
    while (getline(inf, line))
    {
        istringstream iss(line);
        iss >> slat >> slon >> elat >> elon;
        GeoCoord start_end[2] = {GeoCoord(slat, slon), GeoCoord(elat, elon)};
        StreetSegment* segs[2] = {new StreetSegment(start_end[0], start_end[1], streetName),
                                 new StreetSegment(start_end[1], start_end[0], streetName)};
        for (int i = 0; i < 2; i++)
        {
            found = m_map.find(start_end[i]);
            if (!found) {
                m_map.associate(start_end[i], vector<StreetSegment*>());
                found = m_map.find(start_end[i]);
            }
            found->push_back(segs[i]);
            m_segments.push_back(segs[i]);
        }
        numSegLines--;
        if (numSegLines == 0)
        {
            getline(inf, streetName);
            inf >> numSegLines;
            inf.ignore(10000, '\n');
        }
    }
    
    // REMOVE LATER; FOR TESTING PURPOSES ONLY
//    list<MapNode<GeoCoord,vector<StreetSegment*>>*>* results;
//    results = m_map.giveContents();
//    for (auto& it : *results)
//    {
//        cout << it->key.latitudeText << endl;
//    }
    
    return true;
    
}

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
