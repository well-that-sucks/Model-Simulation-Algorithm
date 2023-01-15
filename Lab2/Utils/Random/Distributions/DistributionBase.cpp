#include "DistributionBase.h"

Random::Distributions::DistributionBase::DistributionBase(std::function<double()>&& SeedFunction)
	: m_SeedFunction(std::move(SeedFunction))
{
}
