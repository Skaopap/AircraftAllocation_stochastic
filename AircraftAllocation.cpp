#include <iostream>
#include <vector>
#include <random>
#include <math.h> 
#include <fstream>

class Data
{
public :
    Data() {}
    ~Data() {}

    std::vector<int> m_vAircraftAllocation;

    std::vector<float> m_vCosts;
    std::vector<float> m_vRevenueLost;
    std::vector<float> m_vPassengerDemand;

    std::vector<int> m_vAircraftAvailable;
    std::vector<int> m_vAircraftCapacity;

    std::vector<std::vector<int>> m_bAircraftRouteAllocation; // [routeNumber][aircraftType]

    std::vector<int> m_vPassengerTurnedAway; 

    void LoadDataSet_1()
    {
        m_vCosts                = { 18, 21, 18, 16, 10, 15, 16, 14, 9, 10, 9, 6, 17, 16, 17, 15, 10, 18, 16, 12};
        m_vRevenueLost          = { 13, 13, 7, 7, 1 };
        m_vAircraftCapacity     = { 16, 15, 28, 23, 81, 10, 14, 15, 57, 5, 7, 29, 9, 11, 22, 17, 55, 16, 20, 18 };
        m_vAircraftAvailable    = { 10, 19, 25, 15 };

        m_bAircraftRouteAllocation = { {0,5,10,15}, {1,6,11,16}, {2,7,12,17}, {3,8,13,18}, {4,9,14,19} };

        m_vPassengerDemand = { 250, 100 , 180 , 100 , 600};

        /*m_vPassengerDemand.push_back(std::make_pair(std::vector<float> {200, 220, 250, 270, 300}, std::vector<float> {0.2, 0.05, 0.35, 0.2, 0.2}));
        m_vPassengerDemand.push_back(std::make_pair(std::vector<float> {50,150}, std::vector<float> {0.3,0.7}));
        m_vPassengerDemand.push_back(std::make_pair(std::vector<float> {140,160,180,200,220}, std::vector<float> {0.1,0.2,0.4,0.2,0.1}));
        m_vPassengerDemand.push_back(std::make_pair(std::vector<float> {10,50,80,100,340}, std::vector<float> {0.2,0.2,0.3,0.2,0.1}));
        m_vPassengerDemand.push_back(std::make_pair(std::vector<float> {580,600,620}, std::vector<float> {0.1,0.8,0.1}));

        // Compute Esperanza
        for (auto PassengerDemand : m_vPassengerDemand)
        {
            float l_fEsperanza;

            for (int index = 0 ; index < PassengerDemand.first.size() ; ++index)
            {
                l_fEsperanza += PassengerDemand.first[index] * PassengerDemand.second[index];
            }

            m_vPassengerDemandEsperanza.push_back(l_fEsperanza);
        }*/

    }

    float CostEvaluation(const std::vector<int>& p_vAircraftAllocation)
    {
        if (!CheckAircraftAllocationIntegrity(p_vAircraftAllocation))
        {
            std::cout << " Wrong Size Data ";
        }

        float l_fResult = 0;

        // Calcul total operating cost
        for (int index = 0 ; index < p_vAircraftAllocation.size() ; ++index)
        {
            l_fResult += m_vCosts[index] * p_vAircraftAllocation[index];
        }

        // calcul number of passenger turned away per route
        std::vector<float> l_vPassengersTurnedAway;

        // for each route
        for (int index = 0; index < m_vRevenueLost.size(); ++index)
        {
            float l_fPassengerTA = m_vPassengerDemand[index];

            // for each aircraft type in route
            for (auto idAircraftRoute : m_bAircraftRouteAllocation[index])
            {
                l_fPassengerTA -= m_vAircraftCapacity[idAircraftRoute]* p_vAircraftAllocation[idAircraftRoute];
            }

            if (l_fPassengerTA < 0) // more place than expected passengers 
                l_vPassengersTurnedAway.push_back(0);
            else // less place
                l_vPassengersTurnedAway.push_back(l_fPassengerTA);
        }


        // calcul revenue lost
        for (int route = 0; route < 5; ++route)
        {
            l_fResult += m_vRevenueLost[route] * l_vPassengersTurnedAway[route];
        }

        return l_fResult;
    }

