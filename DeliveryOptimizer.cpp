#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm){}
DeliveryOptimizerImpl::~DeliveryOptimizerImpl(){}

// optimizeDeliveryOrder: given the deliveries, try to lessen the distance
// needed to travel from the depot, to all these locations once, then
// bvack to the depot, by reordering the deliveries
void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
/* This function is essentially trying to solve the Traveling Salesman Problem
 * So I have used the Nearest Neighbor greedy algo to give an approximate solution
 * which may or may not be used. NN is O(N^2).
 */
{
    // Start my code
    
    double oldDist = 0;
    double newDist = 0;
    GeoCoord prevCoord = depot;
    GeoCoord currCoord;
    vector<DeliveryRequest> deliveriesCopy = deliveries;
    
    // Finding the oldCrowDistance
    for (auto& it : deliveries)
    {
        currCoord = it.location;
        oldDist += distanceEarthMiles(prevCoord, currCoord);
        prevCoord = currCoord;
    }
    oldDist += distanceEarthMiles(prevCoord, depot);
    
    // Find the nearest neighbor, then the nearest not-yet reordered neighbor
    vector<DeliveryRequest> reordered;
    currCoord = depot;
    double calcDist;
    int nnIndex = -1;
    while (true) {
        if (deliveries.empty())     // no more deliveries left to reorder
            break;
        
        // Not even the Earth's circumference is this large,
        // so 1 billion should be a nice stand-in for +infinity
        double nnDist = 1000000000;
        
        for (int i = 0; i < deliveries.size(); i++)
        {
            // Find the nearest neighbor and record that distance and neighbor's index in vector
            calcDist = distanceEarthMiles(currCoord, deliveries[i].location);
            if (calcDist < nnDist) {
                nnDist = calcDist;
                nnIndex = i;
            }
        }
        newDist += nnDist;
        reordered.push_back(deliveries[nnIndex]);
        currCoord = deliveries[nnIndex].location;
        deliveries.erase(deliveries.begin() + nnIndex); // remove the neighbor from its own consideration
    }
    newDist += distanceEarthMiles(currCoord, depot);

    // If we can improve the crow distance, we will use the new ordering
    if (newDist <= oldDist) {
        deliveries = reordered;
    // Else, nah I don't trust that
    } else {
        newDist = oldDist;
        deliveries = deliveriesCopy;
    }
    oldCrowDistance = oldDist;
    newCrowDistance = newDist;
    
    // End my code
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
