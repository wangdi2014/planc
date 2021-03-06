/* Copyright 2016 Ramakrishnan Kannan */
#ifndef MPI_DISTANLSBPP_HPP_
#define MPI_DISTANLSBPP_HPP_

#include "bppnnls.hpp"
#include "aunmf.hpp"

using namespace std;

template<class INPUTMATTYPE>
class DistANLSBPP : public DistAUNMF<INPUTMATTYPE>{
 private:
  ROWVEC localWnorm;
  ROWVEC Wnorm;

  void allocateMatrices() {
  }

 protected:
  // updateW given HtH and AHt
  void updateW() {
    BPPNNLS<MAT, VEC> subProblem(this->HtH, this->AHtij, true);
    subProblem.solveNNLS();
    this->Wt = subProblem.getSolutionMatrix();
    fixNumericalError<MAT>(&(this->Wt));
    this->W = this->Wt.t();

    // localWnorm = sum(this->W % this->W);
    // tic();
    // MPI_Allreduce(localWnorm.memptr(), Wnorm.memptr(), this->k, MPI_FLOAT,
    //               MPI_SUM, MPI_COMM_WORLD);
    // double temp = toc();
    // this->time_stats.allgather_duration(temp);
    // for (int i = 0; i < this->k; i++) {
    //     this->W.col(i) = this->W.col(i) / sqrt(Wnorm(i));
    // }
    // this->Wt = this->W.t();
  }

  // updateH given WtW and WtA
  void updateH() {
    BPPNNLS<MAT, VEC> subProblem1(this->WtW, this->WtAij, true);
    subProblem1.solveNNLS();
    this->Ht = subProblem1.getSolutionMatrix();
    fixNumericalError<MAT>(&(this->Ht));
    this->H = this->Ht.t();
  }

 public:
  DistANLSBPP(const INPUTMATTYPE& input, const MAT& leftlowrankfactor,
              const MAT& rightlowrankfactor,
              const MPICommunicator& communicator,
              const int numkblks) :
    DistAUNMF<INPUTMATTYPE>(input, leftlowrankfactor,
                            rightlowrankfactor, communicator, numkblks) {
    localWnorm.zeros(this->k);
    Wnorm.zeros(this->k);
    PRINTROOT("DistANLSBPP() constructor successful");
  }

  ~DistANLSBPP() {
    /*
       tempHtH.clear();
       tempWtW.clear();
       tempAHtij.clear();
       tempWtAij.clear();
     */
  }
};  // class DistANLSBPP2D

#endif  // MPI_DISTANLSBPP_HPP_
