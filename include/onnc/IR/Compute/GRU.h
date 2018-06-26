//===- GRU.h --------------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ONNC_IR_COMPUTE_OPERATOR_GRU_H
#define ONNC_IR_COMPUTE_OPERATOR_GRU_H
#include <onnc/IR/ComputeOperator.h>
#include <onnc/IR/ComputeVisitor.h>
#include <onnc/IR/Compute/Attributes.h>
#include <onnc/Support/IOStream.h>

namespace onnc {

class GRU : public ComputeOperator
{
public:
  enum IOConst {
    kX = 0,
    kW = 1,
    kR = 2,
    kB = 3,
    kSequenceLens = 4,
    kInitialH = 5,
    kY = 0,
    kYH = 1
  };

public:
  GRU();

  GRU(const FloatsAttr& pActivationAlpha,
      const FloatsAttr& pActivationBeta,
      const StringsAttr& pActivations,
      const FloatAttr& pClip,
      const StringAttr& pDirection,
      const IntAttr& pHiddenSize,
      const IntAttr& pLinearBeforeReset,
      const IntAttr& pOutputSequence);

  ~GRU() { }

  const FloatsAttr& getActivationAlpha() const { return m_ActivationAlpha; }

  const FloatsAttr& getActivationBeta() const { return m_ActivationBeta; }

  const StringsAttr& getActivations() const { return m_Activations; }

  const FloatAttr& getClip() const { return m_Clip; }

  const StringAttr& getDirection() const { return m_Direction; }

  const IntAttr& getHiddenSize() const { return m_HiddenSize; }

  const IntAttr& getLinearBeforeReset() const { return m_LinearBeforeReset; }

  const IntAttr& getOutputSequence() const { return m_OutputSequence; }

  Tensor* getInput(unsigned int pIdx) override { return static_cast<Tensor*>(m_Inputs[pIdx]); }

  const Tensor* getInput(unsigned int pIdx) const override { return static_cast<Tensor*>(m_Inputs[pIdx]); }

  Tensor* getOutput(unsigned int pIdx) override { return static_cast<Tensor*>(m_Outputs[pIdx]); }

  const Tensor* getOutput(unsigned int pIdx) const override { return static_cast<Tensor*>(m_Outputs[pIdx]); }

  Tensor* getX() { return getInput(kX); }

  Tensor* getW() { return getInput(kW); }

  Tensor* getR() { return getInput(kR); }

  Tensor* getB() { return getInput(kB); }

  Tensor* getSequenceLens() { return getInput(kSequenceLens); }

  Tensor* getInitialH() { return getInput(kInitialH); }

  Tensor* getY() { return getOutput(kY); }

  Tensor* getYH() { return getOutput(kYH); }

  void setX(Tensor& pTensor) { m_Inputs[kX] = &pTensor; }

  void setW(Tensor& pTensor) { m_Inputs[kW] = &pTensor; }

  void setR(Tensor& pTensor) { m_Inputs[kR] = &pTensor; }

  void setB(Tensor& pTensor) { m_Inputs[kB] = &pTensor; }

  void setSequenceLens(Tensor& pTensor) { m_Inputs[kSequenceLens] = &pTensor; }

  void setInitialH(Tensor& pTensor) { m_Inputs[kInitialH] = &pTensor; }

  void setY(Tensor& pTensor) { m_Outputs[kY] = &pTensor; }

  void setYH(Tensor& pTensor) { m_Outputs[kYH] = &pTensor; }

  void print(std::ostream& pOS) const override;

  void dump() const { print(errs()); }

  void accept(ComputeVisitor& pVisitor) { pVisitor.visit(*this); }

private:
  FloatsAttr m_ActivationAlpha;
  FloatsAttr m_ActivationBeta;
  StringsAttr m_Activations;
  FloatAttr m_Clip;
  StringAttr m_Direction;
  IntAttr m_HiddenSize;
  IntAttr m_LinearBeforeReset;
  IntAttr m_OutputSequence;
};

} // namespace of onnc

#endif
