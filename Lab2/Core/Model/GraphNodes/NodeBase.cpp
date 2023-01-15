#include <algorithm>
#include <iomanip>

#include "NodeBase.h"

Model::Nodes::NodeBase::NodeBase()
	: NodeBase("")
{
}

Model::Nodes::NodeBase::NodeBase(std::string Name)
	: m_Name(Name), m_CurrentTime(0), m_NextTime(0), m_bIsWorking(true)
{
}

void Model::Nodes::NodeBase::InAction(std::shared_ptr<Tasks::TaskBase>)
{
	if (m_StatisticsData.LastInActionTime)
	{
		m_StatisticsData.TotalDeltaInActionTime += m_CurrentTime - m_StatisticsData.LastInActionTime;
	}

	m_StatisticsData.LastInActionTime = m_CurrentTime;
}

void Model::Nodes::NodeBase::OutAction()
{
	if (m_StatisticsData.LastOutActionTime)
	{
		m_StatisticsData.TotalDeltaOutActionTime += m_CurrentTime - m_StatisticsData.LastOutActionTime;
	}
	
	m_StatisticsData.LastOutActionTime = m_CurrentTime;

	++m_StatisticsData.PacketsTotal;
}

std::optional<std::reference_wrapper<Model::Nodes::NodeBase>> Model::Nodes::NodeBase::GetNextNode(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)
{
	// If we have specific function overriding default behavior - use it
	if (m_GetNextNodeFunction)
	{
		return m_GetNextNodeFunction(m_NextNodes, ExecutedTask);
	}
	// Otherwise - return the node with the smallest NextTime

	double MinTimeNext = INFINITY;
	int MinIndex = -1;

	for (int i = 0; i < m_NextNodes.size(); ++i)
	{
		if (m_NextNodes[i].get().GetNextTime() <= MinTimeNext)
		{
			MinTimeNext = m_NextNodes[i].get().GetNextTime();
			MinIndex = i;
		}
	}

	//std::vector<std::reference_wrapper<Model::Nodes::NodeBase>>::iterator MinNodeIter = 
	//	std::min_element(m_NextNodes.begin(), m_NextNodes.end(), 
	//		[](const NodeBase& LeftNode, const NodeBase& RightNode) 
	//		{ return (LeftNode.IsAvailable() && LeftNode.GetNextTime() <= RightNode.GetNextTime()); });

	if (MinIndex == -1)
	{
		return std::nullopt;
	}

	return m_NextNodes[MinIndex];
}

Model::Nodes::NodeBase& Model::Nodes::NodeBase::AddNextNode(NodeBase& NextNode)
{
	m_NextNodes.push_back(std::reference_wrapper{ NextNode });
	return *this;
}

std::string Model::Nodes::NodeBase::GetName() const
{
	return m_Name;
}

double Model::Nodes::NodeBase::GetCurrentTime() const
{
	return m_CurrentTime;
}

double Model::Nodes::NodeBase::GetNextTime() const
{
	return m_NextTime;
}

void Model::Nodes::NodeBase::SetCurrentTime(double Time)
{
	m_StatisticsData.TotalWorkloadTime += m_bIsWorking * (Time - m_CurrentTime);

	m_CurrentTime = Time;
}

void Model::Nodes::NodeBase::SetNextTime(double Time)
{
	m_NextTime = Time;
}

void Model::Nodes::NodeBase::SetGetNextNodeFunction(std::function<std::optional<std::reference_wrapper<Model::Nodes::NodeBase>>(
	std::vector<std::reference_wrapper<Model::Nodes::NodeBase>>& NodeCandidates,
	const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> GetNextNodeFunction)
{
	m_GetNextNodeFunction = std::move(GetNextNodeFunction);
}

void Model::Nodes::NodeBase::SetDelayFunction(std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> DelayFunction)
{
	m_DelayFunction = std::move(DelayFunction);
}

bool Model::Nodes::NodeBase::IsWorking() const
{
	return m_bIsWorking;
}

std::ostringstream Model::Nodes::NodeBase::GetNodeInfo() const
{
	std::ostringstream StringStream;
	StringStream << "Node name: " << m_Name << std::endl;
	StringStream << "Time current: " << std::fixed << std::setprecision(3) << m_CurrentTime << std::endl;
	StringStream << "Time next: " << std::fixed << std::setprecision(3) << m_NextTime << std::endl;
	StringStream << "Is working: " << m_bIsWorking << std::endl;
	return StringStream;
}

std::ostringstream Model::Nodes::NodeBase::GetNodeStatistics(double TotalSimulationTime) const
{
	std::ostringstream StringStream;

	StringStream << "Node name: " << m_Name << std::endl;
	StringStream << "Packets processed total: " << m_StatisticsData.PacketsTotal << std::endl;
	StringStream << "Failure rate: " << std::fixed << std::setprecision(2) << m_StatisticsData.FailureRate / static_cast<double>(m_StatisticsData.PacketsTotal) * 100.0 << "%" << std::endl;
	StringStream << "Amount of packets passed successfully: " << m_StatisticsData.TotalPasses << std::endl;
	StringStream << "Packets left model: " << m_StatisticsData.TotalPacketsLeavingModel << std::endl;

	StringStream << "Time spent working: " << std::fixed << std::setprecision(3) << m_StatisticsData.TotalWorkloadTime << std::endl;
	StringStream << "Average time working: " << std::fixed << std::setprecision(2) << m_StatisticsData.TotalWorkloadTime / TotalSimulationTime * 100.0 << "%" << std::endl;

	const double AveragePacketInTime = m_StatisticsData.TotalDeltaInActionTime / m_StatisticsData.PacketsTotal;
	const double AveragePacketOutTime = m_StatisticsData.TotalDeltaOutActionTime / (m_StatisticsData.TotalPasses + m_StatisticsData.TotalPacketsLeavingModel);

	StringStream << "Average time between packets entering a process: " << std::fixed << std::setprecision(3) << AveragePacketInTime << std::endl;
	StringStream << "Average time between packets leaving a process: " << std::fixed << std::setprecision(3) << AveragePacketOutTime << std::endl;

	return StringStream;
}

void Model::Nodes::NodeBase::ExecutePostTickFunction()
{
}

void Model::Nodes::NodeBase::UpdateNextTime(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)
{
	m_NextTime = m_CurrentTime + m_DelayFunction(ExecutedTask);
	m_bIsWorking = true;
}

void Model::Nodes::NodeBase::ReportFailure(const std::shared_ptr<Tasks::TaskBase>& FailedTask)
{
	++m_StatisticsData.FailureRate;
	++m_StatisticsData.PacketsTotal;
}
