#pragma once
/// Acest fisier este pentru a declara o clasa de oferte care va fi utilizata in HiveMind pentru a calcula care agent sa preia ce pachet, in functie de profitabilitate
class Offer
{
    int profit;
    int agentIdx;
    int packageIdx;
    bool station;
public:
    Offer(int prof, int aIdx, int pIdx, bool stn) : profit(prof), agentIdx(aIdx), packageIdx(pIdx), station(stn){}
    int getProfit() const {
        return profit;
    }
    int getAgentIdx() const {
        return agentIdx;
    }
    int getPackageIdx() const {
        return packageIdx;
    }
    bool getStation() const{
        return station;
    }

    /// Suprascriu operatorul < pentru a putea sorta ofertele direct
    bool operator<(const Offer& other) const {
        return profit > other.profit; // Sortare descrescatoare pentru ca pachetele sa fie alocate in functie de profitabilitate
    }

};
