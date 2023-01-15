#pragma once

#include "NodeBase.h"

namespace Model
{
	namespace Nodes
	{
		class NodeGenerator : public NodeBase
		{
		public:
			NodeGenerator(std::string NodeName);

			virtual void OutAction() override;

			std::optional<std::reference_wrapper<NodeBase>> GetNextNode(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) override;

			void SetTaskDistribution(std::vector<std::pair<Tasks::TaskBase, double>> InDistribution);

			bool IsAvailable() const override;

			int GetQueueSize() const override;

		private:
			// Transform discrete probability to the cumulative one
			void TransformProbabilities();

			Tasks::TaskBase GenerateTask();

		private:
			std::vector<std::pair<Tasks::TaskBase, double>> m_TaskDistribution;
		};
	}
}