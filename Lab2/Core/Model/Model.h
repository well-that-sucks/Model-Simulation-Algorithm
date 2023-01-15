#pragma once

#include "GraphNodes/NodeBase.h"
#include "Tasks/TaskBase.h"

#include <functional>
#include <stack>
#include <vector>

namespace Model
{
	class Model
	{
	public:
		Model() = default;
		Model(const std::vector<std::reference_wrapper<Nodes::NodeBase>>& InNodes);
		Model(const Model& PreDefinedModel);
		Model(Model&& PreDefinedModel) noexcept;

		void Simulate(double SimulationTime);

		const std::vector<std::reference_wrapper<Nodes::NodeBase>>& GetModelNodes() const;

	private:
		std::vector<std::reference_wrapper<Nodes::NodeBase>> m_Nodes;
	};
}