#pragma once

#include "DistributionBase.h"

namespace Random
{
	namespace Distributions
	{
		class DistributionUniform : DistributionBase
		{
		public:
			// Non-static functions
			DistributionUniform(double ParamA, double ParamC);

			double GetValue(double InParam) const override;

			// Static declaration, in case you don't need to create an instance of the class
			static double GetValue(double InParamA, double InParamC, double InParam, const std::function<double()>& SeedFunction);
			static double GenerateByExpectedValue(double Min, double Max);

		private:
			double m_ParamA;
			double m_ParamC;

			double m_PreviousPassResult;
		};
	}
}