    std::vector<int> GenerateRandomAllocation()
    {
        std::vector<int> l_vRandomAllocation(20,0);

        // For each aircraft type
        for (int aircraftType = 0; aircraftType < m_vAircraftAvailable.size(); ++aircraftType)
        {
            int l_iAircraftLeft = m_vAircraftAvailable[aircraftType];
            //std::cout << " dispo  : " << l_iAircraftLeft << std::endl;

            int l_iImproveRandom = 2;

            // For each route
            for (auto routeAirplane : m_bAircraftRouteAllocation)
            {
                if (l_iAircraftLeft != 0)
                {
                    int l_iAircraftAttribution = GenerateRandomNumber(0, l_iAircraftLeft / l_iImproveRandom);
                    l_iAircraftLeft -= l_iAircraftAttribution;
                    l_vRandomAllocation[routeAirplane[aircraftType]] = l_iAircraftAttribution;
                    // std::cout << " attrib : " << l_iAircraftAttribution << std::endl;
                }
            }

            //std::cout << " whats left : " << l_iAircraftLeft << std::endl;
            // if some airplane is still available, put it randomly in one route
            if (l_iAircraftLeft != 0)
                l_vRandomAllocation[m_bAircraftRouteAllocation[GenerateRandomNumber(0, 3)][aircraftType]] += l_iAircraftLeft;

        }

        //printAllocation(l_vRandomAllocation);

        return l_vRandomAllocation;
    }

    std::vector< std::vector<int> > GenerateNeighbors(const std::vector<int>& p_vInitAllocation, const int& l_iAircraftType, const int& p_iNeighborsNumber = 4)
    {
        std::vector< std::vector<int> > l_vNeighbors;

        int rd;

        for (int count = 0; count < p_iNeighborsNumber; ++count)
        {
            rd = GenerateRandomNumber(0, 2);

            if (rd == 0)
            {
                l_vNeighbors.push_back(SingleRandomPermutation(p_vInitAllocation, l_iAircraftType));
            }
            else if (rd == 1)
            {
                l_vNeighbors.push_back(RandomPlusOneMinusOne(p_vInitAllocation, l_iAircraftType));
            }
            else if (rd == 2)
            {
                l_vNeighbors.push_back(WholeNewColumn(p_vInitAllocation, l_iAircraftType));
            }
            /*else if (rd == 3)
            {
                l_vNeighbors.push_back(OneIntoAnother(p_vInitAllocation, l_iAircraftType));
            }*/
        }

        return l_vNeighbors;
       
    }

    void printAllocation(const std::vector<int>& p_vAllocation)
    {
        int route = 1;

        // For each aircraft type
        for (int aircraftType = 0; aircraftType < m_vAircraftAvailable.size(); ++aircraftType)
        {
            std::cout << " Aircraft type : " << aircraftType + 1 << std::endl;

            for (auto routeAirplane : m_bAircraftRouteAllocation)
            {
                std::cout << "x" << routeAirplane[aircraftType] + 1 << " : " << p_vAllocation[routeAirplane[aircraftType]] << std::endl;
            }
        }
    

    }

private:

    bool CheckAircraftAllocationIntegrity(const std::vector<int>& p_vAircraftAllocation)
    {
        if (p_vAircraftAllocation.size() != m_vAircraftCapacity.size())
            return false;

        return true;
    }

    float GenerateRandomNumber(int min, int max)
    {
        // https://en.cppreference.com/w/cpp/numeric/random
        // Seed with a real random value, if available
        std::random_device r;

        // Choose a random mean between 0 and 100
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> uniform_dist(min, max);
        float number = ((float)uniform_dist(e1));
        return number;
    }

    std::vector<int> SingleRandomPermutation(const std::vector<int>& p_vAllocation, const int aircraftType)
    {
        std::vector<int> l_vOutput = p_vAllocation;

        // Compute index
        int l_iIndexToPermuteFirst = GenerateRandomNumber(aircraftType*5, (aircraftType*5) + 4 );
        int l_iIndexToPermuteSecond = l_iIndexToPermuteFirst;

        while(l_iIndexToPermuteFirst== l_iIndexToPermuteSecond)
            l_iIndexToPermuteSecond = GenerateRandomNumber(aircraftType * 5, (aircraftType * 5) + 4);

        // Permute
        int temp = l_vOutput[l_iIndexToPermuteFirst];
        l_vOutput[l_iIndexToPermuteFirst] = l_vOutput[l_iIndexToPermuteSecond];
        l_vOutput[l_iIndexToPermuteSecond] = temp;

        return l_vOutput;
    }

