#pragma once

#include <cmath>
#include <functional>

namespace Random
{
	namespace Distributions
	{
		class DistributionBase
		{
		public:
			DistributionBase() = default;
			DistributionBase(std::function<double()>&& SeedFunction);
			// No copy constructors for today.
			DistributionBase(const std::function<double()>& SeedFunction) = delete;

			virtual double GetValue() const = 0;
			virtual double GetValue(double InParam) const = 0;

		protected:
			const std::function<double()> m_SeedFunction;
		};
	}
}
