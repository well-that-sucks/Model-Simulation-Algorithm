#include <algorithm>

#include "ModelExamples.h"

#include "../GraphNodes/NodeGenerator.h"
#include "../GraphNodes/NodeProcessor.h"
#include "../GraphNodes/NodeTransition.h"

#include "../Tasks/TaskBase.h"

#include "../../../Utils/Random/Random.h"
#include "../../../Utils/Random/Distributions/DistributionErlang.h"
#include "../../../Utils/Random/Distributions/DistributionExponential.h"
#include "../../../Utils/Random/Distributions/DistributionGaussian.h"
#include "../../../Utils/Random/Distributions/DistributionUniform.h"

void Model::ModelExamples::RunLab2Model1(double SimulationTime)
{
	Nodes::NodeGenerator Generator("CREATOR");
	Nodes::NodeProcessor Processor("PROCESSOR", 5);

	std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> DelayFunction =
		[](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(5); };

	Generator.SetDelayFunction(DelayFunction);
	Processor.SetDelayFunction(DelayFunction);

	Generator.AddNextNode(Processor);

	Model Model1({ std::reference_wrapper<Nodes::NodeBase>(Generator), std::reference_wrapper<Nodes::NodeBase>(Processor) });
	Model1.Simulate(SimulationTime);
}

void Model::ModelExamples::RunLab2Model2(double SimulationTime)
{
	Nodes::NodeGenerator Generator("CREATOR");
	Generator.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(5); });

	Nodes::NodeProcessor Processor1("PROCESSOR_1", 3);
	Nodes::NodeProcessor Processor2("PROCESSOR_2", 1);
	Nodes::NodeProcessor Processor3("PROCESSOR_3", 4);

	Processor1.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(5); });
	Processor2.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(3); });
	Processor3.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(7); });

	Generator.AddNextNode(Processor1.AddNextNode(Processor2.AddNextNode(Processor3)));

	Model Model2({ Generator, Processor1, Processor2, Processor3 });
	Model2.Simulate(SimulationTime);
}

void Model::ModelExamples::RunLab2Model3(double SimulationTime)
{
	Nodes::NodeGenerator Generator1("CREATOR");
	Generator1.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(3); });

	Nodes::NodeProcessor MultiProcessor("MULTICHANNEL_PROCESSOR", 5, 2);
	MultiProcessor.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(5); });

	Generator1.AddNextNode(MultiProcessor);

	Model Model3({ Generator1, MultiProcessor });

	Model3.Simulate(SimulationTime);
}

void Model::ModelExamples::RunModelCarBank(double SimulationTime)
{
	Nodes::NodeGenerator Generator("CREATOR");
	Generator.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(0.5); });

	Nodes::NodeProcessor Processor1("MANAGER_1", 3);
	Nodes::NodeProcessor Processor2("MANAGER_2", 3);
	
	std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> DelayFunction =
		[](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionGaussian::GenerateByExpectedValue(1, 0.3); };

	std::function<bool(Nodes::NodeProcessor& Node)> PostTickFunction = 
	[Nodes = std::vector<std::reference_wrapper<Nodes::NodeProcessor>>{ Processor1, Processor2 }](Nodes::NodeProcessor& UpdatedNode)
	{
		bool Result = false;

		std::for_each(Nodes.begin(), Nodes.end(),
			[&UpdatedNode, &Result](Nodes::NodeProcessor& Node)
			{
				const int QueueTransferThreshold = 2;

				if (UpdatedNode.GetQueueSize() - Node.GetQueueSize() >= QueueTransferThreshold)
				{
					Result = true;
					Node.InAction(UpdatedNode.PopFrontTask());
				}
			});

		return Result;
	};

	std::function<std::optional<std::reference_wrapper<Nodes::NodeBase>>(std::vector<std::reference_wrapper<Nodes::NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> GetNextNodeFunction =
		[](std::vector<std::reference_wrapper<Nodes::NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)->std::optional<std::reference_wrapper<Nodes::NodeBase>>
		{
			std::vector<std::reference_wrapper<Nodes::NodeBase>>::iterator MinElementIter = std::ranges::min_element(NodeCandidates.begin(), NodeCandidates.end(),
				[](const Nodes::NodeBase& CurrentNode, const Nodes::NodeBase& NextNode)
				{
					// We are sure that following nodes will be processors
					return (CurrentNode.GetQueueSize() + CurrentNode.IsWorking()) < (NextNode.GetQueueSize() + NextNode.IsWorking());
				});

			if (MinElementIter != NodeCandidates.end())
			{
				return *MinElementIter;
			}

			return std::nullopt;
		};
	const double FirstArrivalTime = 0.1;
	Generator.SetNextTime(FirstArrivalTime);

	Generator.SetGetNextNodeFunction(GetNextNodeFunction);

	Processor1.SetDelayFunction(DelayFunction);
	Processor1.SetPostTickFunction(PostTickFunction);

	Processor2.SetDelayFunction(DelayFunction);
	Processor2.SetPostTickFunction(PostTickFunction);

	Generator.AddNextNode(Processor1).AddNextNode(Processor2);

	Model Model4({ Generator, Processor1, Processor2 });
	Model4.Simulate(SimulationTime);
}

