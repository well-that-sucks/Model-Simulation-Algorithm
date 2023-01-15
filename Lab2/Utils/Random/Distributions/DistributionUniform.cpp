#include "DistributionUniform.h"

#include "../../MathUtils.h"

#define UNIFORM_FUNC(InParamC, Param) (Param / InParamC)

Random::Distributions::DistributionUniform::DistributionUniform(double ParamA, double ParamC) : m_ParamA(ParamA), m_ParamC(ParamC)
{
	m_PreviousPassResult = 0;
}

double Random::Distributions::DistributionUniform::GetValue(double InParam) const
{
	return UNIFORM_FUNC(m_ParamC, InParam);
}

double Random::Distributions::DistributionUniform::GetValue(double InParamA, double InParamC, double InParam, const std::function<double()>& SeedFunction)
{
	return UNIFORM_FUNC(InParamC, InParam);
}

double Random::Distributions::DistributionUniform::GenerateByExpectedValue(double Min, double Max)
{
	return Min + (MathUtils::GetRandomNumberNormalized() * (Max - Min));
}