    std::vector<int> PermutationRandomCutPoint(const std::vector<int>& p_vAllocation, const int aircraftType)
    {
        std::vector<int> l_vOutput = p_vAllocation;

        /*// Compute index
        int l_iIndexToPermute = GenerateRandomNumber(aircraftType * 5, (aircraftType * 5) + 4);

        // create sub vector
        std::vector<int> l_vSub;
        for (int index = aircraftType * 5; index <= (aircraftType * 5) + 4; ++index)
        {
            l_vSub.push_back(l_vOutput[index]);
        }

        //std::reverse(l_vSub.begin(), l_vSub.end());

        // create sub vector
        std::vector<int> l_vReverseSub;
        for (int index = 4; index >= 0; ++index)
        {
            l_vReverseSub.push_back(l_vSub[(l_iIndexToPermute+index)%4]);
        }*/

        return l_vOutput;

    }

    std::vector<int> RandomPlusOneMinusOne(const std::vector<int>& p_vAllocation, const int aircraftType)
    {
        std::vector<int> l_vOutput = p_vAllocation;

        // Compute index
        int l_iIndexToPermuteFirst = GenerateRandomNumber(aircraftType * 5, (aircraftType * 5) + 4);

        bool isValid = false;
        // Error check
        for (int index = aircraftType * 5; index <= (aircraftType * 5) + 4; ++index)
        {
            if (index != l_iIndexToPermuteFirst)
                if (p_vAllocation[index] > 0)
                    isValid = true;
        }

        int l_iIndexToPermuteSecond = l_iIndexToPermuteFirst;

        if (isValid)
        {
            while (l_iIndexToPermuteFirst == l_iIndexToPermuteSecond || l_vOutput[l_iIndexToPermuteSecond] == 0)
                l_iIndexToPermuteSecond = GenerateRandomNumber(aircraftType * 5, (aircraftType * 5) + 4);

            // apply
            l_vOutput[l_iIndexToPermuteFirst] += 1;
            l_vOutput[l_iIndexToPermuteSecond] -= 1;
        }
        else // only one positive value
        {
            while (l_iIndexToPermuteFirst == l_iIndexToPermuteSecond)
                l_iIndexToPermuteSecond = GenerateRandomNumber(aircraftType * 5, (aircraftType * 5) + 4);

            // apply
            l_vOutput[l_iIndexToPermuteFirst] -= 1;
            l_vOutput[l_iIndexToPermuteSecond] += 1;
        }
        

        return l_vOutput;
    }

    std::vector<int> OneIntoAnother(const std::vector<int>& p_vAllocation, const int aircraftType)
    {
        std::vector<int> l_vOutput = p_vAllocation;

        // Compute index
        int l_iIndexToPermuteFirst = GenerateRandomNumber(aircraftType * 5, (aircraftType * 5) + 4);
        int l_iIndexToPermuteSecond = l_iIndexToPermuteFirst;

        while (l_iIndexToPermuteFirst == l_iIndexToPermuteSecond)
            l_iIndexToPermuteSecond = GenerateRandomNumber(aircraftType * 5, (aircraftType * 5) + 4);

        // apply
        l_vOutput[l_iIndexToPermuteFirst] += l_vOutput[l_iIndexToPermuteSecond];
        l_vOutput[l_iIndexToPermuteSecond] = 0;

        return l_vOutput;
    }

    std::vector<int> WholeNewColumn(const std::vector<int>& p_vAllocation, const int aircraftType)
    {
        std::vector<int> l_vOutput = p_vAllocation;

        int l_iAircraftLeft = m_vAircraftAvailable[aircraftType];
        //std::cout << " dispo  : " << l_iAircraftLeft << std::endl;

        int l_iImproveRandom = 2;

        // For each airplane type in route
        for (auto routeAirplane : m_bAircraftRouteAllocation)
        {
            if (l_iAircraftLeft != 0)
            {
                int l_iAircraftAttribution = GenerateRandomNumber(0, l_iAircraftLeft / l_iImproveRandom);
                l_iAircraftLeft -= l_iAircraftAttribution;
                l_vOutput[routeAirplane[aircraftType]] = l_iAircraftAttribution;
                // std::cout << " attrib : " << l_iAircraftAttribution << std::endl;
            }
        }

        //std::cout << " whats left : " << l_iAircraftLeft << std::endl;
        // if some airplane is still available, put it randomly in one route
        if (l_iAircraftLeft != 0)
            l_vOutput[m_bAircraftRouteAllocation[GenerateRandomNumber(0, 3)][aircraftType]] += l_iAircraftLeft;

        return l_vOutput;
    }


};

