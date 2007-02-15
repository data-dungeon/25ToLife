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

// Precompiled header
#include "mathUtil/MathUtilPCH.h"

// My header
#include "mathUtil/Kalman.h"

////////////////////////////// CKalmanDerivative ///////////////////////////////

// A Kalman filter that "samples" a single value that is assumed to have a
// constant time derivative. It generates an estimate for the value and its
// derivative at the sample time.

#define VALUE      0
#define DERIVATIVE 1
#define COVARIANCE 2

/******************************************************************************/
// Construct
/******************************************************************************/
CKalmanDerivative::CKalmanDerivative()
{
	SetMeasurementVariance(0.01f);
	SetModelCovariance(0.0f, 0.0f, 0.0f);
	InitializeErrorCovariance(1.0f, 1.0f, 1.0f);
	Initialize(0.0f, 0.0f, 0.0f);
}

/******************************************************************************/
// Set the variance of sample.
/******************************************************************************/
void CKalmanDerivative::SetMeasurementVariance(float i_variance)
{
	d_measurement = i_variance;
}

/******************************************************************************/
// Set the covariance of the model.
/******************************************************************************/
void CKalmanDerivative::SetModelCovariance(float i_valueVariance,
		float i_derivativeVariance, float i_covariance)
{
	d_model[VALUE] = i_valueVariance;
	d_model[DERIVATIVE] = i_derivativeVariance;
	d_model[COVARIANCE] = i_covariance;
}

/******************************************************************************/
// Set the initial error covariance.
/******************************************************************************/
void CKalmanDerivative::InitializeErrorCovariance(float i_valueVariance,
		float i_derivativeVariance, float i_covariance)
{
	d_error[VALUE] = i_valueVariance;
	d_error[DERIVATIVE] = i_derivativeVariance;
	d_error[COVARIANCE] = i_covariance;
}

/******************************************************************************/
// For some given measurement and model covariances and if the time between
// samples is constant, the error covariance will approach a constant. Run the
// error covariance update a number of times to try to find that constant state.
/******************************************************************************/
void CKalmanDerivative::EstimateErrorCovariance(float i_timeStep,
		int i_iterations, float i_valueVariance, float i_derivativeVariance,
		float i_covariance)
{
	InitializeErrorCovariance(i_valueVariance, i_derivativeVariance,
			i_covariance);

	for (int i = 0; i < i_iterations; i++)
	{
		float valueGain, derivativeGain;
		UpdateErrorCovariance(i_timeStep, valueGain, derivativeGain);
	}
}

/******************************************************************************/
// Set the initial sample value and derivative.
/******************************************************************************/
void CKalmanDerivative::Initialize(float i_time, float i_value,
		float i_derivative)
{
	d_time = i_time;
	d_value = i_value;
	d_derivative = i_derivative;
}

/******************************************************************************/
/******************************************************************************/
void CKalmanDerivative::UpdateErrorCovariance(float d_timeStep,
		float &o_valueGain, float &o_derivativeGain)
{
	// Find the a priori error covariance.
	float temp = d_error[DERIVATIVE] * d_timeStep;
	d_error[VALUE] += (2.0f * d_error[COVARIANCE] + temp) *
			d_timeStep + d_model[VALUE];
	d_error[COVARIANCE] += temp + d_model[COVARIANCE];
	d_error[DERIVATIVE] += d_model[DERIVATIVE];

	// Find the denominator term and try to do something sensible if it is zero.
	temp = d_error[VALUE] + d_measurement;
	if (Math::DivByZero(1.0f, temp))
	{
		ASSERT(false);
		o_valueGain = 0.0f;
		o_derivativeGain = 0.0f;
		d_error[VALUE] = 0.0f;
		d_error[COVARIANCE] = 0.0f;
		return;
	}

	// Find the Kalman gain factors.
	temp = 1.0f / temp;
	o_valueGain = temp * d_error[VALUE];
	o_derivativeGain = temp * d_error[COVARIANCE];

	// Find the a posteriori error covariance.
	d_error[DERIVATIVE] -= d_error[COVARIANCE] * o_derivativeGain;
	d_error[VALUE] = d_measurement * o_valueGain;
	d_error[COVARIANCE] = d_measurement * o_derivativeGain;
}

/******************************************************************************/
// Add a new sample.
/******************************************************************************/
void CKalmanDerivative::Update(float i_time, float i_value)
{
	// Find the time step and update the time.
	float timeStep = i_time - d_time;
	d_time = i_time;

	// Update the error covariance and get the Kalman gain factors.
	float valueGain, derivativeGain;
	UpdateErrorCovariance(timeStep, valueGain, derivativeGain);

	// Find the a priori value estimate at the current time (the a priori
	// derivative estimate is constant) and the measurement error.
	d_value += d_derivative * timeStep;
	float measurementError = i_value - d_value;

	// Find the a posteriori value and derivative estimates.
	d_value += valueGain * measurementError;
	d_derivative += derivativeGain * measurementError;
}

/******************************************************************************/
// Get the estimate of the value at the given time using the estimate of the
// value at the most recent sample time and the current derivative.
/******************************************************************************/
float CKalmanDerivative::Value(float i_time)
{
	return(d_value + d_derivative * (i_time - d_time));
}

/******************************************************************************/
// Get the current error covariance.
/******************************************************************************/
void CKalmanDerivative::GetErrorCovariance(float &o_valueVariance,
		float &o_derivativeVariance, float &o_covariance)
{
	o_valueVariance = d_error[VALUE];
	o_derivativeVariance = d_error[DERIVATIVE];
	o_covariance = d_error[COVARIANCE];
}
