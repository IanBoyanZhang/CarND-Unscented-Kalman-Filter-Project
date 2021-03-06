#ifndef TOOLS_H_
#define TOOLS_H_
#include <vector>
#include <cmath>
#include "Eigen/Dense"
#include "measurement_package.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using namespace std;

class Tools {
public:
  /**
  * Constructor.
  */
  Tools();

  /**
  * Destructor.
  */
  virtual ~Tools();

  /**
  * A helper method to calculate RMSE.
  */
  VectorXd CalculateRMSE(const vector<VectorXd> &estimations, const vector<VectorXd> &ground_truth);

  MatrixXd NormalizeAngle(MatrixXd &Z_diff, int dim);

  VectorXd NormalizeAngleVec(VectorXd &z_diff, int dim);

  MatrixXd Cart2Polar(const MatrixXd &Xsig);

  VectorXd Polar2Cart(const MeasurementPackage &meas_package);

  double_t GetTimeDiff(long long curr_time, long long prev_time);
};

#endif /* TOOLS_H_ */