#pragma once

#include <vector>
#include <functional>

constexpr auto EPSILON = 0.0001;
constexpr double X2_005[] = {3.84, 5.99, 7.81, 9.49, 11.07, 12.59, 14.07, 15.51, 16.92, 18.31, 19.68, 21.03, 22.36, 23.68, 25.00, 26.30, 27.59, 28.87, 30.14, 31.41, 32.67, 33.92};

class MathUtils
{
public:
	static std::vector<std::pair<double, double>> GenerateAndJoinTestData(const std::vector<std::vector<double>>& InData, 
		std::function<double(const std::pair<double, double>&)> TheoreticalValueGenerator);

	static double GetError(const std::vector<std::pair<double, double>>& TestData);

	static std::vector<std::vector<double>> GetEvenlyDistributedSequence(const std::vector<double>& InputSequence, 
		int SubsequencesNumber, int MinSeqLength);

	// Gaussian distribution methods
	static double GetGaussianDistributionProbability(double Sigma, double Alpha, std::pair<double, double> Interval);
	static std::pair<double, double> GetGaussianDistributionXAtValueY(double Sigma, double Alpha, double Y = EPSILON);

	// Exponential distribution methods
	static double GetExponentialDistributionProbability(double Lambda, std::pair<double, double> Interval);

	static double GetRandomNumberNormalized();

private:
	static double GetGaussianDistrubutionCDFValue(double X);
};