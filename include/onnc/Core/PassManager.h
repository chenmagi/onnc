//===- PassManager.h -----------------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ONNC_CORE_PASS_MANAGER_H
#define ONNC_CORE_PASS_MANAGER_H
#include <onnc/Core/Pass.h>
#include <onnc/Core/ModulePass.h>
#include <onnc/Core/PassInfo.h>
#include <onnc/Core/PassRegistry.h>
#include <onnc/IR/Module.h>
#include <onnc/ADT/Digraph.h>
#include <map>

namespace onnc {

class TargetBackend;

/** \class onnc::PassManager
 *  \brief stores a set of passes and run them.
 */
class PassManager
{
public:
  typedef std::deque<Pass::AnalysisID> ExecutionOrder;

  /// run state
  struct State {
    ExecutionOrder execution;
    bool changed; // module changed or not
    bool executed; // current pass is executed (true) or skipped (false)
    Pass* pass; // current pass

    State() : execution(), changed(false), executed(false), pass(nullptr) { }
  };

public:
  /// Use global-wise GetPassRegistry().
  /// @see PassRegistry
  PassManager();

  /// Use external PassRegistry
  PassManager(PassRegistry& pRegistry);

  /// PassManager is responsible to release all passes being added and created
  /// during adding passes.
  ~PassManager();

  /// add pPass to:
  /// 1. dependency graph (ignore if pPass.getPassID() has been added before)
  /// 2. execution queue.
  void add(Pass* pPass);

  void add(Pass* pPass, TargetBackend* pBackend);

  void add(Pass* pPass, State& pState);

  void add(Pass* pPass, TargetBackend* pBackend, State& pState);

  /// PassManager::run behaviour:
  /// 1. If a pass dependents on other passes (cyclic dependency is disallowed),
  ///    PassManager guarantees all dependencies are executed before that pass.
  ///
  /// 2. PassManager doesn't re-execute a pass if that pass's dependencies are
  ///    unchanged.
  ///
  /// 3. If a pass return retry, PassManager re-executes that pass, but whether
  ///    re-executes it's dependencies or not follows rule 2.

  /// run all passes
  /// @retval false A pass return failure.
  bool run(Module& pModule);

  /// run one pass
  /// @return The pass run
  bool step(Module& pModule);

  /// continue to run all passes
  bool run(Module& pModule, State& pState);

  /// run one pass
  /// @return The pass run
  bool step(Module& pModule, State& pState);

  bool needRun(Pass& pPass, Module& pModule);

  void initRunState(Module& pModule, State& pState);

  /// @return The number of registered passes.
  unsigned int size() const;

  const State& state() const { return m_RunState; }

  Pass* lookup(Pass::AnalysisID pID);

  PassRegistry* getPassRegistry() { return m_pPassRegistry; }

  void printState(const State& pState, OStream& pOS) const;

  void dumpState(const State& pState) const;

private:
  struct DepNode : public DigraphNode<DepNode>
  {
  public:
    DepNode(Pass* pPass) : pass(pPass) { }

    // since passes are delegated, we release memory in Digraph destruction.
    ~DepNode() { delete pass; }

  public:
    Pass *pass;
  };

  /// The start pass for lattice.
  struct StartPass : public ModulePass
  {
  public:
    static char ID;

  public:
    StartPass() : ModulePass(ID) { }

    Pass::ReturnType runOnModule(Module &pModule) override
    {
      return kModuleNoChanged;
    }

    StringRef getPassName() const override { return "start"; }
  };

  typedef Digraph<DepNode> PassDependencyLattice;

  typedef std::map<Pass::AnalysisID, DepNode*> AvailableAnalysisMap;

private:
  /// Dependency graph operator: find a node
  DepNode* findNode(Pass::AnalysisID pID) const;

  /// Dependency graph operator: add a node
  DepNode* addNode(Pass& pPass);

  /// Dependency graph operator
  /// @retval true If the pass @ref pID has been added.
  bool hasAdded(Pass::AnalysisID pID) const;

  /// Dependency graph operator
  /// Add a pass to internal dependency graph. Pass is added in DSF order.
  ///
  /// @note The function can be called multiple times with the same pPass
  ///       without side effect.
  void addPassToDependencyGraph(Pass* pPass, TargetBackend* pBackend);

  /// Run the pass
  Pass::ReturnType doRun(Pass& pPass, Module& pModule);

  void UpdateExecutionOrder(ExecutionOrder& pOrder);

  /// Add a pass to execution queue. If a pass depends on other passes, the
  /// dependent passes are added to exe queue unconditionally.
  void addPassToExeQueue(Pass* pPass, State& pState);

private:
  PassRegistry* m_pPassRegistry;

  // a graph describes the dependencies among passes.
  PassDependencyLattice m_Dependencies;

  // A map from ID to node in dependency graph.
  AvailableAnalysisMap m_AvailableAnalysis;

  State m_RunState;

  DepNode *m_pStart;

  // Executing time step, it is reset on initRunState. PassManager uses time
  // step to decide whether to execute a pass or not. If PassManager execute a
  // pass, it also updates Pass' time step.
  unsigned m_TimeStep;
};

} // namespace of onnc

#endif