class SimulatedAnnealing
{

public :

    SimulatedAnnealing() {}
    SimulatedAnnealing(float p_fTemperature, float p_fDecreaseFactor) : m_fTemperature(p_fTemperature), m_fDecreaseFactor(p_fDecreaseFactor) {}

    ~SimulatedAnnealing() {}

    float m_fTemperature;

    float m_fDecreaseFactor;

    int m_iStepNumber = 4; // equals to the number of aircraft type 


    void ApplyAlgorithm(Data& p_Data)
    {
        // initiate first candidate and its cost
        std::vector<int> l_vCurrentCandidate    = p_Data.GenerateRandomAllocation(); 
        float l_fCurrentCost                    = p_Data.CostEvaluation(l_vCurrentCandidate);

        std::vector<float> l_vCostHistory{ l_fCurrentCost };

        std::vector<std::vector<int>> l_vNeighbors;

        while (m_fTemperature > 0.1)
        {
            for(int step = 0 ; step < m_iStepNumber ; ++step)
            {
                // Step 1 : Generate Neighbors
                l_vNeighbors = p_Data.GenerateNeighbors(l_vCurrentCandidate, step);

                // Step 2 : Find Minimum cost neighbor
                std::vector<int> l_vMinimunCostNeighbor;
                float l_fCost = FLT_MAX;

                for (auto NeighBorsCandidate : l_vNeighbors)
                {
                    float l_fNewCost = p_Data.CostEvaluation(NeighBorsCandidate);
                    if (l_fNewCost < l_fCost)
                    {
                        l_vMinimunCostNeighbor  = NeighBorsCandidate;
                        l_fCost                 = l_fNewCost;
                    }
                }
                
                // Step 3 : Apply SA condition

                float l_fNeighborsCandidateCost = p_Data.CostEvaluation(l_vMinimunCostNeighbor);
                float l_fCostDifference         = l_fNeighborsCandidateCost - l_fCurrentCost;

                // If it's a better value choose it
                if (l_fCostDifference < 0)
                {
                    l_vCurrentCandidate = l_vMinimunCostNeighbor;
                    l_fCurrentCost      = l_fNeighborsCandidateCost;
                }
                // Condition with the temperature value
                else if (GenerateRandomNumber(0,100) < exp((-l_fCostDifference)/ m_fTemperature))
                {
                    l_vCurrentCandidate = l_vMinimunCostNeighbor;
                    l_fCurrentCost      = l_fNeighborsCandidateCost;
                }
                
            }
            
            // Cost History
            l_vCostHistory.push_back(l_fCurrentCost);

            // Decrease the temperature
            m_fTemperature *= (1-m_fDecreaseFactor);
        }

        // Generate csv for cost history
        std::ofstream myfile;
        myfile.open("result_.csv");
        myfile << "Cost History.\n";
        myfile << "T = "        << m_fTemperature << ",\n";
        myfile << "DFactor = "  << m_fDecreaseFactor << ",\n";
        for(auto cost : l_vCostHistory)
            myfile << cost <<",\n";
        myfile.close();

        // Print Solution
        p_Data.printAllocation(l_vCurrentCandidate);
    }

    

private:

    float GenerateRandomNumber(int min, int max)
    {
        // https://en.cppreference.com/w/cpp/numeric/random
        // Seed with a real random value, if available
        std::random_device r;

        // Choose a random mean between 0 and 100
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> uniform_dist(min, max);
        float number = ((float)uniform_dist(e1)) / 100;

        return number;
    }
    
};

int main()
{
    std::cout << "########## Aircraft Allocation Problem ##########\n";

    Data l_Data;
    SimulatedAnnealing l_SA(9999,0.001);

    l_Data.LoadDataSet_1();

    l_SA.ApplyAlgorithm(l_Data);

}
