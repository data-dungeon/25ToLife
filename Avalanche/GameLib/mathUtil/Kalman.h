/*
**
**  File:   Kalman.h
**  Date:   August 24, 2005
**  By:     Bryant Collard
**  Desc:   Classes that implement flavors of a Kalman filter.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef KALMAN_H
#define KALMAN_H

////////////////////////////// CKalmanDerivative ///////////////////////////////

// A Kalman filter that "samples" a single value that is assumed to have a
// constant time derivative. It generates an estimate for the value and its
// derivative at the sample time.

class CKalmanDerivative
{
  public:
	// Construct
	CKalmanDerivative();

	// Set the variance of sample.
	void SetMeasurementVariance(float i_variance);

	// Set the covariance of the model.
	void SetModelCovariance(float i_valueVariance, float i_derivativeVariance,
			float i_covariance);

	// Set the initial error covariance.
	void InitializeErrorCovariance(float i_valueVariance,
			float i_derivativeVariance, float i_covariance);

	// For some given measurement and model covariances and if the time between
	// samples is constant, the error covariance will approach a constant. Run
	// the error covariance update a number of times to try to find that constant
	// state.
	void EstimateErrorCovariance(float i_timeStep, int i_iterations,
			float i_valueVariance, float i_derivativeVariance, float i_covariance);

	// Set the initial sample value and derivative.
	void Initialize(float i_time, float i_value, float i_derivative);

	// Add a new sample.
	void Update(float i_time, float i_value);

	// Get the most recent sample time.
	float LastTime(void) {return(d_time);}

	// Get the estimate of the value at the most recent sample time.
	float LastValue(void) {return(d_value);}

	// Get the estimate of the value at the given time using the estimate of the
	// value at the most recent sample time and the current derivative.
	float Value(float i_time);

	// Get the current estimate of the derivative.
	float Derivative(void) {return(d_derivative);}

	// Get the current error covariance.
	void GetErrorCovariance(float &o_valueVariance,
			float &o_derivativeVariance, float &o_covariance);

  private:
	float d_measurement;
	float d_model[3];
	float d_error[3];
	float d_time;
	float d_value;
	float d_derivative;

	void UpdateErrorCovariance(float d_timeStep, float &o_valueGain,
			float &o_derivativeGain);
};

#endif // KALMAN_H
