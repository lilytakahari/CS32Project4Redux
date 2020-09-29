#include "provided.h"
#include <list>
using namespace std;

// Start my code
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <vector>

// MapSearchNode: wrapper container to facilitate A* search
struct MapSearchNode
{
    MapSearchNode(string strCoord, GeoCoord xy, double* f)
                  :id(strCoord), coord(xy), fScore(f){}
    
    string id;          // latitudeText + ", " longitudeText
    GeoCoord coord;     // the actual GeoCoord struct
    double* fScore;     // pointer to an fScore stored in a map
};

// GreaterNode: a functor for use with priority_queue
// so that priority_queue can order the custom-made MapSearchNode
class GreaterNode
{
public:
    bool operator() (MapSearchNode& lhs, const MapSearchNode& rhs)
    {
        return *(lhs.fScore) > *(rhs.fScore);
    }
};
// End my code

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
// Start my code
private:
    const StreetMap* m_streets;
// End my code
};

// BEGIN MY CODE, THE IMPLEMENTATION OF ABOVE SPECIFIED INTERFACE

// constructor: initializes StreetMap pointer
PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
: m_streets(sm){}

// destructor
PointToPointRouterImpl::~PointToPointRouterImpl(){}

// generatePointToPointRoute: generates an optimal route distance-wise
// (if the A* works correctly) from one GeoCoord to another
// based on the StreetMap given
DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const

// This function uses A* search based on the Wikipedia's approach
{
    route.clear();
    totalDistanceTravelled = 0;
    if (start == end)
        return DELIVERY_SUCCESS;
    vector<StreetSegment> successors;
    if (!(m_streets->getSegmentsThatStartWith(end, successors)))
        return BAD_COORD;
    if (!(m_streets->getSegmentsThatStartWith(start, successors)))
        return BAD_COORD;
    
    // The set of discovered nodes that may need to be (re-)expanded.
    unordered_set<string> openSet;
    typedef priority_queue<MapSearchNode, vector<MapSearchNode>, GreaterNode> my_pqueue;
    // The accompanying priority queue for openSet
    my_pqueue openSet_priority;
    string startStr = start.latitudeText + ", " + start.longitudeText;
    openSet.insert(startStr);
    
    // For node n, cameFrom[n] is the StreetSegment we had to take to get from
    // start: previous node; end: n
    unordered_map<string, StreetSegment> cameFrom;
    
    // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
    unordered_map<string, double> gScore;
    gScore[startStr] = 0.0;
    
    // For node n, fScore[n] := gScore[n] + h(n).
    // where h(n) is heuristic(n) and our heuristic is the crow's distance to the end.
    unordered_map<string, double> fScore;
    fScore[startStr] = distanceEarthMiles(start, end);
    
    openSet_priority.push(MapSearchNode(startStr, start, &(fScore[startStr])));
    
    while (!openSet_priority.empty())
    {
        MapSearchNode currNode = openSet_priority.top();
        openSet_priority.pop();
        openSet.erase(currNode.id);
        
        if (currNode.coord == end)
        {
            totalDistanceTravelled = gScore[currNode.id];
            
            // Reconstruct route.
            StreetSegment fromSeg;
            string cameFromStr = end.latitudeText + ", " + end.longitudeText;
            do
            {
                fromSeg = cameFrom[cameFromStr];
                route.push_front(fromSeg);
                cameFromStr = fromSeg.start.latitudeText + ", " + fromSeg.start.longitudeText;
            } while (cameFrom.find(cameFromStr) != cameFrom.end());
            
            return DELIVERY_SUCCESS;
        }
        
        m_streets->getSegmentsThatStartWith(currNode.coord, successors);
        for (auto& seg : successors)
        {
            double tentative_gScore = gScore[currNode.id] + distanceEarthMiles(currNode.coord, seg.end);
            string endStr = seg.end.latitudeText + ", " + seg.end.longitudeText;
            
            if ( (gScore.find(endStr) == gScore.end()) || tentative_gScore < gScore[endStr])
            {
                // This path to neighbor is better than any previous one. Record it!
                cameFrom[endStr] = seg;
                gScore[endStr] = tentative_gScore;
                fScore[endStr] = tentative_gScore + distanceEarthMiles(seg.end, end);
                
                // if neighbor not in openSet
                if (openSet.find(endStr) == openSet.end()) {
                    openSet.insert(endStr);
                    openSet_priority.push(MapSearchNode(endStr, seg.end, &(fScore[endStr])));
                }
            }
        }
    }
    return NO_ROUTE;
}

// END MY IMPLEMENTATION

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
