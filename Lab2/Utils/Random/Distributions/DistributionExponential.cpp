#include "DistributionExponential.h"

#include <iostream>

#include "../../MathUtils.h"

#define EXPONENTIAL_FUNC(Lambda, Param) (-1.0 / Lambda * std::log(Param))

Random::Distributions::DistributionExponential::DistributionExponential(double ParamLambda) : m_ParamLambda(ParamLambda) 
{
}

double Random::Distributions::DistributionExponential::GetValue(double InParam) const
{
	return EXPONENTIAL_FUNC(m_ParamLambda, InParam);
}

double Random::Distributions::DistributionExponential::GenerateValue(double InLambda, double InParam, const std::function<double()>& SeedFunction)
{
	return EXPONENTIAL_FUNC(InLambda, InParam);
}

double Random::Distributions::DistributionExponential::GenerateByExpectedValue(double ExpectedValue)
{
	return -std::log(MathUtils::GetRandomNumberNormalized()) * ExpectedValue;
}
