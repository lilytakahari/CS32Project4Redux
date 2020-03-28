#include "provided.h"
#include <list>
using namespace std;

// Start my code
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <vector>

//struct MapSearchNode
//{
//    MapSearchNode(GeoCoord xy, double dist):coord(xy), distTraveled(dist) {}
//
//    GeoCoord coord;             // identifying coord
//    list<StreetSegment> path;   // the path from the start of search problem to get to this coord
//    double distTraveled;        // the distance traveled on the above path
//};
struct MapSearchNode
{
    MapSearchNode(string strCoord, GeoCoord xy, double* f)
                  :id(strCoord), coord(xy), fScore(f){}
    string id;
    GeoCoord coord;
    double* fScore;
};

class GreaterNode
{
public:
    bool operator() (MapSearchNode& lhs, const MapSearchNode& rhs)
    {
        return *(lhs.fScore) > *(rhs.fScore);
    }
};

//class GreaterNode
//{
//public:
//    GreaterNode(GeoCoord dest) {m_dest = dest;}
//    bool operator() (MapSearchNode& lhs, const MapSearchNode& rhs)
//    {
//        double lCost = lhs.distTraveled + distanceEarthMiles(lhs.coord, m_dest);
//        double rCost = rhs.distTraveled + distanceEarthMiles(rhs.coord, m_dest);
//        return lCost > rCost;
//    }
//private:
//    GeoCoord m_dest;
//};
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
// greaterNode: A* heuristic function, which is
PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_streets = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const

/* So I know that this function is intended to run in O(N).
 * But in the worst case it is at least O(NlogN), maybe worse
 * The commented out code implemented in the style of my earlier work in Python searches
 * But perhaps I should try Wikipedia's approach, which uses lots of maps so it should be O(N).
 */
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
    
//    unordered_set<string> visited;  // keeps track of visited nodes
//    my_pqueue fringe ((GreaterNode(end)));
//    string startStr = start.latitudeText + ", " + start.longitudeText;
//    MapSearchNode startNode(start, 0);
//    fringe.push(startNode);
//
//    // loop until no more nodes left to expand, or goal reached
//    while (!fringe.empty())
//    {
//        MapSearchNode currNode = fringe.top();
//        fringe.pop();
//        GeoCoord currGC = currNode.coord;
//        string currCoord = currGC.latitudeText + ", " + currGC.longitudeText;
//
//        // only look at node's successors if node has not been expanded yet
//        if (visited.find(currCoord) != visited.end())
//        {
//            visited.insert(currCoord);
//            if (currGC == end)      // return if the node is the goal
//            {
//                route = currNode.path;
//                totalDistanceTravelled = currNode.distTraveled;
//                return DELIVERY_SUCCESS;
//            }
//
//            // otherwise, add unexpanded nodes to visit in the next loop
//            m_streets->getSegmentsThatStartWith(currGC, successors);
//            for (auto& it : successors)
//            {
//                // 1. Find the successor GeoCoords through these segments
//                // 2. Create MapSearchNodes for these GeoCoords
//                // 3. Add the path to the nodes
//                // 4. Add the cost to the nodes
//                // 5. Push the nodes to fringe
//                GeoCoord toAddGC = it.end;
//                double toAddCost = distanceEarthMiles(it.start, it.end);
//                MapSearchNode toAddNode(toAddGC, currNode.distTraveled + toAddCost);
//                toAddNode.path = currNode.path;
//                currNode.path.push_back(it);
//                fringe.push(toAddNode);
//            }
//        }
//    }
    return NO_ROUTE;
}

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
