#pragma once

#include "NodeBase.h"
#include "../Tasks/TaskBase.h"

#include <functional>
#include <deque>
#include <vector>
#include <map>

constexpr int NO_LIMITS = -1;

namespace Model
{
	namespace Nodes
	{
		struct NodeProcessorSpecificStatistics
		{
			double AverageQueueLength = 0;
			double TotalTaskLifetime = 0;
			int QueueSwappingOccasions = 0;
		};

		class NodeProcessor : public NodeBase
		{
		public:
			NodeProcessor(std::string NodeName, int MaxQueueLength);
			NodeProcessor(std::string NodeName, int MaxQueueLength, int SubprocessorsNum);

			void InAction(std::shared_ptr<Tasks::TaskBase> InTask) override;
			void OutAction() override;

			void SetCurrentTime(double Time) override;

			double GetNextTime() const override;

			int GetQueueSize() const override;

			std::ostringstream GetNodeInfo() const override;
			std::ostringstream GetNodeStatistics(double TotalSimulationTime) const override;

			bool IsAvailable() const override;

			bool CanTaskEnter(const std::shared_ptr<Tasks::TaskBase>& IncomingTask);

			void ExecutePostTickFunction() override;

			void SetPostTickFunction(std::function<bool(Nodes::NodeProcessor&)> InFunction);
			void SetInActionCondition(std::function<bool(Nodes::NodeProcessor&, const std::shared_ptr<Tasks::TaskBase>&)> InFunction);

			void SetGetNextTaskFunction(std::function<std::shared_ptr<Tasks::TaskBase>&(std::deque<std::shared_ptr<Tasks::TaskBase>>&)> GetNextTask);

			std::shared_ptr<Tasks::TaskBase> PopFrontTask();

			int GetSpecificTaskNum(const std::function<bool(const std::shared_ptr<Tasks::TaskBase>&)>& Pred) const;

		protected:
			void UpdateNextTime(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) override;
			void ReportFailure(const std::shared_ptr<Tasks::TaskBase>& FailedTask) override;

		private:
			struct SubProcess
			{
				std::shared_ptr<Tasks::TaskBase> Task;
				double TimeNext;
			};

			std::shared_ptr<Tasks::TaskBase> GetNextTask();

			std::optional<std::reference_wrapper<SubProcess>> GetIdlingSubprocess();

			inline int GetFreeSlotsNum() const;

		private:
			std::deque<std::shared_ptr<Tasks::TaskBase>> m_TaskQueue;
			std::vector<SubProcess> m_Subprocesses;

			std::function<bool(Nodes::NodeProcessor&, const std::shared_ptr<Tasks::TaskBase>&)> m_CanTaskEnterFunction;

			std::function<bool(Nodes::NodeProcessor&)> m_PostTickFunction;
			std::function<std::shared_ptr<Tasks::TaskBase>&(std::deque<std::shared_ptr<Tasks::TaskBase>>&)> m_GetNextTask;

			NodeProcessorSpecificStatistics m_SpecificStatisticsData;

			int m_MaxQueueLength;
		};
	}
}