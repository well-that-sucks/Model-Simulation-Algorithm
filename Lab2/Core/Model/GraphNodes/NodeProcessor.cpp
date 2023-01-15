#include <algorithm>
#include <ranges>
#include <iomanip>

#include "NodeProcessor.h"

Model::Nodes::NodeProcessor::NodeProcessor(std::string NodeName, int MaxQueueLength)
	: NodeProcessor(NodeName, MaxQueueLength, 1)
{
}

Model::Nodes::NodeProcessor::NodeProcessor(std::string NodeName, int MaxQueueLength, int SubprocessorsNum)
	: NodeBase(NodeName), m_MaxQueueLength(MaxQueueLength)
{
	for (int i = 0; i < SubprocessorsNum; ++i)
	{
		m_Subprocesses.emplace_back(nullptr, m_CurrentTime);
	}

	m_bIsWorking = false;
}

void Model::Nodes::NodeProcessor::InAction(std::shared_ptr<Tasks::TaskBase> InTask)
{
	NodeBase::InAction(InTask);

	// If processor has any idling process...
	if (std::optional<std::reference_wrapper<SubProcess>> IdlingSubprocess = GetIdlingSubprocess())
	{
		std::shared_ptr<Tasks::TaskBase>& CurrentTask = IdlingSubprocess->get().Task;
		CurrentTask = InTask;
		UpdateNextTime(CurrentTask);
	}
	else // if all processes are busy...
	{
		// If the queue isn't full - add the task to it
		if (m_MaxQueueLength == NO_LIMITS || (m_TaskQueue.size() < m_MaxQueueLength))
		{
			m_TaskQueue.push_back(InTask);
		}
		else // otherwise report the failure
		{
			++m_StatisticsData.FailureRate;

			++m_StatisticsData.PacketsTotal;
			m_SpecificStatisticsData.TotalTaskLifetime += m_CurrentTime - InTask->GetInitialTime();
		}
	}
}

void Model::Nodes::NodeProcessor::OutAction()
{
	if (GetFreeSlotsNum() == m_Subprocesses.size())
	{
		return;
	}

	NodeBase::OutAction();

	double MinTimeNext = INFINITY;
	int MinIndex = -1;

	for (int i = 0; i < m_Subprocesses.size(); ++i)
	{
		if (m_Subprocesses[i].Task && m_Subprocesses[i].TimeNext < MinTimeNext)
		{
			MinTimeNext = m_Subprocesses[i].TimeNext;
			MinIndex = i;
		}
	}

	if (MinIndex != -1)
	{
		SubProcess& MinTimeEntry = m_Subprocesses[MinIndex];
		if (std::optional<std::reference_wrapper<NodeBase>> NextNode = GetNextNode(MinTimeEntry.Task))
		{
			NextNode->get().InAction(MinTimeEntry.Task);
			++m_StatisticsData.TotalPasses;
		}
		else
		{
			++m_StatisticsData.TotalPacketsLeavingModel;
			m_SpecificStatisticsData.TotalTaskLifetime += m_CurrentTime - MinTimeEntry.Task->GetInitialTime();
		}

		if (m_TaskQueue.empty())
		{
			MinTimeEntry.Task = nullptr;
			m_bIsWorking = GetFreeSlotsNum() < m_Subprocesses.size();
		}
		else
		{
			MinTimeEntry.Task = GetNextTask();
			UpdateNextTime(MinTimeEntry.Task);
		}
	}
	
}

void Model::Nodes::NodeProcessor::SetCurrentTime(double Time)
{
	m_SpecificStatisticsData.AverageQueueLength += m_TaskQueue.size() * (Time - m_CurrentTime);
	NodeBase::SetCurrentTime(Time);	
}

double Model::Nodes::NodeProcessor::GetNextTime() const
{
	double MinTimeNext = INFINITY;
	int MinIndex = -1;

	for (int i = 0; i < m_Subprocesses.size(); ++i)
	{
		if (m_Subprocesses[i].Task && m_Subprocesses[i].TimeNext < MinTimeNext)
		{
			MinTimeNext = m_Subprocesses[i].TimeNext;
			MinIndex = i;
		}
	}

	return MinTimeNext;

	//if (MinIndex == -1)
	//{
	//	throw(std::logic_error("Model was trying to propagate in the opposite direction"));
	//}

	//return m_Subprocesses[MinIndex].TimeNext;
}

int Model::Nodes::NodeProcessor::GetQueueSize() const
{
	return static_cast<int>(m_TaskQueue.size());
}

