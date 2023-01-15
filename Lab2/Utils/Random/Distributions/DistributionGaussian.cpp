#include "DistributionGaussian.h"

#include "../../MathUtils.h"

#define GAUSSIAN_FUNC(InParamC, Param) (Param / InParamC)

Random::Distributions::DistributionGaussian::DistributionGaussian(double ParamA, double ParamC, std::function<double()>&& SeedFunction)
	: m_ParamA(ParamA), m_ParamC(ParamC), Distributions::DistributionBase(std::move(SeedFunction))
{
}

double Random::Distributions::DistributionGaussian::GetValue() const
{
	return GAUSSIAN_FUNC(m_ParamC, m_SeedFunction());
}

double Random::Distributions::DistributionGaussian::GetValue(double InParam) const
{
	return GAUSSIAN_FUNC(m_ParamC, InParam);
}

double Random::Distributions::DistributionGaussian::GenerateValue(double InParamA, double InParamC, double InParam)
{
	return GAUSSIAN_FUNC(InParamC, InParam);
}

double Random::Distributions::DistributionGaussian::GenerateByExpectedValue(double ExpectedValue, double Deviation)
{
	// ExpectedValue + Deviation * GetGaussianParameter()
	return ExpectedValue + Deviation * GetGaussianParameter();
}

double Random::Distributions::DistributionGaussian::GetGaussianParameter()
{
	const double Offset = -6;
	double OutParamPartial = 0;

	for (int i = 0; i < 12; ++i)
	{
		OutParamPartial += MathUtils::GetRandomNumberNormalized();
	}
		
	return Offset + OutParamPartial;
}
