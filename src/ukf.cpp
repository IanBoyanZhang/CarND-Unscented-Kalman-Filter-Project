#include "ukf.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/**
 * Initializes Unscented Kalman filter
 */
UKF::UKF() {
  // if this is false, laser measurements will be ignored (except during init)
  use_laser_ = true;

  // if this is false, radar measurements will be ignored (except during init)
  use_radar_ = true;

  // Process noise standard deviation longitudinal acceleration in m/s^2
  //std_a_ = 30;
/*  std_a_ = 0.2;

  // Process noise standard deviation yaw acceleration in rad/s^2
  //std_yawdd_ = 30;
  std_yawdd_ = 0.2;

  // Laser measurement noise standard deviation position1 in m
  std_laspx_ = 0.15;

  // Laser measurement noise standard deviation position2 in m
  std_laspy_ = 0.15;

  // Radar measurement noise standard deviation radius in m
  std_radr_ = 0.3;

  // Radar measurement noise standard deviation angle in rad
  //std_radphi_ = 0.03;
  std_radphi_ = 0.0175;

  // Radar measurement noise standard deviation radius change in m/s
  //std_radrd_ = 0.3;
  std_radrd_ = 0.1;*/

  /**
  TODO:

  Complete the initialization. See ukf.h for other member properties.

  Hint: one or more values initialized above might be wildly off...
  */
  n_x_ = 5;
  n_aug_ = 7;
  lambda_ = 3;

  is_initialized_ = false;

  // Radar
  // initial state vector
  x_ = VectorXd::Zero(n_aug_);

  // initial covariance matrix
  P_ = MatrixXd::Zero(n_aug_, n_aug_);
}

UKF::~UKF() {}

/**
 * @param {MeasurementPackage} meas_package The latest measurement data of
 * either radar or laser.
 */
void UKF::ProcessMeasurement(MeasurementPackage meas_package) {
  /**
  TODO:

  Complete this function! Make sure you switch between lidar and radar
  measurements.
  */
  MeasurementPackage::SensorType sensorType = GetSensorType(meas_package);
  /**
   * TODO: Refactor initialization assignment
   * Polar to Cart position initialization
   */
  if(!is_initialized_) {
    switch (sensorType) {
      case MeasurementPackage::RADAR:
        fusionUKF.Init(meas_package);
        x_ = fusionUKF.x_;
        P_ = fusionUKF.P_;
        break;
      case MeasurementPackage::LASER:
        return;
//        break;
      default:
        break;
    }
    time_us_ = meas_package.timestamp_;
    is_initialized_ = true;
    return;
  }

  double_t delta_t = meas_package.timestamp_ - time_us_;
  time_us_ = meas_package.timestamp_;
  double_t threshold = 1e-3;

  switch (sensorType) {
    case MeasurementPackage::RADAR:
      break;
    case MeasurementPackage::LASER:
      return;
//      break;
    default:
      break;
  }
  if (delta_t >= threshold) {
    Prediction(delta_t);
  }

  UpdateRadar(meas_package);
}

/**
 * Predicts sigma points, the state, and the state covariance matrix.
 * @param {double} delta_t the change in time (in seconds) between the last
 * measurement and this one.
 */
void UKF::Prediction(double delta_t) {
  /**
  TODO:

  Complete this function! Estimate the object's location. Modify the state
  vector, x_. Predict sigma points, the state, and the state covariance matrix.
  */
  MatrixXd Xsig_aug = fusionUKF._GenerateSigmaPoints();
  fusionUKF._MotionPrediction(Xsig_aug, delta_t);
  VectorXd x_pred = VectorXd::Zero(n_aug_);
  MatrixXd P_pred = MatrixXd::Zero(n_aug_, n_aug_);
  fusionUKF.X_diff = fusionUKF._PredictMeanAndCovariance(&x_pred, &P_pred,
                                                        3, fusionUKF.Xsig_pred_);
  std::cout << "Xsig_pred_:" << fusionUKF.Xsig_pred_ << std::endl;
  std::cout << "Weights:" << fusionUKF.weights_ << std::endl;
  std::cout << "x_pred:" << x_pred << std::endl;
  std::cout << "P_pred:" << P_pred << std::endl;
  fusionUKF.x_pred = x_pred;
  fusionUKF.P_pred = P_pred;
}

/**
 * Updates the state and the state covariance matrix using a laser measurement.
 * @param {MeasurementPackage} meas_package
 */
void UKF::UpdateLidar(MeasurementPackage meas_package) {
  /**
  TODO:

  Complete this function! Use lidar data to update the belief about the object's
  position. Modify the state vector, x_, and covariance, P_.

  You'll also need to calculate the lidar NIS.
  */
  VectorXd z_pred = VectorXd::Zero(n_z_);
  MatrixXd S = MatrixXd::Zero(n_z_, n_z_);
  MatrixXd Zsig = tools.Cart2Polar(fusionUKF.Xsig_pred_);
  MatrixXd Z_diff = fusionUKF._PredictMeanAndCovariance(&z_pred, &S, 1, Zsig);
  fusionUKF._PropagateNoise(&S);
  fusionUKF.Z_diff = Z_diff;
//  Kalman Process
  MatrixXd Tc = fusionUKF._GetCrossCovariance(fusionUKF.X_diff, fusionUKF.Z_diff);
  MatrixXd K = Tc * S.inverse();
  VectorXd z_diff = meas_package.raw_measurements_ - z_pred;

  // angle normalization
  z_diff = tools.NormalizeAngleVec(z_diff, 1);

  // update state mean and covariance matrix
  fusionUKF.x_ = fusionUKF.x_ + K * z_diff;
  fusionUKF.P_ = fusionUKF.P_ - K * S * K.transpose();

  x_ = fusionUKF.x_;
  P_ = fusionUKF.P_;
}

/**
 * Updates the state and the state covariance matrix using a radar measurement.
 * @param {MeasurementPackage} meas_package
 */
void UKF::UpdateRadar(MeasurementPackage meas_package) {
  /**
  TODO:

  Complete this function! Use radar data to update the belief about the object's
  position. Modify the state vector, x_, and covariance, P_.

  You'll also need to calculate the radar NIS.
  */
}

MeasurementPackage::SensorType UKF::GetSensorType(
        const MeasurementPackage &measurement_pack) {
  return measurement_pack.sensor_type_;
}
