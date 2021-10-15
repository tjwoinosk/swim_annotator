#ifndef SPEEDREPORTER_H
#define SPEEDREPORTER_H

#include <iostream>
#include <boost/timer/timer.hpp>
#include <boost/date_time.hpp>
#include "fileFinder.h"

class SpeedReporter
{
public:
	virtual void reportSpeed() {}
	virtual void startReporter() {}
	virtual bool isBase() { return true; }
	virtual ~SpeedReporter() {}

	void inputMeasurement(boost::timer::cpu_times inMeasurement, int inNumIterations)
	{
		m_measurement = inMeasurement;
		m_numberIterations = inNumIterations;
		produceMeasurementAVG();
	}

protected:
	void produceMeasurementAVG();
	boost::timer::cpu_times m_measurement;
	int m_numberIterations;
};


class DetectionSpeedReporter : public SpeedReporter
{
public:
	void reportSpeed() override;
	void startReporter() override;
	bool isBase() override { return false; };
	bool isReady() { return m_updateFile.is_open(); }
	virtual ~DetectionSpeedReporter() 
	{
		if(m_updateFile.is_open())
			m_updateFile.close();
	}
private:
	std::fstream m_updateFile;
};


class TrackingSpeedReporter : public SpeedReporter
{
public:
	void reportSpeed() override;
	void startReporter() override;
	bool isBase() override { return false; };
	bool isReady() { return m_updateFile.is_open(); }
	virtual ~TrackingSpeedReporter() 
	{
		if (m_updateFile.is_open())
			m_updateFile.close();
	}
private:
	std::fstream m_updateFile;
};


#endif // !SPEEDREPORTER_H

