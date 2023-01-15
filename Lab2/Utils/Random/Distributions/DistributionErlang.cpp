#include "DistributionErlang.h"
#include <random>

double Random::Distributions::DistributionErlang::GenerateByExpectedValue(double Alpha, double Beta)
{
    static std::mt19937 Generator(std::random_device{}());
    return (std::gamma_distribution<>(Beta, Alpha / Beta))(Generator);
}
