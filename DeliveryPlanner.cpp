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

// BEGIN MY CODE, THE IMPLEMENTATION OF ABOVE SPECIFIED INTERFACE

// constructor: sm is used by helper classes
DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
:m_streets(sm){}

// destructor
DeliveryPlannerImpl::~DeliveryPlannerImpl(){}

// generateDeliveryPlan: synthesizes all the other classes
// to generate a step-by-step list of commands for
// Navigating the streets, in order to deliver your quarantine eats.
DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    DeliveryOptimizer optimize(m_streets);
    PointToPointRouter pathfinder(m_streets);
    totalDistanceTravelled = 0;
    commands.clear();
    
    double dist;
    vector<DeliveryRequest> reordered = deliveries;
    optimize.optimizeDeliveryOrder(depot, reordered, dist, dist); // crow distance info has no use
    
    // All order items have non-empty names
    reordered.push_back(DeliveryRequest("", depot)); // Thus, this can save some lines of code
    
    list<list<StreetSegment>> deliverRoute;
    list<StreetSegment> inBetweenRoute;
    
    // FIRST, GENERATE THE ROUTES BETWEEN DELIVERIES
    // (this includes the depot start and depot end)
    // and also handles calculating total distance traveled
    // because PtPRouter is kind enough to calculate the distances for us
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
    
    // THEN, GENERATE THE COMMANDS TO ACHIEVE THOSE DELIVERIES
    int i = 0;  // To keep track of which delivery we are on
    for (auto& deli : deliverRoute)
    {
        StreetSegment prevSeg;
        DeliveryCommand prevCmd;
    
        if (!deli.empty())      // Need to do some actual traveling
        {
            // The start of every initial departure must be a PROCEED command
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
                // If this Segment is the same street, add it to the PROCEED
                if (currSeg.name == prevSeg.name) {
                    prevCmd.increaseDistance(segLength);
                // Else we need to generate a TURN and a PROCEED for the new street
                } else {
                    // push back the running PROCEED for the previous street
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
            // Reached end of route, push back that last PROCEED command
            commands.push_back(prevCmd);
        }
        if (reordered[i].item.empty())  // This is the final return to depot. We are done.
            break;
        // Generate a DELIVER command
        prevCmd.initAsDeliverCommand(reordered[i].item);
        commands.push_back(prevCmd);
        i++;
    }
    return DELIVERY_SUCCESS;
}

// determineTurn: determines the string direction as specified in the spec
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

// determineProceed: determines the string direction as specified in the spec
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

// END MY IMPLEMENTATION

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
