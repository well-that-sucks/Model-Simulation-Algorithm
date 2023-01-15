#pragma once

#include "DistributionBase.h"

namespace Random
{
	namespace Distributions
	{
		class DistributionErlang : public DistributionBase
		{
		public:
			// Non-static functions
			DistributionErlang(double Alpha, double Beta);

			double GetValue(double InParam) const override;

			// Static declaration, in case you don't need to create an instance of the class
			static double GenerateByExpectedValue(double Alpha, double Beta);

		private:
			double m_ParamAlpha;
			double m_ParamBeta;
		};
	}
}