void Model::ModelExamples::RunModelHospital(double SimulationTime)
{
	Nodes::NodeGenerator Generator("CREATOR");

	std::function<double()> DelayTask1 = []() { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(15); };
	std::function<double()> DelayTask2 = []() { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(40); };
	std::function<double()> DelayTask3 = []() { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(30); };

	std::vector<std::pair<Tasks::TaskBase, double>> TaskProbabilities = 
		{ {Tasks::TaskBase("PatientType1").SetTaskSpecificDelayFunction(DelayTask1), 0.5},
		{Tasks::TaskBase("PatientType2").SetTaskSpecificDelayFunction(DelayTask2), 0.1}, 
		{Tasks::TaskBase("PatientType3").SetTaskSpecificDelayFunction(DelayTask3), 0.4} };


	Generator.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionExponential::GenerateByExpectedValue(15); });
	Generator.SetTaskDistribution(TaskProbabilities);

	////////
	Nodes::NodeProcessor MultiprocessorDoctor("MULTIPROCESS_DOCTOR", NO_LIMITS, 2);

	std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> DelayFunctionDoctor =
		[](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) 
		{ 
			if (const std::function<double()> DelayFunction = ExecutedTask->GetTaskSpecificDelayFunction())
			{
				return DelayFunction();
			}

			throw(std::logic_error("Unknown task type"));
			//return Random::Distributions::DistributionExponential::GenerateByExpectedValue(30); // In case we don't have a task with overriding function
		};

	std::map<std::string, std::string> TaskToNodeTypesMapping = { {"PatientType1", "MULTIPROCESS_CARETAKER"}, {"PatientType2", "TRANSITION_LAB"}, {"PatientType3", "TRANSITION_LAB"} };
	std::function<std::optional<std::reference_wrapper<Nodes::NodeBase>>
	(std::vector<std::reference_wrapper<Nodes::NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> GetNextNodeFunctionDoctor =
		[&TaskToNodeTypesMapping](std::vector<std::reference_wrapper<Nodes::NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)->std::optional<std::reference_wrapper<Nodes::NodeBase>>
		{
			if (!ExecutedTask)
			{
				return std::nullopt;
			}

			std::map<std::string, std::string>::iterator TargetElement = TaskToNodeTypesMapping.find(ExecutedTask->GetType());

			if (TargetElement != TaskToNodeTypesMapping.end())
			{
				std::vector<std::reference_wrapper<Nodes::NodeBase>>::iterator NextElement =
					std::find_if(NodeCandidates.begin(), NodeCandidates.end(), 
						[&TargetName = TargetElement->second](const Nodes::NodeBase& Node) { return Node.GetName() == TargetName; });

				if (NextElement != NodeCandidates.end() && NextElement->get().IsAvailable())
				{
					return *NextElement;
				}
			}

			return std::nullopt;
		};

	std::function<std::shared_ptr<Tasks::TaskBase>& (std::deque<std::shared_ptr<Tasks::TaskBase>>&)> GetNextTaskFunctionDoctor =
		[](std::deque<std::shared_ptr<Tasks::TaskBase>>& TaskQueue)->std::shared_ptr<Tasks::TaskBase>&
		{
			std::deque<std::shared_ptr<Tasks::TaskBase>>::iterator NextTaskIter =
				std::find_if(TaskQueue.begin(), TaskQueue.end(), [](const std::shared_ptr<Tasks::TaskBase>& Task) { return Task->GetType() == "PatientType1"; });

			if (NextTaskIter != TaskQueue.end())
			{
				return *NextTaskIter;
			}

			return *TaskQueue.begin();
		};

	MultiprocessorDoctor.SetDelayFunction(DelayFunctionDoctor);
	MultiprocessorDoctor.SetGetNextNodeFunction(GetNextNodeFunctionDoctor);
	MultiprocessorDoctor.SetGetNextTaskFunction(GetNextTaskFunctionDoctor);
	////////

	////////
	Nodes::NodeProcessor MultiprocessorCaretaker("MULTIPROCESS_CARETAKER", NO_LIMITS, 3);

	std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> DelayFunctionCaretaker =
		[](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionUniform::GenerateByExpectedValue(3, 8); };

	MultiprocessorCaretaker.SetDelayFunction(DelayFunctionCaretaker);
	///////

	////////
	std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> DelayFunctionTransitions =
		[](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionUniform::GenerateByExpectedValue(2, 5); };

	Nodes::NodeTransition TransitionToLab("TRANSITION_LAB");
	Nodes::NodeTransition TransitionToAdmissionDeparture("TRANSITION_ADMISSION_DEPARTURE");

	TransitionToLab.SetDelayFunction(DelayFunctionTransitions);
	TransitionToAdmissionDeparture.SetDelayFunction(DelayFunctionTransitions);
	////////

	////////
	Nodes::NodeProcessor Registry("PROC_REGISTRY", NO_LIMITS);
	Registry.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) {return Random::Distributions::DistributionErlang::GenerateByExpectedValue(4.5, 3); });
	///////
	
	//////
	Nodes::NodeProcessor MultiprocessorLabworker("MULTIPROCESS_LABWORKER", NO_LIMITS, 2);

	std::function<double(const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> DelayFunctionLabworker =
		[](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionErlang::GenerateByExpectedValue(4, 2); };

	std::function<std::optional<std::reference_wrapper<Nodes::NodeBase>>
	(std::vector<std::reference_wrapper<Nodes::NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> GetNextNodeFunctionLabworker =
		[](std::vector<std::reference_wrapper<Nodes::NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)->std::optional<std::reference_wrapper<Nodes::NodeBase>>
		{
			if (!ExecutedTask)
			{
				return std::nullopt;
			}

			if (ExecutedTask->GetType() == "PatientType2")
			{
				ExecutedTask->SetType("PatientType1");
				std::vector<std::reference_wrapper<Nodes::NodeBase>>::iterator NextElement =
					std::find_if(NodeCandidates.begin(), NodeCandidates.end(),
						[](const Nodes::NodeBase& Node) { return Node.GetName() == "TRANSITION_ADMISSION_DEPARTURE"; });

				if (NextElement != NodeCandidates.end())
				{
					return std::optional<std::reference_wrapper<Nodes::NodeBase>>{*NextElement};
				}
			}

			return std::nullopt;
		};

	MultiprocessorLabworker.SetDelayFunction(DelayFunctionLabworker);
	MultiprocessorLabworker.SetGetNextNodeFunction(GetNextNodeFunctionLabworker);
	//////

	Generator.AddNextNode(MultiprocessorDoctor.AddNextNode(MultiprocessorCaretaker).
		AddNextNode(TransitionToLab.AddNextNode(Registry.AddNextNode(MultiprocessorLabworker.
		AddNextNode(TransitionToAdmissionDeparture.AddNextNode(MultiprocessorDoctor))))));

	Model Model5({ Generator, MultiprocessorDoctor, MultiprocessorCaretaker, TransitionToLab, TransitionToAdmissionDeparture, Registry, MultiprocessorLabworker });
	Model5.Simulate(SimulationTime);
}

void Model::ModelExamples::RunModelCoursework(double SimulationTime)
{
	Nodes::NodeGenerator Generator1("CREATOR_1");
	Nodes::NodeGenerator Generator2("CREATOR_2");

	std::vector<std::pair<Tasks::TaskBase, double>> TaskProbabilities1 =
	{ {Tasks::TaskBase("MessageType1"), 1} };

	std::vector<std::pair<Tasks::TaskBase, double>> TaskProbabilities2 =
	{ {Tasks::TaskBase("MessageType2"), 1} };

	Generator1.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionUniform::GenerateByExpectedValue(8, 22); });
	Generator1.SetTaskDistribution(TaskProbabilities1);

	Generator2.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionUniform::GenerateByExpectedValue(8, 22); });
	Generator2.SetTaskDistribution(TaskProbabilities2);

	Nodes::NodeProcessor Processor("PROC_BUFFER", NO_LIMITS);
	Processor.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return 7; });

	Nodes::NodeProcessor OutRoute1("OUTPUT_ROUTE_1", 2);
	Nodes::NodeProcessor OutRoute2("OUTPUT_ROUTE_2", 2);

	OutRoute1.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionUniform::GenerateByExpectedValue(10, 20); });
	OutRoute2.SetDelayFunction([](const std::shared_ptr<Tasks::TaskBase>& ExecutedTask) { return Random::Distributions::DistributionUniform::GenerateByExpectedValue(10, 20); });

	std::map<std::string, std::string> TaskToNodeTypesMapping = { {"MessageType1", "OUTPUT_ROUTE_1"}, {"MessageType2", "OUTPUT_ROUTE_2"} };
	std::function<std::optional<std::reference_wrapper<Nodes::NodeBase>>
		(std::vector<std::reference_wrapper<Nodes::NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)> GetNextNodeFunctionProcesspr =
		[&TaskToNodeTypesMapping](std::vector<std::reference_wrapper<Nodes::NodeBase>>& NodeCandidates, const std::shared_ptr<Tasks::TaskBase>& ExecutedTask)->std::optional<std::reference_wrapper<Nodes::NodeBase>>
		{
			if (!ExecutedTask)
			{
				return std::nullopt;
			}

			std::map<std::string, std::string>::iterator TargetElement = TaskToNodeTypesMapping.find(ExecutedTask->GetType());

			if (TargetElement != TaskToNodeTypesMapping.end())
			{
				std::vector<std::reference_wrapper<Nodes::NodeBase>>::iterator NextElement =
					std::find_if(NodeCandidates.begin(), NodeCandidates.end(),
						[&TargetName = TargetElement->second](const Nodes::NodeBase& Node) { return Node.GetName() == TargetName; });

				if (NextElement != NodeCandidates.end() && NextElement->get().IsAvailable())
				{
					return *NextElement;
				}
			}

			return std::nullopt;
		};

	Processor.SetGetNextNodeFunction(GetNextNodeFunctionProcesspr);

	std::function<bool(Nodes::NodeProcessor& Node, const std::shared_ptr<Tasks::TaskBase>&)> InActionConditionFunction =
		[&OutRoute1, &OutRoute2](Nodes::NodeProcessor& UpdatedNode, const std::shared_ptr<Tasks::TaskBase>& InTask)
		{
		const int TaskTypeNum = UpdatedNode.GetSpecificTaskNum(
			[&InTask](const std::shared_ptr<Tasks::TaskBase>& Task) { 
				if (Task && InTask)
				{
					return Task->GetType() == InTask->GetType();
				}
			});
			
			if (InTask->GetType() == "MessageType1")
			{
				const int RouteTypeNum = OutRoute1.GetSpecificTaskNum(
					[&InTask](const std::shared_ptr<Tasks::TaskBase>& Task) {
						if (Task && InTask)
						{
							return Task->GetType() == InTask->GetType();
						}
					});

				if (TaskTypeNum + RouteTypeNum >= 3)
				{
					return false;
				}
			}

			if (InTask->GetType() == "MessageType2")
			{
				const int RouteTypeNum = OutRoute2.GetSpecificTaskNum(
					[&InTask](const std::shared_ptr<Tasks::TaskBase>& Task) {
						if (Task && InTask)
						{
							return Task->GetType() == InTask->GetType();
						}
					});

				if (TaskTypeNum + RouteTypeNum >= 3)
				{
					return false;
				}
			}

			return true;
		};

	Processor.SetInActionCondition(InActionConditionFunction);

	Generator1.AddNextNode(Processor.AddNextNode(OutRoute1).AddNextNode(OutRoute2));
	Generator2.AddNextNode(Processor.AddNextNode(OutRoute1).AddNextNode(OutRoute2));

	Model Model6({ Generator1, Generator2, Processor, OutRoute1, OutRoute2 });
	Model6.Simulate(SimulationTime);
}
