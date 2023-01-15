#pragma once

#include "../Model.h"

namespace Model
{
	class ModelExamples
	{
	public:
		// Lab 2 models
		static void RunLab2Model1(double SimulationTime);
		static void RunLab2Model2(double SimulationTime);
		static void RunLab2Model3(double SimulationTime);

		// Lab 3 models
		static void RunModelCarBank(double SimulationTime);
		static void RunModelHospital(double SimulationTime);
	};
}