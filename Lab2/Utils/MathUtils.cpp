#include "MathUtils.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>

#include <iostream>

std::vector<std::pair<double, double>> MathUtils::GenerateAndJoinTestData(const std::vector<std::vector<double>>& InData, 
	std::function<double(const std::pair<double, double>&)> TheoreticalValueGenerator)
{
	std::vector<std::pair<double, double>> OutData;

	for (int i = 0; i < InData.size(); ++i)
	{
		OutData.push_back(std::make_pair(static_cast<double>(InData[i].size()),
			TheoreticalValueGenerator(std::make_pair(*InData[i].begin(), *InData[i].rbegin()))));
	}

	return OutData;
}

double MathUtils::GetError(const std::vector<std::pair<double, double>>& TestData)
{
	double Error = 0;

	for (int i = 0; i < TestData.size(); ++i)
	{
		Error += std::pow(TestData[i].first - TestData[i].second, 2.0) / TestData[i].second;
	}

	return Error;
}

std::vector<std::vector<double>> MathUtils::GetEvenlyDistributedSequence(const std::vector<double>& InputSequence, 
	int SubsequencesNumber, int MinSubSeqLength)
{
	std::vector<std::vector<double>> Entries;

	std::vector<double> InputSequenceCopy = InputSequence;
	std::sort(InputSequenceCopy.begin(), InputSequenceCopy.end());

	const double LeftBorder = *InputSequenceCopy.begin();
	const double RightBorder = *InputSequenceCopy.rbegin();

	const double TargetSequenceLength = (RightBorder - LeftBorder) / SubsequencesNumber;

	double CurrentLeftBorder = LeftBorder;
	std::vector<double>::const_iterator InputSeqIterator = InputSequenceCopy.begin();

	double Offset = 0;

	while (InputSeqIterator != InputSequenceCopy.end())
	{
		const double DesiredRightBorder = *InputSeqIterator + TargetSequenceLength - Offset;
		std::vector<double> NewSubsequence;

		while (InputSeqIterator != InputSequenceCopy.end() && 
			(*InputSeqIterator <= DesiredRightBorder || NewSubsequence.size() < MinSubSeqLength))
		{
			NewSubsequence.push_back(*InputSeqIterator);
			++InputSeqIterator;
		}

		if (InputSeqIterator != InputSequenceCopy.end())
		{
			Offset = std::clamp(std::abs(*InputSeqIterator - DesiredRightBorder), 0.0, std::abs(DesiredRightBorder));
		}

		if (NewSubsequence.size() < MinSubSeqLength)
		{
			std::vector<double>& PreviosSubsequence = *Entries.rbegin();
			PreviosSubsequence.insert(PreviosSubsequence.end(), NewSubsequence.begin(), NewSubsequence.end());
		}
		else
		{
			Entries.push_back(NewSubsequence);
		}
	}

	return Entries;
}

double MathUtils::GetGaussianDistributionProbability(double Sigma, double Alpha, std::pair<double, double> Interval)
{
	if (Interval.first >= Interval.second)
	{
		return 0;
	}

	const double LeftCDFValue = GetGaussianDistrubutionCDFValue((Interval.first - Alpha) / Sigma);
	const double RightCDFValue = GetGaussianDistrubutionCDFValue((Interval.second - Alpha) / Sigma);

	return RightCDFValue - LeftCDFValue;
}

double MathUtils::GetGaussianDistrubutionCDFValue(double X)
{
	return 0.5 * (1.0 + std::erf(X / std::sqrt(2.0)));
}

double MathUtils::GetRandomNumberNormalized()
{
	// https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
	static std::mt19937 RandEngine{ std::random_device{}() };
	return RandEngine() / static_cast<double>(std::mt19937::max());
}

std::pair<double, double> MathUtils::GetGaussianDistributionXAtValueY(double Sigma, double Alpha, double Y)
{
	const double CommonValue = std::sqrt(-2 * std::log(Sigma * std::sqrt(2 * std::numbers::pi) * Y));

	if (!CommonValue)
	{
		return std::make_pair(std::nan(""), std::nan(""));
	}

	const double LeftX = -Sigma * CommonValue + Alpha;
	const double RightX = Sigma * CommonValue + Alpha;


	return std::make_pair(LeftX, RightX);
}

double MathUtils::GetExponentialDistributionProbability(double Lambda, std::pair<double, double> Interval)
{
	return std::exp(-Lambda * Interval.first) - std::exp(-Lambda * Interval.second);
}
