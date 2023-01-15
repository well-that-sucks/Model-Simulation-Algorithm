#pragma once

#include "DistributionBase.h"

namespace Random
{
	namespace Distributions
	{
		class DistributionExponential : DistributionBase
		{
		public:
			// Non-static functions
			DistributionExponential(double ParamLambda);

			double GetValue(double InParam) const override;

			// Static declaration, in case you don't need to create an instance of the class
			static double GenerateValue(double InLambda, double InParam, const std::function<double()>& SeedFunction);
			static double GenerateByExpectedValue(double ExpectedValue);

		private:
			double m_ParamLambda;
		};
	}
}
