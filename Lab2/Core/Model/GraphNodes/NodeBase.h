#pragma once

#include <vector>
#include <sstream> 
#include <functional>
#include <optional>

namespace Model
{
	namespace Tasks
	{
		class TaskBase;
	}

	namespace Nodes
	{
		struct NodeBaseStatistics
		{
			int PacketsTotal = 0;
			int FailureRate = 0;
			int TotalPasses = 0;
			int TotalPacketsLeavingModel = 0;

			double TotalWorkloadTime = 0;
			double TotalDeltaInActionTime = 0;
			double TotalDeltaOutActionTime = 0;

			double LastInActionTime = 0;
			double LastOutActionTime = 0;
		};

		class NodeBase
		{
		public:
			NodeBase();
			NodeBase(std::string Name);

			// A method which is invoked after a task arrives to the node
			virtual void InAction(std::shared_ptr<Tasks::TaskBase>);

			// A method which is invoked after a task leaves the node
			virtual void OutAction();

			// Used to infer the next node to propagate forward the model
			virtual std::optional<std::reference_wrapper<NodeBase>> GetNextNode(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask);

			// Add next node to the list if it's lvalue
			NodeBase& AddNextNode(NodeBase& NextNode);

			std::string GetName() const;

			virtual int GetQueueSize() const = 0;

			double GetCurrentTime() const;
			virtual double GetNextTime() const;

			virtual void SetCurrentTime(double Time);
			void SetNextTime(double Time);

			void SetGetNextNodeFunction(
				std::function<std::optional<std::reference_wrapper<NodeBase>>(
					std::vector<std::reference_wrapper<NodeBase>>& NodeCandidates,
					const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> GetNextNodeFunction);

			void SetDelayFunction(std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> DelayFunction);

			virtual bool IsAvailable() const = 0;
			virtual bool IsWorking() const;

			virtual std::ostringstream GetNodeInfo() const;
			virtual std::ostringstream GetNodeStatistics(double TotalSimulationTime) const;

			virtual void ExecutePostTickFunction();

		protected:
			virtual void UpdateNextTime(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask);

		protected:
			std::string m_Name;

			bool m_bIsWorking;

			double m_CurrentTime;
			double m_NextTime;

			std::function<std::optional<std::reference_wrapper<NodeBase>>(std::vector<std::reference_wrapper<NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> m_GetNextNodeFunction;
			std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> m_DelayFunction;

			std::vector<std::reference_wrapper<NodeBase>> m_NextNodes;

			NodeBaseStatistics m_StatisticsData;
		};
	}
}