std::ostringstream Model::Nodes::NodeProcessor::GetNodeInfo() const
{
	std::ostringstream StringStream = NodeBase::GetNodeInfo();

	StringStream << "Queue size: " << m_TaskQueue.size() << std::endl;

	if (m_Subprocesses.size() > 1)
	{
		StringStream << "Idling subprocesses: " << GetFreeSlotsNum() << "/" << m_Subprocesses.size() << std::endl;
	}

	return StringStream;
}

std::ostringstream Model::Nodes::NodeProcessor::GetNodeStatistics(double TotalSimulationTime) const
{
	std::ostringstream StringStream = NodeBase::GetNodeStatistics(TotalSimulationTime);

	StringStream << "Average queue length: " << std::fixed << std::setprecision(3) << m_SpecificStatisticsData.AverageQueueLength / m_StatisticsData.TotalWorkloadTime << std::endl;
	StringStream << "Average time between in and out: " << std::fixed << std::setprecision(3) << m_StatisticsData.TotalWorkloadTime / m_StatisticsData.PacketsTotal << std::endl;

	if (m_PostTickFunction)
	{
		StringStream << "Times task was transferred to another node: " << m_SpecificStatisticsData.QueueSwappingOccasions << std::endl;
	}

	return StringStream;
}

bool Model::Nodes::NodeProcessor::IsAvailable() const
{
	return (m_MaxQueueLength == NO_LIMITS) || (m_TaskQueue.size() < m_MaxQueueLength) || (!m_MaxQueueLength && GetFreeSlotsNum());
}

void Model::Nodes::NodeProcessor::ExecutePostTickFunction()
{
	if (m_PostTickFunction)
	{
		m_SpecificStatisticsData.QueueSwappingOccasions += !!m_PostTickFunction(*this);
	}
}

void Model::Nodes::NodeProcessor::SetPostTickFunction(std::function<bool(Model::Nodes::NodeProcessor& Node)> InFunction)
{
	m_PostTickFunction = std::move(InFunction);
}

void Model::Nodes::NodeProcessor::SetGetNextTaskFunction(std::function<std::shared_ptr<Tasks::TaskBase>&(std::deque<std::shared_ptr<Tasks::TaskBase>>&)> GetNextTask)
{
	m_GetNextTask = std::move(GetNextTask);
}

std::shared_ptr<Model::Tasks::TaskBase> Model::Nodes::NodeProcessor::PopFrontTask()
{
	std::shared_ptr<Model::Tasks::TaskBase> Task = m_TaskQueue.front();
	m_TaskQueue.pop_front();

	return Task;
}

void Model::Nodes::NodeProcessor::UpdateNextTime(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)
{
	std::vector<SubProcess>::iterator SubprocessIter = std::find_if(m_Subprocesses.begin(), m_Subprocesses.end(), 
		[&ExecutedTask](const SubProcess& Entry) 
		{ 
			return Entry.Task == ExecutedTask; 
		}
	);

	if (SubprocessIter != m_Subprocesses.end())
	{
		SubProcess& SubprocessEntry = *SubprocessIter;
		SubprocessEntry.TimeNext = m_CurrentTime + m_DelayFunction(ExecutedTask);
		m_NextTime = GetNextTime();
		m_bIsWorking = true;
	}
}

std::shared_ptr<Model::Tasks::TaskBase> Model::Nodes::NodeProcessor::GetNextTask()
{
	if (m_GetNextTask)
	{
		std::shared_ptr<Tasks::TaskBase> NextTask = m_GetNextTask(m_TaskQueue);
		std::erase(m_TaskQueue, NextTask);

		return NextTask;
	}

	return PopFrontTask();
}

std::optional<std::reference_wrapper<Model::Nodes::NodeProcessor::SubProcess>> Model::Nodes::NodeProcessor::GetIdlingSubprocess()
{
	double MinTimeNext = INFINITY;
	int MinIndex = -1;

	for (int i = 0; i < m_Subprocesses.size(); ++i)
	{
		if (!m_Subprocesses[i].Task && m_Subprocesses[i].TimeNext < MinTimeNext)
		{
			MinTimeNext = m_Subprocesses[i].TimeNext;
			MinIndex = i;
		}
	}
	
	if (MinIndex == -1)
	{
		return std::nullopt;
	}

	return m_Subprocesses[MinIndex];
}

inline int Model::Nodes::NodeProcessor::GetFreeSlotsNum() const
{
	return static_cast<int>(std::count_if(m_Subprocesses.cbegin(), m_Subprocesses.cend(), [](const SubProcess& Entry) { return Entry.Task == nullptr; }));
}
