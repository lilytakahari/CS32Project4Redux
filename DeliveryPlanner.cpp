#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
// Start my code
private:
    const StreetMap* m_streets;
    bool determineTurn(const StreetSegment& prevSeg, const StreetSegment& currSeg, string& dir) const;
    void determineProceed(const StreetSegment& street, string& dir) const;
// End my code
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
:m_streets(sm)
{
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    //TODO: testing, commenting
    DeliveryOptimizer optimize(m_streets);
    PointToPointRouter pathfinder(m_streets);
    totalDistanceTravelled = 0;
    commands.clear();
    double dist;
    vector<DeliveryRequest> reordered = deliveries;
    optimize.optimizeDeliveryOrder(depot, reordered, dist, dist);
    // I assume all order items have non-empty names
    // I am trying to save some lines of code
    reordered.push_back(DeliveryRequest("", depot));
    
    list<list<StreetSegment>> deliverRoute;
    list<StreetSegment> inBetweenRoute;
    
    GeoCoord prevLoc = depot;
    GeoCoord currLoc;
    DeliveryResult ret;
    for (auto& currOrder : reordered)
    {
        currLoc = currOrder.location;
        ret = pathfinder.generatePointToPointRoute(prevLoc, currLoc, inBetweenRoute, dist);
        if (ret == BAD_COORD || ret == NO_ROUTE)
            return ret;
        deliverRoute.push_back(inBetweenRoute);
        totalDistanceTravelled += dist;
        prevLoc = currLoc;
    }
    
    int i = 0;
    
    for (auto& deli : deliverRoute)
    {
        StreetSegment prevSeg;
        DeliveryCommand prevCmd;
        if (!deli.empty())
        {
            prevSeg = *(deli.begin());
            StreetSegment currSeg;
            double segLength = distanceEarthMiles(prevSeg.start, prevSeg.end);
            string dir;
            determineProceed(prevSeg, dir);
            prevCmd.initAsProceedCommand(dir, prevSeg.name, segLength);
            
            auto seg = deli.begin();
            seg++;
            for ( ; seg != deli.end(); seg++)
            {
                currSeg = *(seg);
                segLength = distanceEarthMiles(currSeg.start, currSeg.end);
                if (currSeg.name == prevSeg.name) {
                    prevCmd.increaseDistance(segLength);
                } else {
                    commands.push_back(prevCmd);
                    if (determineTurn(prevSeg, currSeg, dir)) {
                        prevCmd.initAsTurnCommand(dir, currSeg.name);
                        commands.push_back(prevCmd);
                    }
                    determineProceed(currSeg, dir);
                    prevCmd.initAsProceedCommand(dir, currSeg.name, segLength);
                }
                prevSeg = currSeg;
            }
            commands.push_back(prevCmd);
        }
        if (reordered[i].item.empty())
            continue;
        prevCmd.initAsDeliverCommand(reordered[i].item);
        commands.push_back(prevCmd);
        i++;
    }
    return DELIVERY_SUCCESS;
}


bool DeliveryPlannerImpl::determineTurn(const StreetSegment& prevSeg,
                                        const StreetSegment& currSeg, string& dir) const
{
    double angle = angleBetween2Lines(prevSeg, currSeg);
    if (angle < 1.0 || angle > 359.0)
        return false;
    else if (angle >= 1.0 && angle < 180)
        dir = "left";
    else
        dir = "right";
    return true;
}
void DeliveryPlannerImpl::determineProceed(const StreetSegment& street, string& dir) const
{
    double angle = angleOfLine(street);
    if (0.0 <= angle && angle < 22.5)
        dir = "east";
    else if (22.5 <= angle && angle < 67.5)
        dir = "northeast";
    else if (67.5 <= angle && angle < 112.5)
        dir = "north";
    else if (112.5 <= angle && angle < 157.5)
        dir = "northwest";
    else if (157.5 <= angle && angle < 202.5)
        dir = "west";
    else if (202.5 <= angle && angle < 247.5)
        dir = "southwest";
    else if (247.5 <= angle && angle < 292.5)
        dir = "south";
    else if (292.5 <= angle && angle < 337.5)
        dir = "southeast";
    else
        dir = "east";
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
