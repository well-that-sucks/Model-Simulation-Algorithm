#pragma once

#include <vector>



namespace Random
{
	namespace Distributions
	{
		class DistributionBase;
	}

	class Random
	{
	public:
		// We don't want to create an instance of this class. Do we?
		Random() = delete;

		static std::vector<double> GenerateRandomSequence(Distributions::DistributionBase&& Distribution, int SeqLength);
	};
}