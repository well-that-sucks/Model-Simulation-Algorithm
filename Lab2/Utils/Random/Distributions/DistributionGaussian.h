#pragma once

#include "DistributionBase.h"

namespace Random
{
	namespace Distributions
	{
		class DistributionGaussian : DistributionBase
		{
		public:
			// Non-static functions
			DistributionGaussian(double ParamA, double ParamC, std::function<double()>&& SeedFunction);

			double GetValue() const override;

			double GetValue(double InParam) const override;

			// Static declaration, in case you don't need to create an instance of the class
			static double GenerateValue(double InParamA, double InParamC, double InParam/*, const std::function<double()>& SeedFunction*/);
			static double GenerateByExpectedValue(double ExpectedValue, double Deviation);

		private:
			static double GetGaussianParameter();

		private:
			double m_ParamA;
			double m_ParamC;

		};
	}
}
