#include "Random.h"

#include "Distributions/DistributionBase.h"

std::vector<double> Random::Random::GenerateRandomSequence(Distributions::DistributionBase&& Distribution, int SeqLength)
{
	std::vector<double> GeneratedSequence;

	for (int i = 0; i < SeqLength; ++i)
	{
		GeneratedSequence.emplace_back(Distribution.GetValue());
	}

	return GeneratedSequence;
}
